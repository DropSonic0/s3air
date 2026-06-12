/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

#include "rmxmedia_externals.h"


namespace rmx
{
	class OpenGLHelper
	{
	public:
	#if defined(RMX_USE_GLES2) || defined(PLATFORM_PS3)
	#if defined(PLATFORM_PS3)
		static constexpr GLint FORMAT_RGB   = GL_ARGB_SCE;
		static constexpr GLint FORMAT_RGBA  = GL_ARGB_SCE;
	#else
		static constexpr GLint FORMAT_RGB   = GL_RGBA;				// OpenGL ES 2.0 does not have GL_RGB, so we have to use GL_RGBA instead
		static constexpr GLint FORMAT_RGBA  = GL_RGBA;
	#endif
		static constexpr GLint FORMAT_DEPTH = GL_DEPTH_COMPONENT16;	// ES 2.0 / PS3 PSGL uses GL_DEPTH_COMPONENT16
	#else
		static constexpr GLint FORMAT_RGB   = GL_RGB8;
		static constexpr GLint FORMAT_RGBA  = GL_RGBA8;
		static constexpr GLint FORMAT_DEPTH = GL_DEPTH_COMPONENT;
	#endif
	};
}

#endif
