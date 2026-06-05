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
	struct SDL_WindowEvent { int windowID; int event; int data1; int data2; };
	struct SDL_Keysym { int sym; int scancode; int mod; };
	struct SDL_KeyboardEvent { int type; SDL_Keysym keysym; int state; int repeat; };
	struct SDL_TextInputEvent { int type; char text[32]; };
	struct SDL_MessageBoxButtonData { uint32 flags; int buttonid; const char* text; };
	struct SDL_MessageBoxData { uint32 flags; struct SDL_Window* window; const char* title; const char* message; int numbuttons; const SDL_MessageBoxButtonData* buttons; const void* colorScheme; };
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
	#define SDLK_KP_DIVIDE 0x40000054
	#define SDLK_KP_MULTIPLY 0x40000055
	#define SDLK_KP_PLUS 0x40000057
	#define SDLK_KP_MINUS 0x40000056
	#define KMOD_LCTRL 0x0040
	#define KMOD_RCTRL 0x0080
	#define KMOD_CTRL (KMOD_LCTRL | KMOD_RCTRL)
	#define KMOD_LSHIFT 0x0001
	#define KMOD_RSHIFT 0x0002
	#define KMOD_SHIFT (KMOD_LSHIFT | KMOD_RSHIFT)
	#define SDLK_LALT 0x400000e2
	#define SDLK_RALT 0x400000e6
	#define SDLK_RETURN 0x0d
	#define SDLK_LSHIFT 0x400000e1
	#define SDLK_RSHIFT 0x400000e5
	#define SDLK_END 0x4000004d
	#define SDLK_HOME 0x4000004a
	#define SDLK_PAGEUP 0x4000004b
	#define SDLK_PAGEDOWN 0x4000004e
	#define SDLK_F1 0x4000003a
	#define SDLK_F2 0x4000003b
	#define SDLK_F3 0x4000003c
	#define SDLK_F4 0x4000003d
	#define SDLK_F5 0x4000003e
	#define SDLK_F7 0x40000040
	#define SDLK_F8 0x40000041
	#define SDLK_F10 0x40000043
	#define SDLK_F11 0x40000044
	#define SDLK_PRINTSCREEN 0x40000046
	#define SDLK_TAB '\t'
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
	#define SDLK_BACKQUOTE '`'
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
	#define SDLK_CAPSLOCK 0x40000039
	#define SDLK_INSERT 0x40000049
	#define SDLK_DELETE 0x7f
	#define SDLK_UP 0x40000052
	#define SDLK_DOWN 0x40000051
	#define SDLK_LEFT 0x40000050
	#define SDLK_RIGHT 0x4000004f
	#define SDLK_ESCAPE 0x1b
	#define SDLK_BACKSPACE 0x08
	#define SDLK_CLEAR 0x4000009c
	#define SDLK_KP_ENTER 0x40000058
	#define SDLK_KP_1 0x40000059
	#define SDLK_KP_2 0x4000005a
	#define SDLK_KP_3 0x4000005b
	#define SDLK_KP_4 0x4000005c
	#define SDLK_KP_5 0x4000005d
	#define SDLK_KP_6 0x4000005e
	#define SDLK_KP_7 0x4000005f
	#define SDLK_KP_8 0x40000060
	#define SDLK_KP_9 0x40000061
	#define SDLK_KP_0 0x40000062
	#define SDLK_KP_PERIOD 0x40000063
	#define SDLK_RSHIFT 0x400000e5
	#define SDLK_LCTRL 0x400000e0
	#define SDLK_RCTRL 0x400000e4

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
	#define SDL_INIT_JOYSTICK 16
	#define SDL_arraysize(X) (sizeof(X)/sizeof(X[0]))
	#define SDL_VERSION_ATLEAST(X, Y, Z) 0
	inline int SDL_Init(int f) { return 0; }
	inline int SDL_InitSubSystem(Uint32 f) { return 0; }
	inline void SDL_Quit() {}
	inline char* SDL_GetError() { return (char*)""; }
	inline int SDL_SetHint(const char* n, const char* v) { return 1; }
	inline void SDL_WarpMouseInWindow(SDL_Window* w, int x, int y) {}
	inline int SDL_GetModState() { return 0; }
	inline int SDL_ShowSimpleMessageBox(uint32 f, const char* t, const char* m, struct SDL_Window* w) { return 0; }
	inline int SDL_ShowMessageBox(const struct SDL_MessageBoxData* d, int* b) { if (b) *b = 0; return 0; }

	typedef struct _SDL_Joystick SDL_Joystick;
	typedef struct _SDL_GameController SDL_GameController;
	typedef enum {
		SDL_CONTROLLER_BINDTYPE_NONE = 0,
		SDL_CONTROLLER_BINDTYPE_AXIS,
		SDL_CONTROLLER_BINDTYPE_BUTTON,
		SDL_CONTROLLER_BINDTYPE_HAT
	} SDL_GameControllerBindType;
	struct SDL_GameControllerButtonBind {
		SDL_GameControllerBindType bindType;
		union {
			int button;
			int axis;
			struct {
				int hat;
				int hat_mask;
			} hat;
		} value;
	};
	#define SDL_CONTROLLER_AXIS_LEFTX 0
	#define SDL_CONTROLLER_AXIS_LEFTY 1
	#define SDL_CONTROLLER_BUTTON_A 0
	#define SDL_CONTROLLER_BUTTON_B 1
	#define SDL_CONTROLLER_BUTTON_X 2
	#define SDL_CONTROLLER_BUTTON_Y 3
	#define SDL_CONTROLLER_BUTTON_BACK 4
	#define SDL_CONTROLLER_BUTTON_GUIDE 5
	#define SDL_CONTROLLER_BUTTON_START 6
	#define SDL_CONTROLLER_BUTTON_LEFTSTICK 7
	#define SDL_CONTROLLER_BUTTON_RIGHTSTICK 8
	#define SDL_CONTROLLER_BUTTON_LEFTSHOULDER 9
	#define SDL_CONTROLLER_BUTTON_RIGHTSHOULDER 10
	#define SDL_CONTROLLER_BUTTON_DPAD_UP 11
	#define SDL_CONTROLLER_BUTTON_DPAD_DOWN 12
	#define SDL_CONTROLLER_BUTTON_DPAD_LEFT 13
	#define SDL_CONTROLLER_BUTTON_DPAD_RIGHT 14

	inline const char* SDL_JoystickName(SDL_Joystick* j) { return (const char*)0; }
	inline const char* SDL_GameControllerName(SDL_GameController* c) { return (const char*)0; }
	inline SDL_GameControllerButtonBind SDL_GameControllerGetBindForAxis(SDL_GameController* c, int a) { SDL_GameControllerButtonBind b; b.bindType = SDL_CONTROLLER_BINDTYPE_NONE; return b; }
	inline SDL_GameControllerButtonBind SDL_GameControllerGetBindForButton(SDL_GameController* c, int bt) { SDL_GameControllerButtonBind b; b.bindType = SDL_CONTROLLER_BINDTYPE_NONE; return b; }
	inline int SDL_JoystickNumButtons(SDL_Joystick* j) { return 0; }
	inline unsigned char SDL_JoystickGetButton(SDL_Joystick* j, int b) { return 0; }
	inline int SDL_JoystickNumAxes(SDL_Joystick* j) { return 0; }
	inline short SDL_JoystickGetAxis(SDL_Joystick* j, int a) { return 0; }
	inline int SDL_JoystickNumHats(SDL_Joystick* j) { return 0; }
	inline unsigned char SDL_JoystickGetHat(SDL_Joystick* j, int h) { return 0; }
	inline int SDL_NumJoysticks() { return 0; }
	inline SDL_Joystick* SDL_JoystickOpen(int i) { return (SDL_Joystick*)0; }
	inline int SDL_JoystickInstanceID(SDL_Joystick* j) { return -1; }
	inline SDL_GameController* SDL_GameControllerOpen(int i) { return (SDL_GameController*)0; }
	inline int SDL_JoystickRumble(SDL_Joystick* j, unsigned short l, unsigned short h, uint32 d) { return 0; }

	typedef int64 SDL_TouchID;
	typedef struct SDL_Finger {
		float x;
		float y;
		float pressure;
	} SDL_Finger;
	inline int SDL_GetNumTouchDevices() { return 0; }
	inline SDL_TouchID SDL_GetTouchDevice(int i) { return 0; }
	inline int SDL_GetNumTouchFingers(SDL_TouchID t) { return 0; }
	inline SDL_Finger* SDL_GetTouchFinger(SDL_TouchID t, int i) { return (SDL_Finger*)0; }

	inline SDL_Window* SDL_CreateWindow(const char* t, int x, int y, int w, int h, Uint32 f) { return (SDL_Window*)1; }
	inline int SDL_GetWindowID(SDL_Window* w) { return 1; }
	inline int SDL_GetWindowDisplayIndex(SDL_Window* w) { return 0; }
	inline void SDL_SetWindowFullscreen(SDL_Window* w, Uint32 f) {}
	inline void SDL_SetWindowSize(SDL_Window* w, int w1, int h1) {}
	inline void SDL_GetWindowSize(SDL_Window* w, int* w1, int* h1) { if (w1) *w1 = 1280; if (h1) *h1 = 720; }
	inline void SDL_SetWindowPosition(SDL_Window* w, int x, int y) {}
	inline void SDL_SetWindowResizable(SDL_Window* w, int r) {}
	inline void SDL_SetWindowBordered(SDL_Window* w, int b) {}
	inline int SDL_ShowCursor(int t) { return 0; }
	struct SDL_Rect { int x, y, w, h; };
	inline int SDL_GetDisplayBounds(int i, SDL_Rect* r) { if (r) { r->x = r->y = 0; r->w = 1920; r->h = 1080; } return 0; }
	struct SDL_DisplayMode { int w, h; };
	inline int SDL_GetDesktopDisplayMode(int i, SDL_DisplayMode* m) { if (m) { m->w = 1920; m->h = 1080; } return 0; }

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
	#define SDL_JOYDEVICEADDED 11
	#define SDL_JOYDEVICEREMOVED 12
	#define SDL_APP_WILLENTERBACKGROUND 13
	#define SDL_HINT_VIDEO_ALLOW_SCREENSAVER "SDL_VIDEO_ALLOW_SCREENSAVER"
	#define SDL_HINT_ACCELEROMETER_AS_JOYSTICK "SDL_ACCELEROMETER_AS_JOYSTICK"
	#define SDL_HINT_RENDER_VSYNC "SDL_RENDER_VSYNC"

	#define SDL_PIXELFORMAT_INDEX1LSB 0
	#define SDL_PIXELFORMAT_INDEX1MSB 1
	#define SDL_PIXELFORMAT_INDEX4LSB 2
	#define SDL_PIXELFORMAT_INDEX4MSB 3
	#define SDL_PIXELFORMAT_INDEX8 4
	#define SDL_PIXELFORMAT_RGB332 5
	#define SDL_PIXELFORMAT_RGB444 6
	#define SDL_PIXELFORMAT_RGB555 7
	#define SDL_PIXELFORMAT_BGR555 8
	#define SDL_PIXELFORMAT_ARGB4444 9
	#define SDL_PIXELFORMAT_RGBA4444 10
	#define SDL_PIXELFORMAT_ABGR4444 11
	#define SDL_PIXELFORMAT_BGRA4444 12
	#define SDL_PIXELFORMAT_ARGB1555 13
	#define SDL_PIXELFORMAT_RGBA5551 14
	#define SDL_PIXELFORMAT_ABGR1555 15
	#define SDL_PIXELFORMAT_BGRA5551 16
	#define SDL_PIXELFORMAT_RGB565 17
	#define SDL_PIXELFORMAT_BGR565 18
	#define SDL_PIXELFORMAT_RGB24 19
	#define SDL_PIXELFORMAT_BGR24 20
	#define SDL_PIXELFORMAT_RGB888 21
	#define SDL_PIXELFORMAT_RGBX8888 22
	#define SDL_PIXELFORMAT_BGR888 23
	#define SDL_PIXELFORMAT_BGRX8888 24
	#define SDL_PIXELFORMAT_ARGB8888 25
	#define SDL_PIXELFORMAT_RGBA8888 26
	#define SDL_PIXELFORMAT_ABGR8888 27
	#define SDL_PIXELFORMAT_BGRA8888 28
	#define SDL_PIXELFORMAT_ARGB2101010 29
	#define SDL_PIXELFORMAT_YV12 30
	#define SDL_PIXELFORMAT_IYUV 31
	#define SDL_PIXELFORMAT_YUY2 32
	#define SDL_PIXELFORMAT_UYVY 33
	#define SDL_PIXELFORMAT_YVYU 34
	#define SDL_PIXELFORMAT_NV12 35
	#define SDL_PIXELFORMAT_NV21 36

	#define SDL_FALSE 0
	#define SDL_TRUE 1
	#define SDL_PRESSED 1
	#define SDL_BUTTON_LEFT 1
	#define SDL_BUTTON_RIGHT 2
	#define SDL_BUTTON_MIDDLE 3
	#define SDL_BUTTON_X1 4
	#define SDL_BUTTON_X2 5
	#define SDL_AUDIO_PLAYING 1

	#define SDL_WINDOWEVENT_FOCUS_LOST 1
	#define SDL_MESSAGEBOX_ERROR 0x00000010
	#define SDL_MESSAGEBOX_WARNING 0x00000020
	#define SDL_MESSAGEBOX_INFORMATION 0x00000040
	#define SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT 0x00000001
	#define SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT 0x00000002
	#define SDL_WINDOW_FULLSCREEN_DESKTOP 0x1000
	#define SDL_WINDOWPOS_CENTERED_MASK 0x2FFF0000u
	#define SDL_WINDOWPOS_CENTERED_DISPLAY(X) (SDL_WINDOWPOS_CENTERED_MASK|(X))

	inline void SDL_CloseAudioDevice(SDL_AudioDeviceID d) {}
	inline void SDL_DisableScreenSaver() {}
	#define SDL_AUDIO_ALLOW_ANY_CHANGE 0
	inline SDL_AudioDeviceID SDL_OpenAudioDevice(const char* d, int is, SDL_AudioSpec* des, SDL_AudioSpec* obt, int f) { return 0; }
	inline void SDL_Delay(unsigned int ms) {}

	// OpenGL Shims for PS3
	typedef int GLint;
	typedef int GLenum;
	typedef unsigned int GLuint;
	typedef float GLfloat;
	typedef int GLsizei;
	typedef long GLintptr;
	typedef long GLsizeiptr;
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
	#define GL_TEXTURE1 0x84C1
	#define GL_TEXTURE2 0x84C2
	#define GL_ONE 0
	#define GL_ZERO 0
	#define GL_SRC_ALPHA 0
	#define GL_ONE_MINUS_SRC_ALPHA 0
	#define GL_DST_COLOR 0
	#define GL_COMPILE_STATUS 0
	#define GL_LINK_STATUS 0
	#define GL_INFO_LOG_LENGTH 0
	#define GL_NONE 0
	#define GL_INVALID_OPERATION 1
	#define GL_INVALID_ENUM 2
	#define GL_INVALID_VALUE 3
	#define GL_OUT_OF_MEMORY 4
	#define GL_INVALID_FRAMEBUFFER_OPERATION 5
	#define GL_COLOR_ATTACHMENT0 0
	#define GL_TEXTURE_MIN_FILTER 0
	#define GL_TEXTURE_MAG_FILTER 0
	#define GL_NEAREST 0
	#define GL_LINEAR 1
	#define GL_TEXTURE_WRAP_S 0
	#define GL_TEXTURE_WRAP_T 0
	#define GL_CLAMP_TO_EDGE 0
	#define GL_REPEAT 1
	#define GL_TRIANGLES 0
	#define GL_DEPTH_ATTACHMENT 0x8D00
	#define GL_DEPTH_TEST 0x0B71
	#define GL_FRAMEBUFFER_BINDING 0x8CA6
	#define GL_VIEWPORT 0x0BA2
	#define GL_ALWAYS 0x0207
	#define GL_GEQUAL 0x0206
	#define GL_TEXTURE_BUFFER 0x8C2A
	#define GL_LUMINANCE 0x1909
	#define GL_BLEND 0
	#define GL_FUNC_ADD 0
	#define GL_FUNC_REVERSE_SUBTRACT 0
	#define GL_MIN 0
	#define GL_MAX 0
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
	inline void glTexParameteri(GLenum t, GLenum p, GLint v) {}
	inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) {}
	inline void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) {}
	inline void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {}
	inline void glClearColor(float r, float g, float b, float a) {}
	inline void glClear(int m) {}
	inline void glEnable(GLenum cap) {}
	inline void glDisable(GLenum cap) {}
	inline void glViewport(int x, int y, int w, int h) {}
	inline void glReadPixels(int x, int y, int w, int h, int f, int t, void* d) {}
	inline void glGetTexImage(GLenum t, GLint l, GLenum f, GLenum ty, void* p) {}
	inline void glScissor(int x, int y, int w, int h) {}
	inline void glClearDepth(float d) {}
	inline void glDepthRange(float n, float f) {}
	inline void glDepthMask(unsigned char m) {}
	inline void glDepthFunc(GLenum f) {}
	inline void glGetIntegerv(GLenum p, GLint* v) {}
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
	inline void glBlendEquation(GLenum m) {}
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
	inline int SDL_GL_SetAttribute(int a, int v) { return 0; }
	#define SDL_GL_RED_SIZE 1
	#define SDL_GL_GREEN_SIZE 2
	#define SDL_GL_BLUE_SIZE 3
	#define SDL_GL_DOUBLEBUFFER 4
	#define SDL_GL_CONTEXT_PROFILE_MASK 5
	#define SDL_GL_CONTEXT_PROFILE_CORE 6
	#define SDL_GL_CONTEXT_MAJOR_VERSION 7
	#define SDL_GL_CONTEXT_MINOR_VERSION 8

	struct SDL_PixelFormat { uint32 format; };
	struct SDL_Surface { int w, h; void* pixels; SDL_PixelFormat* format; };
	inline SDL_Surface* SDL_CreateRGBSurfaceFrom(void* p, int w, int h, int d, int s, Uint32 r, Uint32 g, Uint32 b, Uint32 a) { return (SDL_Surface*)1; }
	inline SDL_Surface* SDL_GetWindowSurface(SDL_Window* w) { return (SDL_Surface*)1; }
	inline int SDL_LockSurface(SDL_Surface* s) { return 0; }
	inline void SDL_UnlockSurface(SDL_Surface* s) {}
	inline int SDL_UpdateWindowSurface(SDL_Window* w) { return 0; }
	inline void SDL_SetWindowIcon(SDL_Window* w, SDL_Surface* i) {}
	inline void SDL_FreeSurface(SDL_Surface* s) {}

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
