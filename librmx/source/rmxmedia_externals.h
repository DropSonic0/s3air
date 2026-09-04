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
	#ifndef SDL_VERSION_ATLEAST
		#define SDL_VERSION_ATLEAST(X, Y, Z) 0
	#endif
	typedef struct SDL_mutex SDL_mutex;
	typedef struct SDL_Thread SDL_Thread;
	typedef struct SDL_cond SDL_cond;
	typedef struct SDL_Window { int dummy; } SDL_Window;
	typedef struct _SDL_Joystick SDL_Joystick;
	typedef struct _SDL_GameController SDL_GameController;
	typedef int64_t SDL_TouchID;
	typedef int64_t SDL_FingerID;

	typedef struct SDL_Finger {
		SDL_FingerID id;
		float x;
		float y;
		float pressure;
	} SDL_Finger;

	typedef enum {
		SDL_CONTROLLER_BINDTYPE_NONE = 0,
		SDL_CONTROLLER_BINDTYPE_BUTTON = 1,
		SDL_CONTROLLER_BINDTYPE_AXIS = 2,
		SDL_CONTROLLER_BINDTYPE_HAT = 3
	} SDL_GameControllerBindType;

	typedef struct SDL_GameControllerButtonBind {
		SDL_GameControllerBindType bindType;
		union {
			int button;
			int axis;
			struct {
				int hat;
				int hat_mask;
			} hat;
		} value;
	} SDL_GameControllerButtonBind;

	typedef int SDL_AudioDeviceID;
	typedef void* SDL_GLContext;
	typedef int32_t SDL_Keycode;
	
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
		unsigned char state;
		SDL_Keysym keysym;
	};

	#define SDL_PRESSED 1
	#define SDL_RELEASED 0
	
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
		unsigned int windowID;
	} SDL_WindowEvent;

	typedef struct SDL_Rect {
		int x;
		int y;
		int w;
		int h;
	} SDL_Rect;

	typedef struct SDL_DisplayMode {
		int w;
		int h;
	} SDL_DisplayMode;
	
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
	#define SDL_WINDOWEVENT_FOCUS_LOST 12
	#define SDL_APP_WILLENTERBACKGROUND 13
	#define SDL_JOYDEVICEADDED 14
	#define SDL_JOYDEVICEREMOVED 15

	#define SDLK_LALT 1001
	#define SDLK_RALT 1002
	#define SDLK_RETURN 1003
	#define SDLK_LSHIFT 1004
	#define SDLK_RSHIFT 1005
	#define SDLK_END 1006
	#define SDLK_F1 1007
	#define SDLK_F2 1008
	#define SDLK_F3 1009
	#define SDLK_F4 1010
	#define SDLK_F5 1011
	#define SDLK_F8 1012
	#define SDLK_PRINTSCREEN 1013
	#define SDLK_KP_PLUS 1014
	#define SDLK_KP_MINUS 1015
	#define SDLK_KP_DIVIDE 1016
	#define SDLK_KP_MULTIPLY 1017
	#define SDLK_KP_0 1018
	#define SDLK_KP_1 1019
	#define SDLK_KP_2 1020
	#define SDLK_KP_3 1021
	#define SDLK_KP_4 1022
	#define SDLK_KP_5 1023
	#define SDLK_KP_6 1024
	#define SDLK_KP_7 1025
	#define SDLK_KP_8 1035
	#define SDLK_KP_9 1026
	#define SDLK_KP_PERIOD 1027
	#define SDLK_KP_ENTER 1036
	#define SDLK_LCTRL 1028
	#define SDLK_RCTRL 1037
	#define SDLK_F7 1029
	#define SDLK_F10 1030
	#define SDLK_F11 1031
	#define SDLK_F12 1050
	#define SDLK_LGUI 1051
	#define SDLK_RGUI 1052
	#define SDLK_SCROLLLOCK 1053
	#define SDLK_PAUSE 1054
	#define SDLK_NUMLOCKCLEAR 1055
	#define SDLK_CLEAR 1032
	#define SDLK_BACKQUOTE 1033
	#define SDLK_TAB 1034
	#define SDLK_ESCAPE 1038
	#define SDLK_BACKSPACE 1039
	#define SDLK_SPACE ' '
	#define SDLK_EXCLAIM '!'
	#define SDLK_QUOTEDBL '"'
	#define SDLK_HASH '#'
	#define SDLK_PERCENT '%'
	#define SDLK_DOLLAR '$'
	#define SDLK_AMPERSAND '&'
	#define SDLK_QUOTE '\''
	#define SDLK_LEFTPAREN '('
	#define SDLK_RIGHTPAREN ')'
	#define SDLK_ASTERISK '*'
	#define SDLK_PLUS '+'
	#define SDLK_COMMA ','
	#define SDLK_MINUS '-'
	#define SDLK_PERIOD '.'
	#define SDLK_SLASH '/'
	#define SDLK_0 '0'
	#define SDLK_1 '1'
	#define SDLK_2 '2'
	#define SDLK_3 '3'
	#define SDLK_4 '4'
	#define SDLK_5 '5'
	#define SDLK_6 '6'
	#define SDLK_7 '7'
	#define SDLK_8 '8'
	#define SDLK_9 '9'
	#define SDLK_COLON ':'
	#define SDLK_SEMICOLON ';'
	#define SDLK_LESS '<'
	#define SDLK_EQUALS '='
	#define SDLK_GREATER '>'
	#define SDLK_QUESTION '?'
	#define SDLK_AT '@'
	#define SDLK_LEFTBRACKET '['
	#define SDLK_BACKSLASH '\\'
	#define SDLK_RIGHTBRACKET ']'
	#define SDLK_CARET '^'
	#define SDLK_UNDERSCORE '_'
	#define SDLK_a 'a'
	#define SDLK_b 'b'
	#define SDLK_c 'c'
	#define SDLK_d 'd'
	#define SDLK_e 'e'
	#define SDLK_f 'f'
	#define SDLK_g 'g'
	#define SDLK_h 'h'
	#define SDLK_i 'i'
	#define SDLK_j 'j'
	#define SDLK_k 'k'
	#define SDLK_l 'l'
	#define SDLK_m 'm'
	#define SDLK_n 'n'
	#define SDLK_o 'o'
	#define SDLK_p 'p'
	#define SDLK_q 'q'
	#define SDLK_r 'r'
	#define SDLK_s 's'
	#define SDLK_t 't'
	#define SDLK_u 'u'
	#define SDLK_v 'v'
	#define SDLK_w 'w'
	#define SDLK_x 'x'
	#define SDLK_y 'y'
	#define SDLK_z 'z'
	#define SDLK_CAPSLOCK 1040
	#define SDLK_INSERT 1041
	#define SDLK_HOME 1042
	#define SDLK_PAGEUP 1043
	#define SDLK_DELETE 1044
	#define SDLK_PAGEDOWN 1045
	#define SDLK_UP 1046
	#define SDLK_DOWN 1047
	#define SDLK_LEFT 1048
	#define SDLK_RIGHT 1049

	#define KMOD_LSHIFT 0x0001
	#define KMOD_RSHIFT 0x0002
	#define KMOD_LCTRL  0x0040
	#define KMOD_RCTRL  0x0080
	#define KMOD_CTRL   (KMOD_LCTRL | KMOD_RCTRL)
	#define KMOD_SHIFT  (KMOD_LSHIFT | KMOD_RSHIFT)
	static inline int SDL_GetModState() { return 0; }

	#define SDL_INIT_JOYSTICK 0

	#define SDL_CONTROLLER_AXIS_LEFTX 0
	#define SDL_CONTROLLER_AXIS_LEFTY 1

	#define SDL_CONTROLLER_BUTTON_A 0
	#define SDL_CONTROLLER_BUTTON_B 1
	#define SDL_CONTROLLER_BUTTON_X 2
	#define SDL_CONTROLLER_BUTTON_Y 3
	#define SDL_CONTROLLER_BUTTON_BACK 4
	#define SDL_CONTROLLER_BUTTON_GUIDE 5
	#define SDL_CONTROLLER_BUTTON_START 6
	#define SDL_CONTROLLER_BUTTON_LEFTSHOULDER 7
	#define SDL_CONTROLLER_BUTTON_RIGHTSHOULDER 8
	#define SDL_CONTROLLER_BUTTON_DPAD_UP 9
	#define SDL_CONTROLLER_BUTTON_DPAD_DOWN 10
	#define SDL_CONTROLLER_BUTTON_DPAD_LEFT 11
	#define SDL_CONTROLLER_BUTTON_DPAD_RIGHT 12

	static inline const char* SDL_JoystickName(SDL_Joystick*) { return nullptr; }
	static inline const char* SDL_GameControllerName(SDL_GameController*) { return nullptr; }
	static inline SDL_Joystick* SDL_JoystickOpen(int) { return nullptr; }
	static inline SDL_GameController* SDL_GameControllerOpen(int) { return nullptr; }
	static inline int SDL_NumJoysticks() { return 0; }
	static inline int32_t SDL_JoystickInstanceID(SDL_Joystick*) { return -1; }
	static inline int SDL_JoystickNumButtons(SDL_Joystick*) { return 0; }
	static inline unsigned char SDL_JoystickGetButton(SDL_Joystick*, int) { return 0; }
	static inline int SDL_JoystickNumAxes(SDL_Joystick*) { return 0; }
	static inline int16_t SDL_JoystickGetAxis(SDL_Joystick*, int) { return 0; }
	static inline int SDL_JoystickNumHats(SDL_Joystick*) { return 0; }
	static inline unsigned char SDL_JoystickGetHat(SDL_Joystick*, int) { return 0; }
	static inline SDL_GameControllerButtonBind SDL_GameControllerGetBindForAxis(SDL_GameController*, int) { SDL_GameControllerButtonBind b; b.bindType = SDL_CONTROLLER_BINDTYPE_NONE; return b; }
	static inline SDL_GameControllerButtonBind SDL_GameControllerGetBindForButton(SDL_GameController*, int) { SDL_GameControllerButtonBind b; b.bindType = SDL_CONTROLLER_BINDTYPE_NONE; return b; }
	static inline int SDL_GetNumTouchDevices() { return 0; }
	static inline SDL_TouchID SDL_GetTouchDevice(int) { return 0; }
	static inline int SDL_GetNumTouchFingers(SDL_TouchID) { return 0; }
	static inline const SDL_Finger* SDL_GetTouchFinger(SDL_TouchID, int) { return nullptr; }

	typedef int SDL_bool;
	#define SDL_TRUE 1
	#define SDL_FALSE 0
	#define SDL_WINDOW_FULLSCREEN_DESKTOP 0
	#define SDL_WINDOWPOS_CENTERED 0

	#define SDL_HINT_VIDEO_ALLOW_SCREENSAVER "1"
	#define SDL_HINT_ACCELEROMETER_AS_JOYSTICK "2"
	#define SDL_HINT_RENDER_VSYNC "3"

	#define SDL_GL_CONTEXT_PROFILE_MASK 1
	#define SDL_GL_CONTEXT_PROFILE_CORE 1
	#define SDL_GL_CONTEXT_MAJOR_VERSION 2
	#define SDL_GL_CONTEXT_MINOR_VERSION 3

	static inline int SDL_SetHint(const char*, const char*) { return 0; }
	static inline void SDL_DisableScreenSaver() {}
	static inline int SDL_IsTextInputActive() { return 0; }
	static inline void SDL_StartTextInput() {}
	static inline void SDL_StopTextInput() {}
	static inline unsigned int SDL_GetWindowID(SDL_Window*) { return 0; }
	static inline int SDL_GetWindowDisplayIndex(SDL_Window*) { return 0; }
	static inline int SDL_SetWindowFullscreen(SDL_Window*, unsigned int) { return 0; }
	static inline void SDL_SetWindowSize(SDL_Window*, int, int) {}
	static inline void SDL_SetWindowPosition(SDL_Window*, int, int) {}
	static inline void SDL_SetWindowResizable(SDL_Window*, unsigned char) {}
	static inline void SDL_SetWindowBordered(SDL_Window*, unsigned char) {}
	static inline int SDL_GetDisplayBounds(int, SDL_Rect* rect)
	{
		if (rect) { rect->x = 0; rect->y = 0; rect->w = 1280; rect->h = 720; }
		return 0;
	}
	static inline int SDL_GetDesktopDisplayMode(int, SDL_DisplayMode* mode)
	{
		if (mode) { mode->w = 1280; mode->h = 720; }
		return 0;
	}

	static inline int SDL_PollEvent(SDL_Event*) { return 0; }
	static inline unsigned int SDL_GetTicks() { return 0; }
	static inline void SDL_WarpMouseInWindow(SDL_Window*, int, int) {}
	#define AUDIO_S16LSB 0
	static inline int SDL_OpenAudioDevice(const char*, int, const SDL_AudioSpec*, SDL_AudioSpec*, int) { return 0; }
	static inline int SDL_GetNumAudioDevices(int) { return 0; }
	static inline const char* SDL_GetAudioDeviceName(int, int) { return nullptr; }
	static inline const char* SDL_GetError() { return ""; }

	#define SDL_arraysize(array) (sizeof(array) / sizeof((array)[0]))

	#define SDL_MESSAGEBOX_ERROR 0x00000010
	#define SDL_MESSAGEBOX_WARNING 0x00000020
	#define SDL_MESSAGEBOX_INFORMATION 0x00000040

	#define SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT 0x00000001
	#define SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT 0x00000002

	typedef struct SDL_MessageBoxButtonData {
		uint32_t flags;
		int buttonid;
		const char* text;
	} SDL_MessageBoxButtonData;

	typedef struct SDL_MessageBoxData {
		uint32_t flags;
		SDL_Window* window;
		const char* title;
		const char* message;
		int numbuttons;
		const SDL_MessageBoxButtonData* buttons;
		void* colorScheme;
	} SDL_MessageBoxData;

	static inline int SDL_ShowSimpleMessageBox(uint32_t, const char*, const char*, SDL_Window*) { return 0; }
	static inline int SDL_ShowMessageBox(const SDL_MessageBoxData*, int*) { return 0; }

	static inline int SDL_SetClipboardText(const char*) { return -1; }
	static inline char* SDL_GetClipboardText() { return nullptr; }
	static inline SDL_bool SDL_HasClipboardText() { return SDL_FALSE; }
	static inline void SDL_free(void*) {}
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
	
	#define GL_TRIANGLES 0x0004
	#define GL_RGB8 0
	#define GL_RGBA8 0
	#define GL_DEPTH_COMPONENT 0
	#define GL_TEXTURE_2D 0
	#define GL_TEXTURE0 0x84C0
	#define GL_TEXTURE1 0x84C1
	#define GL_TEXTURE2 0x84C2
	#define GL_TEXTURE3 0x84C3
	#define GL_TEXTURE4 0x84C4
	#define GL_TEXTURE5 0x84C5
	#define GL_TEXTURE6 0x84C6
	#define GL_TEXTURE7 0x84C7
	#define GL_TEXTURE_BUFFER 0x8C2A
	#define GL_LUMINANCE 0x1909
	#define GL_R8UI 0x8232
	#define GL_R16I 0x8233
	#define GL_R16UI 0x8234
	
	static inline void glTexBuffer(unsigned int, unsigned int, unsigned int) {}
	static inline void glEnable(unsigned int) {}
	static inline void glDisable(unsigned int) {}
	static inline void glViewport(int, int, int, int) {}
	
	// More SDL Video types
	#define SDL_WINDOW_OPENGL 0
	#define SDL_WINDOW_FULLSCREEN 0
	#define SDL_WINDOW_BORDERLESS 0
	#define SDL_WINDOW_RESIZABLE 0
	#define SDL_WINDOWPOS_CENTERED_DISPLAY(x) 0
	
	typedef struct SDL_PixelFormat {
		uint32_t format;
	} SDL_PixelFormat;

	typedef struct SDL_Surface {
		SDL_PixelFormat* format;
		int w;
		int h;
		int pitch;
		void* pixels;
	} SDL_Surface;

	#define SDL_PIXELFORMAT_UNKNOWN 0
	#define SDL_PIXELFORMAT_INDEX1LSB 1
	#define SDL_PIXELFORMAT_INDEX1MSB 2
	#define SDL_PIXELFORMAT_INDEX4LSB 3
	#define SDL_PIXELFORMAT_INDEX4MSB 4
	#define SDL_PIXELFORMAT_INDEX8 5
	#define SDL_PIXELFORMAT_RGB332 6
	#define SDL_PIXELFORMAT_RGB444 7
	#define SDL_PIXELFORMAT_RGB555 8
	#define SDL_PIXELFORMAT_BGR555 9
	#define SDL_PIXELFORMAT_ARGB4444 10
	#define SDL_PIXELFORMAT_RGBA4444 11
	#define SDL_PIXELFORMAT_ABGR4444 12
	#define SDL_PIXELFORMAT_BGRA4444 13
	#define SDL_PIXELFORMAT_ARGB1555 14
	#define SDL_PIXELFORMAT_RGBA5551 15
	#define SDL_PIXELFORMAT_ABGR1555 16
	#define SDL_PIXELFORMAT_BGRA5551 17
	#define SDL_PIXELFORMAT_RGB565 18
	#define SDL_PIXELFORMAT_BGR565 19
	#define SDL_PIXELFORMAT_RGB24 20
	#define SDL_PIXELFORMAT_BGR24 21
	#define SDL_PIXELFORMAT_RGB888 22
	#define SDL_PIXELFORMAT_RGBX8888 23
	#define SDL_PIXELFORMAT_BGR888 24
	#define SDL_PIXELFORMAT_BGRX8888 25
	#define SDL_PIXELFORMAT_ARGB8888 26
	#define SDL_PIXELFORMAT_RGBA8888 27
	#define SDL_PIXELFORMAT_ABGR8888 28
	#define SDL_PIXELFORMAT_BGRA8888 29
	#define SDL_PIXELFORMAT_ARGB2101010 30
	#define SDL_PIXELFORMAT_YV12 31
	#define SDL_PIXELFORMAT_IYUV 32
	#define SDL_PIXELFORMAT_YUY2 33
	#define SDL_PIXELFORMAT_UYVY 34
	#define SDL_PIXELFORMAT_YVYU 35
	#define SDL_PIXELFORMAT_NV12 36
	#define SDL_PIXELFORMAT_NV21 37

	static inline SDL_Surface* SDL_GetWindowSurface(SDL_Window*) { return nullptr; }
	static inline int SDL_LockSurface(SDL_Surface*) { return 0; }
	static inline void SDL_UnlockSurface(SDL_Surface*) {}
	static inline int SDL_UpdateWindowSurface(SDL_Window*) { return 0; }
	
	static inline void SDL_DestroyWindow(SDL_Window*) {}
	static inline SDL_Window* SDL_CreateWindow(const char*, int, int, int, int, unsigned int)
	{
		return (SDL_Window*)1;
	}
	static inline void* SDL_GL_CreateContext(SDL_Window*)
	{
		static int dummy_context = 1;
		return &dummy_context;
	}
	static inline int SDL_GL_SetSwapInterval(int) { return 0; }
	static inline void SDL_GetWindowSize(SDL_Window*, int* w, int* h)
	{
		if (w) *w = 1280;
		if (h) *h = 720;
	}
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
	
	#define GL_FUNC_ADD 0x8006
	#define GL_FUNC_REVERSE_SUBTRACT 0x800B
	#define GL_DST_COLOR 0x0306
	#define GL_MIN 0x8007
	#define GL_MAX 0x8008

	static inline void glBlendEquation(unsigned int) {}
	static inline void glBlendFunc(unsigned int, unsigned int) {}
	#define GL_DEPTH_ATTACHMENT 0x8D00
	#define GL_DEPTH_TEST 0x0B71
	#define GL_FRAMEBUFFER_BINDING 0x8CA6
	#define GL_VIEWPORT 0x0BA2
	#define GL_ALWAYS 0x0207
	#define GL_GEQUAL 0x0206

	static inline void glClearColor(float, float, float, float) {}
	static inline void glClearDepth(double) {}
	static inline void glDepthRange(double, double) {}
	static inline void glDepthMask(unsigned char) {}
	static inline void glDepthFunc(unsigned int) {}
	static inline void glGetIntegerv(unsigned int, int*) {}
	static inline void glClear(unsigned int) {}
	static inline void glReadPixels(int, int, int, int, unsigned int, unsigned int, void*) {}
	
	// Framebuffer / Renderbuffer stubs
	#define GL_RENDERBUFFER 0
	#define GL_FRAMEBUFFER 0
	#define GL_FRAMEBUFFER_COMPLETE 0
	#define GL_COLOR_ATTACHMENT0 0x8CE0
	
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
	static inline unsigned int glCreateShader(unsigned int) { static unsigned int shader_id = 1; return shader_id++; }
	static inline void glShaderSource(unsigned int, int, const char**, const int*) {}
	static inline void glCompileShader(unsigned int) {}
	static inline void glGetShaderiv(unsigned int, unsigned int pname, int* params)
	{
		if (params)
		{
			if (pname == GL_COMPILE_STATUS)
				*params = 1;
			else
				*params = 0;
		}
	}
	static inline void glGetShaderInfoLog(unsigned int, int, int*, char*) {}
	static inline unsigned int glCreateProgram() { static unsigned int program_id = 1; return program_id++; }
	static inline void glAttachShader(unsigned int, unsigned int) {}
	static inline void glBindAttribLocation(unsigned int, unsigned int, const char*) {}
	static inline void glLinkProgram(unsigned int) {}
	static inline void glGetProgramiv(unsigned int, unsigned int pname, int* params)
	{
		if (params)
		{
			if (pname == GL_LINK_STATUS)
				*params = 1;
			else
				*params = 0;
		}
	}
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
	static inline void glGetTexImage(unsigned int, int, unsigned int, unsigned int, void*) {}
	static inline void glGenerateMipmap(unsigned int) {}

	// VertexArrayObject OpenGL stubs and constants
#if !defined(__CELLOS_LV2__) && !defined(__SNC__) && !defined(PLATFORM_PS3) && !defined(RMX_PLATFORM_PS3)
	typedef long GLsizeiptr;
#endif
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
	static inline void glBufferSubData(unsigned int, long, long, const void*) {}
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
