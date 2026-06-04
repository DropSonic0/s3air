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
	void create(int format, int width, int height);
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

	void attachTexture(int attachment, unsigned int handle, int texTarget = GL_TEXTURE_2D);
	void attachTexture(int attachment, const Texture* texture, int texTarget = GL_TEXTURE_2D);

	void attachRenderbuffer(int attachment, unsigned int handle);
	void createRenderbuffer(int attachment, int internalformat);

	void bind();
	void unbind();

	void activate();
	void activate(int clearmask);
	void deactivate();

	inline unsigned int getHandle() const   { return mHandle; }
	inline Recti getViewport() const  { return Recti(0, 0, mWidth, mHeight); }

private:
	void deleteAttachedBuffer(int attachment);

private:
	unsigned int mHandle;
	int mWidth;
	int mHeight;
	std::map<int, Renderbuffer*> mRenderbuffers;
};

#endif
