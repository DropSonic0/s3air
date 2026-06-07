/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	Framebuffer
*		OpenGL framebuffer wrapper classes.
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT


class API_EXPORT Renderbuffer
{
public:
	Renderbuffer();
	~Renderbuffer();

	void create();
	void create(unsigned int format, int width, int height);
	void setSize(int width, int height);
	void destroy();

	inline unsigned int getHandle() const  { return mHandle; }

private:
	unsigned int mHandle;
	int mFormat;
	int mWidth;
	int mHeight;
};


class API_EXPORT Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void create();
	void create(int width, int height);
	void finishCreation();
	void destroy();
	void setSize(int width, int height);

	void attachTexture(unsigned int attachment, unsigned int handle, unsigned int texTarget = 0x0DE1); // 0x0DE1 = GL_TEXTURE_2D
	void attachTexture(unsigned int attachment, const Texture* texture, unsigned int texTarget = 0x0DE1);

	void attachRenderbuffer(unsigned int attachment, unsigned int handle);
	void createRenderbuffer(unsigned int attachment, unsigned int internalformat);

	void bind();
	void unbind();

	void activate();
	void activate(GLbitfield clearmask);
	void deactivate();

	inline unsigned int getHandle() const   { return mHandle; }
	inline Recti getViewport() const  { return Recti(0, 0, mWidth, mHeight); }

private:
	void deleteAttachedBuffer(unsigned int attachment);

private:
	unsigned int mHandle;
	int mWidth;
	int mHeight;
	std::map<unsigned int, Renderbuffer*> mRenderbuffers;
};

#endif
