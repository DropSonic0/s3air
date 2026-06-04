/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	Shader
*		Support for GLSL shaders.
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

#include <functional>


// Shader class
class API_EXPORT Shader
{
public:
	enum ShaderType
	{
		ShaderType_FRAGMENT,
		ShaderType_VERTEX
	};

	enum BlendMode
	{
		BlendMode_UNDEFINED = -1,
		BlendMode_OPAQUE,
		BlendMode_ALPHA,
		BlendMode_ADD
	};

#if !defined(PLATFORM_PS3)
	static std::function<void(String&, ShaderType)> mShaderSourcePostProcessCallback;
	static std::function<bool(BlendMode)> mShaderApplyBlendModeCallback;
#endif
	 
	 				// Internal application of blend function will only be done if this is not set, or returns false

public:
	Shader();
	~Shader();

	bool compile(const String& vsSource, const String& fsSource, const std::map<int, String>* vertexAttribMap = nullptr);

	inline const String& getVertexSource() const	{ return mVertexSource; }
	inline const String& getFragmentSource() const	{ return mFragmentSource; }
	inline const String& getCompileLog() const		{ return mCompileLog; }

	inline BlendMode getBlendMode() const			{ return mBlendMode; }
	inline void setBlendMode(BlendMode mode)		{ mBlendMode = mode; }

	inline unsigned int getProgramHandle() const			{ return mProgram; }

	unsigned int getUniformLocation(const char* name) const;
	unsigned int getAttribLocation(const char* name) const;

	void setParam(const char* name, int param);
	void setParam(const char* name, const Vec2i& param);
	void setParam(const char* name, const Vec3i& param);
	void setParam(const char* name, const Vec4i& param);
	void setParam(const char* name, float param);
	void setParam(const char* name, const Vec2f& param);
	void setParam(const char* name, const Vec3f& param);
	void setParam(const char* name, const Vec4f& param);

	void setMatrix(const char* name, const Mat3f& matrix);
	void setMatrix(const char* name, const Mat4f& matrix);

	void setTexture(const char* name, unsigned int handle, int target);
	void setTexture(const char* name, const Texture& texture);

	void bind();
	void unbind();

	bool load(const String& filename, const String& techname = String(), const String& additionalDefines = String());
	bool load(const std::vector<uint8>& content, const String& techname = String(), const String& additionalDefines = String());

private:
	bool compileShader(int shaderType, unsigned int& shaderHandle, const String& source);
	bool linkProgram(const std::map<int, String>* vertexAttribMap = nullptr);

private:
	unsigned int mVertexShader;
	unsigned int mFragmentShader;
	unsigned int mProgram;
	BlendMode mBlendMode;
	int mTextureCount;

	String mVertexSource;
	String mFragmentSource;
	String mCompileLog;
};


// ShaderEffect class
class API_EXPORT ShaderEffect
{
public:
	ShaderEffect();
	~ShaderEffect();

	bool load(const String& filename);
	bool load(const std::vector<uint8>& content);
	bool loadFromString(const String& content);
	bool getShader(Shader& shader, int index = 0, const String& additionalDefines = String());
	bool getShader(Shader& shader, const String& name, const String& additionalDefines = String());

private:
	struct PartStruct
	{
		String mTitle;
		String mContent;
	};

	struct TechniqueStruct
	{
		String mName;
		std::vector<String> mVertexShaderParts;
		std::vector<String> mFragmentShaderParts;
		std::vector<String> mDefines;
		std::map<int, String> mVertexAttribMap;
		Shader::BlendMode mBlendMode;
	};

private:
	TechniqueStruct* findTechniqueByName(const String& name);
	bool getShaderInternal(Shader& shader, TechniqueStruct& tech, const String& additionalDefines);
	void readParts(const String& source);
	void parseTechniques(String& source);
	PartStruct* findPartByName(const String& name);
	void buildSourceFromParts(String& source, const std::vector<String>& parts, const String& definitions);
	void preprocessSource(String& source, Shader::ShaderType shaderType);

private:
	std::vector<PartStruct> mParts;
	std::vector<TechniqueStruct> mTechniques;
	String mIncludeDir;
};

#endif
