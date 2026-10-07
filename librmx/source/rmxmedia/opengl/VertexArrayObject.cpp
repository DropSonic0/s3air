/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"

#ifdef RMX_WITH_OPENGL_SUPPORT

// OpenGL ES 2 does not support actual vertex array objects - but despite its name, the VertexArrayObject class will still without them (though a bit less efficient in that case)
// PS3 (PSGL + Cg) neither: see the RMX_OPENGL_PS3_CG branches
#if !defined(RMX_USE_GLES2) && !defined(RMX_OPENGL_PS3_CG)
	#define RMX_OPENGL_SUPPORT_VAO
#endif

#ifdef RMX_OPENGL_PS3_CG
	#include <Cg/cg.h>
	#include <Cg/cgGL.h>
	CGparameter Shader_getBoundVertexAttribute(const char* name);	// Implemented in Shader_PS3.cpp
#endif

namespace opengl
{

#ifdef RMX_OPENGL_PS3_CG
	VertexArrayObject* VertexArrayObject::sCurrent = nullptr;
#endif

	VertexArrayObject::~VertexArrayObject()
	{
	#ifdef RMX_OPENGL_PS3_CG
		if (sCurrent == this)
			sCurrent = nullptr;
	#else
		#ifdef RMX_OPENGL_SUPPORT_VAO
			if (mVertexArrayObjectHandle != 0)
				glDeleteVertexArrays(1, &mVertexArrayObjectHandle);
		#endif

		if (mVertexBufferObjectHandle != 0)
			glDeleteBuffers(1, &mVertexBufferObjectHandle);
	#endif
	}

	void VertexArrayObject::setup(Format format)
	{
	#ifdef RMX_OPENGL_PS3_CG
		mVertexBufferObjectHandle = 1;		// Only a marker for "isValid()", there is no real GL buffer on PS3
		mCurrentFormat = format;
		applyCurrentFormat();
	#else
		const bool needsInitialization = (mVertexBufferObjectHandle == 0);
		#ifdef RMX_OPENGL_SUPPORT_VAO
			if (needsInitialization)
			{
				glGenVertexArrays(1, &mVertexArrayObjectHandle);
				glGenBuffers(1, &mVertexBufferObjectHandle);
				glBindVertexArray(mVertexArrayObjectHandle);
				glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectHandle);
			}
			else
			{
				glBindVertexArray(mVertexArrayObjectHandle);
			}
		#else
			if (needsInitialization)
			{
				glGenBuffers(1, &mVertexBufferObjectHandle);
				glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectHandle);
			}
		#endif

		mCurrentFormat = format;
		applyCurrentFormat();
	#endif
	}

	void VertexArrayObject::updateVertexData(const float* vertexData, size_t numVertices)
	{
		if (mVertexBufferObjectHandle == 0)
		{
			RMX_ASSERT(false, "VAO must be setup with a format before updating data");
			return;
		}

	#ifdef RMX_OPENGL_PS3_CG
		// Keep a copy in client memory (PSGL streams client arrays itself at draw time) -- no GL buffer re-allocation per update
		mClientData.assign(vertexData, vertexData + mFloatsPerVertex * numVertices);
		mNumBufferedVertices = numVertices;
	#else
		#ifdef RMX_OPENGL_SUPPORT_VAO
			glBindVertexArray(mVertexArrayObjectHandle);
		#endif
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(mFloatsPerVertex * numVertices * sizeof(GLfloat)), vertexData, GL_STATIC_DRAW);
		mNumBufferedVertices = numVertices;
	#endif
	}

	void VertexArrayObject::bind()
	{
	#ifdef RMX_OPENGL_PS3_CG
		// Nothing to apply yet: the attribute pointers are set in ps3DrawArrays, once the vertex program is known
		sCurrent = this;
	#elif defined(RMX_OPENGL_SUPPORT_VAO)
		// Bind the VAO, which will implicitly bind the VBO
		if (mVertexArrayObjectHandle != 0)
		{
			glBindVertexArray(mVertexArrayObjectHandle);
		}
	#else
		// Explicitly bind the VAO, and apply the format
		if (mVertexBufferObjectHandle != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectHandle);
			applyCurrentFormat();
		}
	#endif
	}

	void VertexArrayObject::unbind() const
	{
	#ifdef RMX_OPENGL_PS3_CG
		sCurrent = nullptr;
	#elif defined(RMX_OPENGL_SUPPORT_VAO)
		glBindVertexArray(0);
	#else
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	#endif
	}

	void VertexArrayObject::draw(GLenum mode)
	{
		if (mNumBufferedVertices > 0)
		{
			bind();
			glDrawArrays(mode, 0, (GLsizei)mNumBufferedVertices);	// On PS3 this is ps3DrawArrays (see PS3GLCompat.h)
		}
	}

	void VertexArrayObject::applyCurrentFormat()
	{
	#ifdef RMX_OPENGL_PS3_CG
		// Only the layout is needed here, nothing is sent to GL
		switch (mCurrentFormat)
		{
			case Format::P2:		mNumVertexAttributes = 1;  mFloatsPerVertex = 2;  break;
			case Format::P2_C3:		mNumVertexAttributes = 2;  mFloatsPerVertex = 5;  break;
			case Format::P2_C4:		mNumVertexAttributes = 2;  mFloatsPerVertex = 6;  break;
			case Format::P2_T2:		mNumVertexAttributes = 2;  mFloatsPerVertex = 4;  break;
			case Format::P3_C3:		mNumVertexAttributes = 2;  mFloatsPerVertex = 6;  break;
			case Format::P3_N3_C3:	mNumVertexAttributes = 3;  mFloatsPerVertex = 9;  break;
			default:
				RMX_ERROR("Unrecognized or invalid format", );
				break;
		}
	#else
		switch (mCurrentFormat)
		{
			case Format::P2:
			{
				mNumVertexAttributes = 1;
				mFloatsPerVertex = 2;
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				break;
			}

			case Format::P2_C3:
			{
				mNumVertexAttributes = 2;
				mFloatsPerVertex = 5;
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(2 * sizeof(float)));	// Colors
				break;
			}

			case Format::P2_C4:
			{
				mNumVertexAttributes = 2;
				mFloatsPerVertex = 6;
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(2 * sizeof(float)));	// Colors
				break;
			}

			case Format::P2_T2:
			{
				mNumVertexAttributes = 2;
				mFloatsPerVertex = 4;
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(2 * sizeof(float)));	// Texcoords
				break;
			}

			case Format::P3_C3:
			{
				mNumVertexAttributes = 2;
				mFloatsPerVertex = 6;
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(3 * sizeof(float)));	// Colors
				break;
			}

			case Format::P3_N3_C3:
			{
				mNumVertexAttributes = 3;
				mFloatsPerVertex = 9;
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(0 * sizeof(float)));	// Positions
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(3 * sizeof(float)));	// Normals
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (GLsizei)(mFloatsPerVertex * sizeof(float)), (void*)(6 * sizeof(float)));	// Colors
				break;
			}

			default:
				RMX_ERROR("Unrecognized or invalid format", );
				break;
		}

		for (size_t i = 0; i < mNumVertexAttributes; ++i)
		{
			glEnableVertexAttribArray((GLuint)i);
		}
		for (size_t i = mNumVertexAttributes; i < 4; ++i)	// Assuming we'll never use more than 4 vertex attributes inside here
		{
			glDisableVertexAttribArray((GLuint)i);
		}
	#endif
	}

}


#ifdef RMX_OPENGL_PS3_CG

// From here on, glDrawArrays must be the real one again
#undef glDrawArrays

namespace
{
	struct Ps3Attribute
	{
		const char* mName;
		int mSize;
		int mOffset;
	};

	inline void setPs3Attribute(Ps3Attribute& attribute, const char* name, int size, int offset)
	{
		attribute.mName = name;
		attribute.mSize = size;
		attribute.mOffset = offset;
	}
}

void ps3DrawArrays(GLenum mode, GLint first, GLsizei count)
{
	using opengl::VertexArrayObject;

	CGparameter enabled[4];
	int numEnabled = 0;

	VertexArrayObject* vao = VertexArrayObject::sCurrent;
	if (nullptr != vao && !vao->mClientData.empty())
	{
		Ps3Attribute attributes[3];
		int numAttributes = 0;

		switch (vao->mCurrentFormat)
		{
			case VertexArrayObject::Format::P2:
				setPs3Attribute(attributes[numAttributes++], "position", 2, 0);
				break;
			case VertexArrayObject::Format::P2_C3:
				setPs3Attribute(attributes[numAttributes++], "position", 2, 0);
				setPs3Attribute(attributes[numAttributes++], "color", 3, 2);
				break;
			case VertexArrayObject::Format::P2_C4:
				setPs3Attribute(attributes[numAttributes++], "position", 2, 0);
				setPs3Attribute(attributes[numAttributes++], "color", 4, 2);
				break;
			case VertexArrayObject::Format::P2_T2:
				setPs3Attribute(attributes[numAttributes++], "position", 2, 0);
				setPs3Attribute(attributes[numAttributes++], "texcoords0", 2, 2);
				break;
			case VertexArrayObject::Format::P3_C3:
				setPs3Attribute(attributes[numAttributes++], "position", 3, 0);
				setPs3Attribute(attributes[numAttributes++], "color", 3, 3);
				break;
			case VertexArrayObject::Format::P3_N3_C3:
				setPs3Attribute(attributes[numAttributes++], "position", 3, 0);
				setPs3Attribute(attributes[numAttributes++], "normal", 3, 3);
				setPs3Attribute(attributes[numAttributes++], "color", 3, 6);
				break;
			default:
				break;
		}

		const float* base = &vao->mClientData[0];
		const GLsizei stride = (GLsizei)(vao->mFloatsPerVertex * sizeof(float));
		for (int k = 0; k < numAttributes; ++k)
		{
			// Attributes are looked up by their argument name in the Cg "vmain" of the currently bound vertex program
			const CGparameter p = Shader_getBoundVertexAttribute(attributes[k].mName);
			if (p != 0)
			{
				cgGLSetParameterPointer(p, attributes[k].mSize, GL_FLOAT, stride, base + attributes[k].mOffset);
				cgGLEnableClientState(p);
				enabled[numEnabled++] = p;
			}
		}
	}

	glDrawArrays(mode, first, count);

	for (int k = 0; k < numEnabled; ++k)
	{
		cgGLDisableClientState(enabled[k]);
	}
}

#endif

#endif