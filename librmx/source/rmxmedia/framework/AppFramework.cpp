/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"
#pragma warning(disable: 4005)	// Macro redefinition of APIENTRY


namespace rmx
{

	VideoConfig::VideoConfig() : mColorDepth(32), mFullscreen(false), mPositioning(false), mResizeable(true), mBorderless(false), mVSync(true), mHideCursor(false), mMultisampling(0), mAutoClearScreen(true), mAutoSwapBuffers(true), mIconResource(0), mRenderer(Renderer_OPENGL)
	{
		mWindowRect.set(0, 0, 800, 500);
		mCaption = "FTX Window";
	}

	VideoConfig::VideoConfig(bool fullscreen, int width, int height, const String& caption) : mColorDepth(32), mFullscreen(false), mPositioning(false), mResizeable(true), mBorderless(false), mVSync(true), mHideCursor(false), mMultisampling(0), mAutoClearScreen(true), mAutoSwapBuffers(true), mIconResource(0), mRenderer(Renderer_OPENGL)
	{
		mFullscreen = fullscreen;
		mResizeable = !fullscreen;
		mWindowRect.set(0, 0, width, height);
		mColorDepth = 32;
		mCaption = "FTX Window";
		mMultisampling = 0;
		mAutoClearScreen = true;
		mAutoSwapBuffers = true;
		if (caption.nonEmpty())
			mCaption = caption;
	}



	InputContext::InputContext()
	{
		memset(this, 0, sizeof(InputContext));
	}

	InputContext::~InputContext()
	{
	}

	void InputContext::copy(const InputContext& source)
	{
		memcpy(this, &source, sizeof(InputContext));
		mKeyState = source.mKeyState;
		mKeyChange = source.mKeyChange;
	}

	void InputContext::applyEvent(const KeyboardEvent& ev)
	{
		const size_t bit = getBitIndex(ev.key);
		if (mKeyState.isBitSet(bit) != ev.state)
		{
			mKeyState.setBit(bit, ev.state);
			mKeyChange.setBit(bit, true);
		}
	}

	void InputContext::applyEvent(const MouseEvent& ev)
	{
		mMousePos = ev.position;
		if (mMouseState[ev.button] != ev.state)
		{
			mMouseState[ev.button] = ev.state;
			mMouseChange[ev.button] = true;
		}
	}

	bool InputContext::getMouseState(int button) const
	{
		return (button >= 0 && button < 5) ? mMouseState[button] : false;
	}

	bool InputContext::getMouseChange(int button) const
	{
		return (button >= 0 && button < 5) ? mMouseChange[button] : false;
	}

}



namespace FTX
{
	int screenWidth()  { return Video->getScreenWidth(); }
	int screenHeight() { return Video->getScreenHeight(); }
	Vec2i screenSize() { return Video->getScreenSize(); }
	const Recti& screenRect() { return Video->getScreenRect(); }
	bool reshaped()    { return Video->reshaped(); }

	float getTimeDifference() { return System->getTimeDifference(); }
	float getTime()           { return System->getTime(); }
	float getFramerate()      { return System->getFramerate(); }
	int   getFrameCounter()   { return System->getFrameCounter(); }

	bool keyState(int key)  { return System->getKeyState(key); }
	bool keyChange(int key) { return System->getKeyChange(key); }

	const Vec2i& mousePos() { return System->getMousePos(); }
	const Vec2i& mouseRel() { return System->getMouseRel(); }
	int  mouseWheel() { return System->getMouseWheel(); }
	bool mouseState(rmx::MouseButton button)  { return System->getMouseState((int)button); }
	bool mouseChange(rmx::MouseButton button) { return System->getMouseChange((int)button); }
	bool mouseIn(const Recti& rect) { return System->mouseIn(rect); }
}
