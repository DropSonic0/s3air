/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
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
#if defined(PLATFORM_PS3)
	// SDL Shims for PS3
	typedef uint32 Uint32;
	typedef int SDL_Keycode;
	typedef int SDL_mutex;
	typedef int SDL_cond;
	typedef int SDL_Thread;
	typedef int SDL_AudioDeviceID;
	typedef int SDL_AudioStatus;
	#define SDL_AUDIO_STOPPED 0
	#define SDL_AUDIO_PLAYING 1
	#define SDL_AUDIO_PAUSED 2
	struct SDL_AudioSpec {
		int freq;
		int channels;
		int samples;
		int format;
		void (*callback)(void*, unsigned char*, int);
		void* userdata;
	};
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
	};
	#define AUDIO_S16LSB 0
	struct SDL_WindowEvent { int event; int data1; int data2; };
	struct SDL_Keysym { int sym; int scancode; int mod; };
	struct SDL_KeyboardEvent { int type; SDL_Keysym keysym; int state; int repeat; };
	struct SDL_TextInputEvent { int type; char text[32]; };
	struct SDL_MouseButtonEvent { int type; int button; int state; int x; int y; };
	struct SDL_MouseWheelEvent { int type; int y; };
	struct SDL_MouseMotionEvent { int type; int state; int x; int y; };
	struct SDL_Event {
		int type;
		SDL_WindowEvent window;
		SDL_KeyboardEvent key;
		SDL_TextInputEvent text;
		SDL_MouseButtonEvent button;
		SDL_MouseWheelEvent wheel;
		SDL_MouseMotionEvent motion;
	};
	struct SDL_Window { int unused; };

	inline SDL_mutex* SDL_CreateMutex() { return (SDL_mutex*)1; }
	inline void SDL_DestroyMutex(SDL_mutex* m) {}
	inline void SDL_LockMutex(SDL_mutex* m) {}
	inline void SDL_UnlockMutex(SDL_mutex* m) {}
	inline SDL_cond* SDL_CreateCond() { return (SDL_cond*)1; }
	inline void SDL_DestroyCond(SDL_cond* c) {}
	inline void SDL_CondSignal(SDL_cond* c) {}
	inline void SDL_CondWait(SDL_cond* c, SDL_mutex* m) {}
	inline int  SDL_CondWaitTimeout(SDL_cond* c, SDL_mutex* m, unsigned int ms) { return 0; }
	inline SDL_Thread* SDL_CreateThread(int (*f)(void*), const char* n, void* d) { return (SDL_Thread*)1; }
	inline void SDL_WaitThread(SDL_Thread* t, int* s) {}
	inline void* SDL_RWFromFile(const char* f, const char* m) { return 0; }
	inline void SDL_RWclose(void* c) {}
	inline size_t SDL_RWsize(void* c) { return 0; }
	inline size_t SDL_RWtell(void* c) { return 0; }
	inline size_t SDL_RWseek(void* c, long long p, int w) { return 0; }
	inline size_t SDL_RWread(void* c, void* d, size_t s, size_t n) { return 0; }
	#define RW_SEEK_SET 0
	#define SDLK_SCANCODE_MASK (1<<30)
	inline void SDL_PauseAudioDevice(SDL_AudioDeviceID d, int p) {}
	inline SDL_AudioStatus SDL_GetAudioStatus() { return (SDL_AudioStatus)0; }
	inline void SDL_LockAudioDevice(SDL_AudioDeviceID d) {}
	inline void SDL_UnlockAudioDevice(SDL_AudioDeviceID d) {}
	inline int SDL_LoadWAV(const char* f, SDL_AudioSpec* s, unsigned char** d, unsigned int* l) { return 0; }
	inline void SDL_FreeWAV(unsigned char* d) {}
	inline int SDL_BuildAudioCVT(SDL_AudioCVT* c, int sf, int sc, int sr, int df, int dc, int dr) { return 0; }
	inline int SDL_ConvertAudio(SDL_AudioCVT* c) { return 0; }
	inline void SDL_DestroyWindow(SDL_Window* w) {}
	#define SDL_INIT_VIDEO 1
	#define SDL_INIT_AUDIO 2
	#define SDL_INIT_TIMER 4
	#define SDL_INIT_GAMECONTROLLER 8
	inline int SDL_Init(int f) { return 0; }
	inline void SDL_Quit() {}
	inline char* SDL_GetError() { return (char*)""; }
	inline void SDL_WarpMouseInWindow(SDL_Window* w, int x, int y) {}
	inline SDL_Window* SDL_CreateWindow(const char* t, int x, int y, int w, int h, Uint32 f) { return (SDL_Window*)1; }
	inline unsigned int SDL_GetTicks() { return 0; }
	inline int SDL_PollEvent(SDL_Event* e) { return 0; }
	#define SDL_QUIT 1
	#define SDL_WINDOWEVENT 2
	#define SDL_WINDOWEVENT_RESIZED 3
	#define SDL_KEYDOWN 4
	#define SDL_KEYUP 5
	#define SDL_TEXTINPUT 6
	#define SDL_MOUSEBUTTONDOWN 7
	#define SDL_MOUSEBUTTONUP 8
	#define SDL_MOUSEWHEEL 9
	#define SDL_MOUSEMOTION 10
	#define SDL_PRESSED 1
	#define SDL_BUTTON_LEFT 1
	#define SDL_BUTTON_RIGHT 2
	#define SDL_BUTTON_MIDDLE 3
	#define SDL_BUTTON_X1 4
	#define SDL_BUTTON_X2 5
	#define SDL_AUDIO_PLAYING 1
	inline void SDL_CloseAudioDevice(SDL_AudioDeviceID d) {}
	#define SDL_AUDIO_ALLOW_ANY_CHANGE 0
	inline SDL_AudioDeviceID SDL_OpenAudioDevice(const char* d, int is, SDL_AudioSpec* des, SDL_AudioSpec* obt, int f) { return 0; }
	inline void SDL_Delay(unsigned int ms) {}

	// OpenGL Shims for PS3
	typedef int GLint;
	typedef int GLenum;
	typedef unsigned int GLuint;
	typedef float GLfloat;
	typedef int GLsizei;
	typedef int GLsizeiptr;
	typedef unsigned int GLbitfield;
	typedef char GLchar;
	#define GL_TEXTURE_2D 0
	#define GL_RGBA 0
	#define GL_RGB 0
	#define GL_RGBA8 0
	#define GL_RGB8 0
	#define GL_DEPTH_COMPONENT 0
	#define GL_DEPTH_COMPONENT16 0
	#define GL_FALSE 0
	#define GL_TRUE 1
	#define GL_FLOAT 0
	#define GL_STATIC_DRAW 0
	#define GL_ARRAY_BUFFER 0
	#define GL_COLOR_BUFFER_BIT 0x00004000
	#define GL_DEPTH_BUFFER_BIT 0x00000100
	#define GL_UNSIGNED_BYTE 0
	#define GL_SCISSOR_TEST 0
	#define GL_RENDERBUFFER 0
	#define GL_FRAMEBUFFER 0
	#define GL_FRAMEBUFFER_COMPLETE 0
	#define GL_VERTEX_SHADER 0
	#define GL_FRAGMENT_SHADER 0
	#define GL_TEXTURE0 0
	#define GL_ONE 0
	#define GL_ZERO 0
	#define GL_SRC_ALPHA 0
	#define GL_ONE_MINUS_SRC_ALPHA 0
	#define GL_COMPILE_STATUS 0
	#define GL_LINK_STATUS 0
	#define GL_INFO_LOG_LENGTH 0
	#define GL_NONE 0
	#define GL_INVALID_OPERATION 1
	#define GL_INVALID_ENUM 2
	#define GL_INVALID_VALUE 3
	#define GL_OUT_OF_MEMORY 4
	#define GL_INVALID_FRAMEBUFFER_OPERATION 5
	inline void glGenTextures(GLsizei n, GLuint* t) {}
	inline void glDeleteTextures(GLsizei n, const GLuint* t) {}
	inline void glGenVertexArrays(GLsizei n, GLuint* a) {}
	inline void glDeleteVertexArrays(GLsizei n, const GLuint* a) {}
	inline void glGenBuffers(GLsizei n, GLuint* b) {}
	inline void glDeleteBuffers(GLsizei n, const GLuint* b) {}
	inline void glBindVertexArray(GLuint a) {}
	inline void glBindBuffer(GLenum t, GLuint b) {}
	inline void glVertexAttribPointer(GLuint i, GLint s, GLenum t, int n, GLsizei st, const void* p) {}
	inline void glEnableVertexAttribArray(GLuint i) {}
	inline void glDisableVertexAttribArray(GLuint i) {}
	inline void glBufferData(GLenum t, GLsizeiptr s, const void* d, GLenum u) {}
	inline void glDrawArrays(GLenum m, GLint f, GLsizei c) {}
	inline void glBindTexture(GLenum t, GLuint h) {}
	inline void glClearColor(float r, float g, float b, float a) {}
	inline void glClear(int m) {}
	inline void glEnable(GLenum cap) {}
	inline void glDisable(GLenum cap) {}
	inline void glViewport(int x, int y, int w, int h) {}
	inline void glReadPixels(int x, int y, int w, int h, int f, int t, void* d) {}
	inline void glScissor(int x, int y, int w, int h) {}
	inline unsigned char glIsRenderbuffer(GLuint b) { return 0; }
	inline void glGenRenderbuffers(GLsizei n, GLuint* b) {}
	inline void glBindRenderbuffer(GLenum t, GLuint b) {}
	inline void glRenderbufferStorage(GLenum t, GLenum i, GLsizei w, GLsizei h) {}
	inline void glDeleteRenderbuffers(GLsizei n, const GLuint* b) {}
	inline void glGenFramebuffers(GLsizei n, GLuint* b) {}
	inline GLenum glCheckFramebufferStatus(GLenum t) { return 0; }
	inline GLenum glGetError() { return 0; }
	inline void glDeleteFramebuffers(GLsizei n, const GLuint* b) {}
	inline void glFramebufferTexture2D(GLenum t, GLenum a, GLenum tt, GLuint te, GLint l) {}
	inline void glFramebufferRenderbuffer(GLenum t, GLenum a, GLenum rt, GLuint r) {}
	inline unsigned char glIsFramebuffer(GLuint b) { return 0; }
	inline void glBindFramebuffer(GLenum t, GLuint b) {}
	inline void glDeleteProgram(GLuint p) {}
	inline void glDeleteShader(GLuint s) {}
	inline GLint glGetUniformLocation(GLuint p, const char* n) { return 0; }
	inline GLint glGetAttribLocation(GLuint p, const char* n) { return 0; }
	inline void glUniform1i(GLint l, GLint v) {}
	inline void glUniform2iv(GLint l, GLsizei c, const GLint* v) {}
	inline void glUniform3iv(GLint l, GLsizei c, const GLint* v) {}
	inline void glUniform4iv(GLint l, GLsizei c, const GLint* v) {}
	inline void glUniform1f(GLint l, GLfloat v) {}
	inline void glUniform2fv(GLint l, GLsizei c, const GLfloat* v) {}
	inline void glUniform3fv(GLint l, GLsizei c, const GLfloat* v) {}
	inline void glUniform4fv(GLint l, GLsizei c, const GLfloat* v) {}
	inline void glUniformMatrix3fv(GLint l, GLsizei c, unsigned char t, const GLfloat* v) {}
	inline void glUniformMatrix4fv(GLint l, GLsizei c, unsigned char t, const GLfloat* v) {}
	inline void glActiveTexture(GLenum t) {}
	inline void glBlendFunc(GLenum s, GLenum d) {}
	inline void glUseProgram(GLuint p) {}
	inline GLuint glCreateShader(GLenum t) { return 0; }
	inline void glShaderSource(GLuint s, GLsizei c, const GLchar** st, const GLint* l) {}
	inline void glCompileShader(GLuint s) {}
	inline void glGetShaderiv(GLuint s, GLenum p, GLint* v) {}
	inline void glGetShaderInfoLog(GLuint s, GLsizei b, GLsizei* l, GLchar* i) {}
	inline GLuint glCreateProgram() { return 0; }
	inline void glAttachShader(GLuint p, GLuint s) {}
	inline void glBindAttribLocation(GLuint p, GLuint i, const GLchar* n) {}
	inline void glLinkProgram(GLuint p) {}
	inline void glGetProgramiv(GLuint p, GLenum n, GLint* v) {}
	inline void glGetProgramInfoLog(GLuint p, GLsizei b, GLsizei* l, GLchar* i) {}
	inline void SDL_GL_SwapWindow(SDL_Window* w) {}
	#define SDL_WINDOW_OPENGL 0x01
	#define SDL_WINDOW_SHOWN 0x02
	#define SDL_WINDOW_FULLSCREEN 0x04
	#define SDL_WINDOW_RESIZABLE 0x08
	#define SDL_WINDOW_BORDERLESS 0x10
	#define SDL_WINDOWPOS_CENTERED 0
	typedef void* SDL_GLContext;
	inline SDL_GLContext SDL_GL_CreateContext(SDL_Window* w) { return 0; }
	inline void SDL_GL_SetSwapInterval(int i) {}

#elif defined(PLATFORM_WINDOWS)
	// Needed for MSYS2
	#if defined(__GNUC__)
		#include <SDL2/SDL.h>
	#else
		#include <SDL/SDL.h>
	#endif
#else
	#include <SDL.h>
#endif


// OpenGL
#if defined(PLATFORM_PS3)
	// PS3 uses its own graphics API (libgcm or similar)
#elif defined(PLATFORM_WINDOWS)
	#define ALLOW_LEGACY_OPENGL
	#define RMX_USE_GLEW

#elif defined(PLATFORM_LINUX)
	#if defined(RMX_LINUX_ENFORCE_GLES2)	// Build option: Use OpenGL ES 2
		#define ALLOW_LEGACY_OPENGL
		#define RMX_USE_GLES2
		#include <GLES3/gl3.h>		// We need the ES 3 headers for e.g. glBindVertexArray
		#include <GLES3/gl3ext.h>
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

#else
	#error Unsupported platform
#endif


#if defined(RMX_USE_GLES2) && !defined(__EMSCRIPTEN__)
	#if !defined(PLATFORM_LINUX)
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
