/*
*	Cg implementation of the rmx "Shader" class for PS3 (PSGL).
*
*	Idea: keep the whole .shader pipeline (ShaderEffect: sections, techniques, "define = ...", inheritance)
*	and only swap what happens at the end:
*	  - GLSL:  glCreateShader / glLinkProgram / glUniform*      (not available in PSGL)
*	  - Cg:    cgCreateProgram(CG_SOURCE) x2 / cgGLSetParameter* / cgGLSetTextureParameter
*
*	How to integrate (see the notes in the chat):
*	  1. Shader.h:   add the two lines marked "PS3" (public CgData declaration + pointer).
*	  2. Shader.cpp: wrap the GLSL-only Shader functions in "#if !PS3" and put this file's content under "#else".
*	     Shader::load(...) (both overloads), Shader::unbind(), the constructor and setTexture(loc, Texture) stay common.
*	     ShaderEffect is not touched.
*
*	Conventions in the Cg .shader files:
*	  - entry points "vmain" / "fmain"
*	  - vertex attribute argument names are the names used in "vertexattrib[n] = name;" (position, texcoords0, color)
*	  - samplers are plain "uniform sampler2D Name;" (no TEXUNIT semantic): they are bound with cgGLSetTextureParameter
*	  - integer uniforms from C++ (int, Vec2i, ..., Recti) arrive as float uniforms
*/

#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__)

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>


// ---- Shader.h additions (PS3) ----------------------------------------------------------------------------------
//
//	public:
//		struct CgData;						// PS3
//	private:
//		CgData* mCgData = nullptr;			// PS3
//
// -----------------------------------------------------------------------------------------------------------------


namespace
{
	const GLuint INVALID_LOCATION = 0xffffffff;

	CGcontext sCgContext = 0;
	CGprofile sVertexProfile;
	CGprofile sFragmentProfile;
	CGprogram sBoundVertexProgram = 0;

	void ensureCgInitialized()
	{
		if (sCgContext != 0)
			return;

		cgRTCgcInit();		// runtime compiler, same as in RSDKv4-PS3
		sCgContext = cgCreateContext();
		sVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		sFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	}
}


struct Shader::CgData
{
	struct Slot
	{
		CGparameter mVertex = 0;
		CGparameter mFragment = 0;
	};

	CGprogram mVertexProgram = 0;
	CGprogram mFragmentProgram = 0;
	std::vector<Slot> mSlots;							// "Uniform location" returned to the callers is an index into this vector

	struct NamedLocation
	{
		std::string mName;
		GLuint mLocation = 0xffffffff;					// Includes negative results (INVALID_LOCATION), so lookups of removed uniforms stay cheap
	};
	std::vector<NamedLocation> mNamedLocations;			// Small (a handful of uniforms per shader): linear search with strcmp, no allocations per lookup

	std::map<int, String> mVertexAttribMap;
};


namespace
{
	void setFloats(const Shader::CgData* cg, GLuint loc, const float* v, int n)
	{
		if (nullptr == cg || loc >= cg->mSlots.size())
			return;		// Unknown or optimized-away uniform: silently ignore

		const Shader::CgData::Slot& slot = cg->mSlots[loc];
		for (int k = 0; k < 2; ++k)
		{
			const CGparameter p = (k == 0) ? slot.mVertex : slot.mFragment;
			if (p == 0)
				continue;

			switch (n)
			{
				case 1:  cgGLSetParameter1f(p, v[0]);  break;
				case 2:  cgGLSetParameter2f(p, v[0], v[1]);  break;
				case 3:  cgGLSetParameter3f(p, v[0], v[1], v[2]);  break;
				case 4:  cgGLSetParameter4f(p, v[0], v[1], v[2], v[3]);  break;
			}
		}
	}
}


// Used by the PS3 VertexArrayObject: finds the attribute parameter (by its argument name in "vmain") of the currently bound vertex program
CGparameter Shader_getBoundVertexAttribute(const char* name)
{
	return (sBoundVertexProgram != 0) ? cgGetNamedParameter(sBoundVertexProgram, name) : (CGparameter)0;
}


void Shader::unbindShader()
{
	if (sCgContext != 0)
	{
		cgGLDisableProfile(sVertexProfile);
		cgGLDisableProfile(sFragmentProfile);
	}
	sBoundVertexProgram = 0;
	glActiveTexture(GL_TEXTURE0);
}

// Called by the glUseProgram(0) shim in rmxmedia_externals.h, so existing "glUseProgram(0)" calls (e.g. in OpenGLRenderer) also switch the Cg profiles off
void Shader_unbindCg()
{
	Shader::unbindShader();
}

Shader::~Shader()
{
	if (nullptr != mCgData)
	{
		if (sBoundVertexProgram == mCgData->mVertexProgram)
			sBoundVertexProgram = 0;
		if (mCgData->mVertexProgram != 0)
			cgDestroyProgram(mCgData->mVertexProgram);
		if (mCgData->mFragmentProgram != 0)
			cgDestroyProgram(mCgData->mFragmentProgram);
		delete mCgData;
		mCgData = nullptr;
	}
}

bool Shader::compile(const String& vsSource, const String& fsSource, const std::map<int, String>* vertexAttribMap)
{
	ensureCgInitialized();

	mVertexSource = vsSource;
	mFragmentSource = fsSource;

	if (nullptr == mCgData)
		mCgData = new CgData();
	if (nullptr != vertexAttribMap)
		mCgData->mVertexAttribMap = *vertexAttribMap;

	mCgData->mVertexProgram = cgCreateProgram(sCgContext, CG_SOURCE, *vsSource, sVertexProfile, "vmain", NULL);
	if (mCgData->mVertexProgram == 0)
	{
		const char* listing = cgGetLastListing(sCgContext);
		mCompileLog << "Error(s) compiling vertex shader:\n" << listing;
		printf("[Cg] Error(s) compiling vertex shader:\n%s\n", listing);
		return false;
	}

	mCgData->mFragmentProgram = cgCreateProgram(sCgContext, CG_SOURCE, *fsSource, sFragmentProfile, "fmain", NULL);
	if (mCgData->mFragmentProgram == 0)
	{
		const char* listing = cgGetLastListing(sCgContext);
		mCompileLog << "Error(s) compiling fragment shader:\n" << listing;
		printf("[Cg] Error(s) compiling fragment shader:\n%s\n", listing);
		return false;
	}

	cgGLLoadProgram(mCgData->mVertexProgram);
	cgGLLoadProgram(mCgData->mFragmentProgram);
	return true;
}

GLuint Shader::getUniformLocation(const char* name) const
{
	if (nullptr == mCgData || mCgData->mVertexProgram == 0 || mCgData->mFragmentProgram == 0)
		return INVALID_LOCATION;

	// Fast path: called per draw call by name (e.g. "MainTexture"), so avoid allocating anything here
	for (const CgData::NamedLocation& entry : mCgData->mNamedLocations)
	{
		if (strcmp(entry.mName.c_str(), name) == 0)
			return entry.mLocation;
	}

	CgData::Slot slot;
	slot.mVertex = cgGetNamedParameter(mCgData->mVertexProgram, name);
	slot.mFragment = cgGetNamedParameter(mCgData->mFragmentProgram, name);

	GLuint loc = INVALID_LOCATION;
	if (slot.mVertex != 0 || slot.mFragment != 0)
	{
		loc = (GLuint)mCgData->mSlots.size();
		mCgData->mSlots.push_back(slot);
	}
	// No assert if missing: cgc may optimize away unused uniforms, which is fine here
	CgData::NamedLocation entry;
	entry.mName = name;
	entry.mLocation = loc;
	mCgData->mNamedLocations.push_back(entry);
	return loc;
}

GLuint Shader::getAttribLocation(const char* name) const
{
	// With Cg the attribute slot comes from the semantic, not from a bound index; this only reports the index from the technique's "vertexattrib[n]"
	if (nullptr != mCgData)
	{
		for (const auto& entry : mCgData->mVertexAttribMap)
		{
			if (entry.second == name)
				return (GLuint)entry.first;
		}
	}
	return INVALID_LOCATION;
}

void Shader::setParam(GLuint loc, int param)
{
	const float v[1] = { (float)param };
	setFloats(mCgData, loc, v, 1);
}

void Shader::setParam(GLuint loc, const Vec2i& param)
{
	const float v[2] = { (float)param.x, (float)param.y };
	setFloats(mCgData, loc, v, 2);
}

void Shader::setParam(GLuint loc, const Vec3i& param)
{
	const float v[3] = { (float)param.x, (float)param.y, (float)param.z };
	setFloats(mCgData, loc, v, 3);
}

void Shader::setParam(GLuint loc, const Vec4i& param)
{
	const float v[4] = { (float)param.x, (float)param.y, (float)param.z, (float)param.w };
	setFloats(mCgData, loc, v, 4);
}

void Shader::setParam(GLuint loc, const Recti& param)
{
	const float v[4] = { (float)param.mData[0], (float)param.mData[1], (float)param.mData[2], (float)param.mData[3] };
	setFloats(mCgData, loc, v, 4);
}

void Shader::setParam(GLuint loc, float param)
{
	setFloats(mCgData, loc, &param, 1);
}

void Shader::setParam(GLuint loc, const Vec2f& param)
{
	const float v[2] = { param.x, param.y };
	setFloats(mCgData, loc, v, 2);
}

void Shader::setParam(GLuint loc, const Vec3f& param)
{
	const float v[3] = { param.x, param.y, param.z };
	setFloats(mCgData, loc, v, 3);
}

void Shader::setParam(GLuint loc, const Vec4f& param)
{
	const float v[4] = { param.x, param.y, param.z, param.w };
	setFloats(mCgData, loc, v, 4);
}

void Shader::setParam(GLuint loc, const Rectf& param)
{
	setFloats(mCgData, loc, param.mData, 4);
}

void Shader::setMatrix(GLuint loc, const Mat3f& matrix)
{
	if (nullptr == mCgData || loc >= mCgData->mSlots.size())
		return;
	const CgData::Slot& slot = mCgData->mSlots[loc];
	if (slot.mVertex != 0)	 cgGLSetMatrixParameterfr(slot.mVertex, *matrix);
	if (slot.mFragment != 0) cgGLSetMatrixParameterfr(slot.mFragment, *matrix);
}

void Shader::setMatrix(GLuint loc, const Mat4f& matrix)
{
	if (nullptr == mCgData || loc >= mCgData->mSlots.size())
		return;
	const CgData::Slot& slot = mCgData->mSlots[loc];
	if (slot.mVertex != 0)	 cgGLSetMatrixParameterfr(slot.mVertex, *matrix);
	if (slot.mFragment != 0) cgGLSetMatrixParameterfr(slot.mFragment, *matrix);
}

void Shader::setTexture(GLuint loc, GLuint handle, GLenum target)
{
	// The Cg runtime picks the texture unit of the sampler parameter, so no glActiveTexture / glUniform1i here
	//  -> "target" is always GL_TEXTURE_2D on PS3
	if (nullptr != mCgData && loc < mCgData->mSlots.size())
	{
		const CGparameter p = mCgData->mSlots[loc].mFragment;
		if (p != 0)
		{
			cgGLSetTextureParameter(p, handle);
			cgGLEnableTextureParameter(p);
		}
	}
	++mTextureCount;
}

void Shader::bind()
{
	if (mBlendMode != BlendMode::UNDEFINED)
	{
		bool handled = false;
		if (mShaderApplyBlendModeCallback)
		{
			handled = Shader::mShaderApplyBlendModeCallback(mBlendMode);
		}

		if (!handled)
		{
			switch (mBlendMode)
			{
				case BlendMode::OPAQUE:	glBlendFunc(GL_ONE, GL_ZERO);  break;
				case BlendMode::ALPHA:	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  break;
				case BlendMode::ADD:	glBlendFunc(GL_ONE, GL_ONE);   break;
				default:				glBlendFunc(GL_ONE, GL_ZERO);  break;
			}
		}
	}

	if (nullptr != mCgData && mCgData->mVertexProgram != 0 && mCgData->mFragmentProgram != 0)
	{
		cgGLBindProgram(mCgData->mVertexProgram);
		cgGLBindProgram(mCgData->mFragmentProgram);
		cgGLEnableProfile(sVertexProfile);
		cgGLEnableProfile(sFragmentProfile);
		sBoundVertexProgram = mCgData->mVertexProgram;
	}
	resetTextureCount();
}

#endif
