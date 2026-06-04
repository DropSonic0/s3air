/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	AppFramework
*		Management of the application, incl. input and output.
*/

#pragma once


namespace rmx
{
	struct KeyboardEvent
	{
		int key;
		uint32 scancode;
		uint16 modifiers;
		bool state;
		bool repeat;

		KeyboardEvent() : key(0), scancode(0), modifiers(0), state(false), repeat(false) {}
	};

	struct TextInputEvent
	{
		WString text;
	};

	enum MouseButton
	{
		MouseButton_Left = 0,
		MouseButton_Right,
		MouseButton_Middle,
		MouseButton_Button4,
		MouseButton_Button5
	};

	struct MouseEvent
	{
		MouseButton button;
		bool state;
		Vec2i position;

		MouseEvent() : button(MouseButton_Left), state(false) {}
	};


	struct VideoConfig
	{
		enum Renderer
		{
			Renderer_SOFTWARE,
			Renderer_OPENGL
		};

		Recti mWindowRect;				// Window rect on screen
		Vec2i mStartPos;				// Upper left corner
		int mColorDepth;			// Fullscreen only: Color depth in bits per pixel
		bool mFullscreen;		// (Exclusive) fullscreen or window mode?
		bool mPositioning;		// Window mode only: Use "mStartPos"
		bool mResizeable;		// Window mode only: Resizable window
		bool mBorderless;		// Window mode only: Hide window frame / borderless window
		bool mVSync;				// Use vertical sync
		bool mHideCursor;		// Show or hide mouse cursor
		int mMultisampling;			// Multisampling setting, usually 0 to disable
		bool mAutoClearScreen;	// Automatically clear screen before rendering
		bool mAutoSwapBuffers;	// Automatically swap buffers after rendering
		int mIconResource;			// Resource number of icon
		Bitmap mIconBitmap;				// Bitmap of icon, as an alternative for "iconSource"
		String mIconSource;				// Source file for icon (in PNG format)
		String mCaption;				// Caption text for window
		Renderer mRenderer;

		VideoConfig();
		VideoConfig(bool fullscreen, int width, int height, const String& caption = "");
	};



	class InputContext
	{
	public:
		InputContext();
		~InputContext();

		void copy(const InputContext& source);

		void applyEvent(const KeyboardEvent& ev);
		void applyEvent(const MouseEvent& ev);

		inline size_t getBitIndex(int key) const { return (key & 0x01ff) + ((key & SDLK_SCANCODE_MASK) >> 21); }
		inline bool getKeyState(int key) const	 { return mKeyState.isBitSet(getBitIndex(key)); }
		inline bool getKeyChange(int key) const	 { return mKeyChange.isBitSet(getBitIndex(key)); }

		bool getMouseState(int button) const;
		bool getMouseChange(int button) const;

	public:
		BitArray<0x400> mKeyState;
		BitArray<0x400> mKeyChange;
		Vec2i mMousePos;
		Vec2i mMouseRel;
		int   mMouseWheel;
		bool  mMouseState[5];
		bool  mMouseChange[5];
	};
}


namespace FTX
{
	// Video
	int   screenWidth();
	int   screenHeight();
	Vec2i screenSize();
	const Recti& screenRect();
	bool  reshaped();

	// Timing
	float getTimeDifference();
	float getTime();
	float getFramerate();
	int   getFrameCounter();

	// Keyboard
	bool keyState(int key);
	bool keyChange(int key);

	// Mouse
	const Vec2i& mousePos();
	const Vec2i& mouseRel();
	int  mouseWheel();
	bool mouseState(rmx::MouseButton button);
	bool mouseChange(rmx::MouseButton button);
	bool mouseIn(const Recti& rect);
}
