/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


// Library linking via pragma
#if defined(PLATFORM_WINDOWS) && defined(RMX_LIB)
	#pragma comment(lib, "sdl2main.lib")
	#pragma comment(lib, "sdl2.lib")
	#pragma comment(lib, "winmm.lib")
	#pragma comment(lib, "imm32.lib")
	#pragma comment(lib, "version.lib")
	#pragma comment(lib, "setupapi.lib")
	#pragma comment(lib, "opengl32.lib")
#endif

// This is for some reason needed under Linux
#if defined(__GNUC__) && __GNUC__ >= 4
	#define DECLSPEC __attribute__ ((visibility("default")))
#endif


// SDL
#ifdef PLATFORM_WINDOWS
	// Needed for MSYS2
	#if defined(__GNUC__)
		#include <SDL2/SDL.h>
	#else
		#include <SDL/SDL.h>
	#endif

#elif defined(PLATFORM_LINUX)
	#include <SDL2/SDL.h>

#elif !defined(__CELLOS_LV2__) && !defined(__SNC__)
	#include <SDL.h>
#endif


// OpenGL
#if defined(PLATFORM_WINDOWS)
	#define ALLOW_LEGACY_OPENGL
	#define RMX_USE_GLEW

#elif defined(PLATFORM_LINUX)
	#if defined(RMX_LINUX_ENFORCE_GLES2)	// Build option: Use OpenGL ES 2
		#define RMX_USE_GLES2
		#define GL_GLEXT_PROTOTYPES
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#else
		#define RMX_USE_GLEW
	#endif

#elif defined(PLATFORM_MAC)
	#define ALLOW_LEGACY_OPENGL		// Should be removed for macOS I guess?
	#include <OpenGL/gl3.h>
	#include <OpenGL/glu.h>

#elif defined(PLATFORM_WEB)
	#include <GL/glew.h>

#elif defined(PLATFORM_ANDROID)
	#define RMX_USE_GLES2
	#define GL_GLEXT_PROTOTYPES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

#elif defined(PLATFORM_IOS)
	#define RMX_USE_GLES2
	#define GL_GLEXT_PROTOTYPES
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>

#elif defined(PLATFORM_SWITCH)
	#include <EGL/egl.h>    // EGL library
	#include <EGL/eglext.h> // EGL extensions
	#include <glad/glad.h>  // glad library (OpenGL loader)
	#define RMX_USE_GLAD
	#define GL_LUMINANCE GL_RED

#elif defined(PLATFORM_VITA)
	#include <vitaGL.h>
	#define RMX_USE_GLES2

#elif defined(PLATFORM_PS3)
	// PS3 does not use standard SDL or OpenGL/GLES2 headers from here
	typedef struct SDL_mutex SDL_mutex;
	typedef struct SDL_Thread SDL_Thread;
	typedef struct SDL_cond SDL_cond;
	typedef struct SDL_Window SDL_Window;
	typedef int SDL_AudioDeviceID;
	
	struct SDL_AudioSpec {
		int freq;
		int format;
		int channels;
		int samples;
		void (*callback)(void*, unsigned char*, int);
		void* userdata;
	};
	
	typedef struct SDL_Keysym {
		int sym;
		int scancode;
		int mod;
	} SDL_Keysym;

	struct SDL_KeyboardEvent {
		int type;
		int repeat;
		SDL_Keysym keysym;
	};
	
	struct SDL_TextInputEvent {
		char text[32];
	};
	
	struct SDL_MouseButtonEvent {
		int type;
		int button;
		int x;
		int y;
	};
	
	struct SDL_MouseWheelEvent {
		int y;
	};
	
	typedef struct SDL_WindowEvent {
		int event;
		int data1;
		int data2;
	} SDL_WindowEvent;
	
	typedef struct SDL_MouseMotionEvent {
		int x;
		int y;
	} SDL_MouseMotionEvent;
	
	union SDL_Event {
		int type;
		SDL_KeyboardEvent key;
		SDL_TextInputEvent text;
		SDL_MouseButtonEvent button;
		SDL_MouseWheelEvent wheel;
		SDL_WindowEvent window;
		SDL_MouseMotionEvent motion;
	};
	
	#define SDLK_SCANCODE_MASK 0
	
	static inline SDL_mutex* SDL_CreateMutex() { return nullptr; }
	static inline void SDL_DestroyMutex(SDL_mutex*) {}
	static inline int SDL_LockMutex(SDL_mutex*) { return 0; }
	static inline int SDL_UnlockMutex(SDL_mutex*) { return 0; }
	static inline SDL_cond* SDL_CreateCond() { return nullptr; }
	static inline void SDL_DestroyCond(SDL_cond*) {}
	static inline int SDL_CondSignal(SDL_cond*) { return 0; }
	static inline void SDL_Delay(unsigned int) {}
	static inline int SDL_CondWaitTimeout(SDL_cond*, SDL_mutex*, unsigned int) { return 0; }
	static inline SDL_Thread* SDL_CreateThread(int (*)(void*), const char*, void*) { return nullptr; }
	static inline void SDL_WaitThread(SDL_Thread*, int*) {}

	#define SDL_INIT_AUDIO 0
	#define SDL_INIT_VIDEO 0
	static inline int SDL_Init(unsigned int) { return 0; }
	static inline void SDL_Quit() {}
	static inline int SDL_InitSubSystem(unsigned int) { return 0; }
	
	#define SDL_QUIT 1
	#define SDL_WINDOWEVENT 2
	#define SDL_WINDOWEVENT_RESIZED 3
	#define SDL_WINDOWEVENT_SIZE_CHANGED 4
	#define SDL_KEYDOWN 5
	#define SDL_KEYUP 6
	#define SDL_TEXTINPUT 7
	#define SDL_MOUSEBUTTONDOWN 8
	#define SDL_MOUSEBUTTONUP 9
	#define SDL_MOUSEWHEEL 10
	#define SDL_MOUSEMOTION 11
	
	static inline int SDL_PollEvent(SDL_Event*) { return 0; }
	static inline unsigned int SDL_GetTicks() { return 0; }
	static inline void SDL_WarpMouseInWindow(SDL_Window*, int, int) {}
	#define AUDIO_S16LSB 0
	static inline int SDL_OpenAudioDevice(const char*, int, const SDL_AudioSpec*, SDL_AudioSpec*, int) { return 0; }
	static inline int SDL_GetNumAudioDevices(int) { return 0; }
	static inline const char* SDL_GetAudioDeviceName(int, int) { return nullptr; }
	static inline const char* SDL_GetError() { return ""; }
	static inline void SDL_CloseAudioDevice(int) {}
	static inline void SDL_PauseAudioDevice(int, int) {}
	typedef int SDL_AudioStatus;
	#define SDL_AUDIO_PLAYING 0
	static inline SDL_AudioStatus SDL_GetAudioStatus() { return SDL_AUDIO_PLAYING; }
	static inline void SDL_LockAudioDevice(int) {}
	static inline void SDL_UnlockAudioDevice(int) {}
	
	struct SDL_AudioCVT {
		int needed;
		int src_format;
		int dst_format;
		double rate_incr;
		unsigned char* buf;
		int len;
		int len_cvt;
		int len_mult;
		double len_ratio;
		void* filters[10];
		int filter_index;
	};
	
	static inline int SDL_BuildAudioCVT(SDL_AudioCVT*, int, int, int, int, int, int) { return 0; }
	static inline int SDL_ConvertAudio(SDL_AudioCVT*) { return 0; }
	static inline void* SDL_LoadWAV(const char*, SDL_AudioSpec*, unsigned char**, unsigned int*) { return nullptr; }
	static inline void SDL_FreeWAV(unsigned char*) {}

	// OpenGL types
	typedef int GLint;
	typedef unsigned int GLenum;
	typedef unsigned int GLuint;
	typedef unsigned char GLboolean;
	typedef unsigned int GLbitfield;
	
	#define GL_RGB8 0
	#define GL_RGBA8 0
	#define GL_DEPTH_COMPONENT 0
	#define GL_TEXTURE_2D 0
	#define GL_TEXTURE0 0
	
	static inline void glEnable(unsigned int) {}
	static inline void glDisable(unsigned int) {}
	static inline void glViewport(int, int, int, int) {}
	
	// More SDL Video types
	#define SDL_WINDOW_OPENGL 0
	#define SDL_WINDOW_FULLSCREEN 0
	#define SDL_WINDOW_BORDERLESS 0
	#define SDL_WINDOW_RESIZABLE 0
	#define SDL_WINDOWPOS_CENTERED_DISPLAY(x) 0
	
	typedef struct SDL_Surface {
		int w;
		int h;
		int pitch;
		void* pixels;
	} SDL_Surface;
	
	static inline void SDL_DestroyWindow(SDL_Window*) {}
	static inline SDL_Window* SDL_CreateWindow(const char*, int, int, int, int, unsigned int) { return nullptr; }
	static inline void* SDL_GL_CreateContext(SDL_Window*) { return nullptr; }
	static inline int SDL_GL_SetSwapInterval(int) { return 0; }
	static inline void SDL_GetWindowSize(SDL_Window*, int*, int*) {}
	static inline int SDL_ShowCursor(int) { return 0; }
	
	#define SDL_GL_RED_SIZE 0
	#define SDL_GL_GREEN_SIZE 0
	#define SDL_GL_BLUE_SIZE 0
	#define SDL_GL_DEPTH_SIZE 0
	#define SDL_GL_DOUBLEBUFFER 0
	#define SDL_GL_MULTISAMPLESAMPLES 0
	
	static inline int SDL_GL_SetAttribute(int, int) { return 0; }
	static inline SDL_Surface* SDL_CreateRGBSurfaceFrom(void*, int, int, int, int, unsigned int, unsigned int, unsigned int, unsigned int) { return nullptr; }
	static inline void SDL_SetWindowIcon(SDL_Window*, SDL_Surface*) {}
	static inline void SDL_FreeSurface(SDL_Surface*) {}
	static inline void SDL_GL_SwapWindow(SDL_Window*) {}

	// More OpenGL stubs
	#define GL_BLEND 0
	#define GL_SRC_ALPHA 0
	#define GL_ONE_MINUS_SRC_ALPHA 0
	#define GL_COLOR_BUFFER_BIT 0
	#define GL_DEPTH_BUFFER_BIT 0
	#define GL_RGBA 0
	#define GL_UNSIGNED_BYTE 0
	
	static inline void glBlendFunc(unsigned int, unsigned int) {}
	static inline void glClear(unsigned int) {}
	static inline void glReadPixels(int, int, int, int, unsigned int, unsigned int, void*) {}
	
	// Framebuffer / Renderbuffer stubs
	#define GL_RENDERBUFFER 0
	#define GL_FRAMEBUFFER 0
	#define GL_FRAMEBUFFER_COMPLETE 0
	
	// OpenGL Error constants (with unique values to prevent case label collisions)
	#define GL_NONE 100
	#define GL_INVALID_OPERATION 101
	#define GL_INVALID_ENUM 102
	#define GL_INVALID_VALUE 103
	#define GL_OUT_OF_MEMORY 104
	#define GL_INVALID_FRAMEBUFFER_OPERATION 105
	
	static inline unsigned char glIsRenderbuffer(unsigned int) { return 0; }
	static inline void glGenRenderbuffers(int, unsigned int*) {}
	static inline void glBindRenderbuffer(unsigned int, unsigned int) {}
	static inline void glRenderbufferStorage(unsigned int, unsigned int, int, int) {}
	static inline void glDeleteRenderbuffers(int, const unsigned int*) {}
	static inline void glGenFramebuffers(int, unsigned int*) {}
	static inline unsigned int glCheckFramebufferStatus(unsigned int) { return 0; }
	static inline unsigned int glGetError() { return 0; }
	static inline void glDeleteFramebuffers(int, const unsigned int*) {}
	static inline void glFramebufferTexture2D(unsigned int, unsigned int, unsigned int, unsigned int, int) {}
	static inline void glFramebufferRenderbuffer(unsigned int, unsigned int, unsigned int, unsigned int) {}
	static inline unsigned char glIsFramebuffer(unsigned int) { return 0; }
	static inline void glBindFramebuffer(unsigned int, unsigned int) {}
	
	// Scissor / Blend stubs
	#define GL_SCISSOR_TEST 0
	static inline void glScissor(int, int, int, int) {}

	// Shader OpenGL stubs
	typedef char GLchar;
	typedef int GLsizei;

	#define GL_VERTEX_SHADER 0
	#define GL_FRAGMENT_SHADER 1
	#define GL_ONE 1
	#define GL_ZERO 0
	#define GL_COMPILE_STATUS 0
	#define GL_INFO_LOG_LENGTH 0
	#define GL_LINK_STATUS 0

	static inline void glUseProgram(unsigned int) {}
	static inline void glActiveTexture(unsigned int) {}
	static inline void glDeleteProgram(unsigned int) {}
	static inline void glDeleteShader(unsigned int) {}
	static inline unsigned int glGetUniformLocation(unsigned int, const char*) { return 0; }
	static inline unsigned int glGetAttribLocation(unsigned int, const char*) { return 0; }
	static inline void glUniform1i(unsigned int, int) {}
	static inline void glUniform2iv(unsigned int, int, const int*) {}
	static inline void glUniform3iv(unsigned int, int, const int*) {}
	static inline void glUniform4iv(unsigned int, int, const int*) {}
	static inline void glUniform1f(unsigned int, float) {}
	static inline void glUniform2fv(unsigned int, int, const float*) {}
	static inline void glUniform3fv(unsigned int, int, const float*) {}
	static inline void glUniform4fv(unsigned int, int, const float*) {}
	static inline void glUniformMatrix3fv(unsigned int, int, unsigned char, const float*) {}
	static inline void glUniformMatrix4fv(unsigned int, int, unsigned char, const float*) {}
	static inline void glBindTexture(unsigned int, unsigned int) {}
	static inline unsigned int glCreateShader(unsigned int) { return 0; }
	static inline void glShaderSource(unsigned int, int, const char**, const int*) {}
	static inline void glCompileShader(unsigned int) {}
	static inline void glGetShaderiv(unsigned int, unsigned int, int*) {}
	static inline void glGetShaderInfoLog(unsigned int, int, int*, char*) {}
	static inline unsigned int glCreateProgram() { return 0; }
	static inline void glAttachShader(unsigned int, unsigned int) {}
	static inline void glBindAttribLocation(unsigned int, unsigned int, const char*) {}
	static inline void glLinkProgram(unsigned int) {}
	static inline void glGetProgramiv(unsigned int, unsigned int, int*) {}
	static inline void glGetProgramInfoLog(unsigned int, int, int*, char*) {}

	// Texture OpenGL stubs and constants
	#define GL_TEXTURE_CUBE_MAP 0
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0
	#define GL_TEXTURE_MIN_FILTER 0
	#define GL_TEXTURE_MAG_FILTER 0
	#define GL_LINEAR_MIPMAP_LINEAR 0
	#define GL_NEAREST_MIPMAP_NEAREST 0
	#define GL_NEAREST 0
	#define GL_LINEAR 0
	#define GL_TEXTURE_WRAP_S 0
	#define GL_TEXTURE_WRAP_T 0
	#define GL_CLAMP_TO_EDGE 0
	#define GL_REPEAT 0
	#define GL_MIRRORED_REPEAT 0

	static inline void glDeleteTextures(int, const unsigned int*) {}
	static inline void glGenTextures(int, unsigned int*) {}
	static inline void glTexImage2D(unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*) {}
	static inline void glTexSubImage2D(unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*) {}
	static inline void glCopyTexImage2D(unsigned int, int, unsigned int, int, int, int, int, int) {}
	static inline void glTexParameteri(unsigned int, unsigned int, int) {}
	static inline void glGenerateMipmap(unsigned int) {}

	// VertexArrayObject OpenGL stubs and constants
	typedef long GLsizeiptr;
	typedef float GLfloat;

	#define GL_ARRAY_BUFFER 0
	#define GL_STATIC_DRAW 0
	#define GL_FLOAT 0
	#define GL_FALSE 0
	#define GL_TRUE 1

	static inline void glDeleteVertexArrays(int, const unsigned int*) {}
	static inline void glDeleteBuffers(int, const unsigned int*) {}
	static inline void glGenVertexArrays(int, unsigned int*) {}
	static inline void glGenBuffers(int, unsigned int*) {}
	static inline void glBindVertexArray(unsigned int) {}
	static inline void glBindBuffer(unsigned int, unsigned int) {}
	static inline void glBufferData(unsigned int, long, const void*, unsigned int) {}
	static inline void glDrawArrays(unsigned int, int, int) {}
	static inline void glVertexAttribPointer(unsigned int, int, unsigned int, unsigned char, int, const void*) {}
	static inline void glEnableVertexAttribArray(unsigned int) {}
	static inline void glDisableVertexAttribArray(unsigned int) {}
#else
	#error Unsupported platform
#endif


#if defined(RMX_USE_GLES2) && !defined(__EMSCRIPTEN__)
	#if !defined(PLATFORM_LINUX) && !defined(__vita__)
		#define GL_RGB8				 GL_RGB
		#define GL_RGBA8			 GL_RGBA
		#define glGenVertexArrays	 glGenVertexArraysOES
		#define glDeleteVertexArrays glDeleteVertexArraysOES
		#define glBindVertexArray	 glBindVertexArrayOES
	#endif
	#define glClearDepth glClearDepthf
	#define glDepthRange glDepthRangef
#endif


#ifdef RMX_USE_GLEW
	#ifndef GLEW_STATIC
		#define GLEW_STATIC
	#endif
	#define GLEW_NO_GLU
	#include "rmxmedia/_glew/GL/glew.h"
#endif
