/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"

#if defined(PLATFORM_WINDOWS)
	#pragma warning(disable: 4005)	// Macro redefinition of APIENTRY

	#if defined(__GNUC__)
		#include <SDL2/SDL_syswm.h>
	#else
		#include <SDL/SDL_syswm.h>
	#endif

	#define WIN32_LEAN_AND_MEAN
	#include "CleanWindowsInclude.h"

#elif defined(PLATFORM_WEB)
	#include <emscripten.h>
	#include <emscripten/html5.h>

#endif


namespace rmx
{

	FTX_SystemManager::FTX_SystemManager() : mInitialized(false), mRunning(false), mTicks(0), mTotalTime(0.0f), mTimeDifference(0.0f), mFrameRate(0.0f), mFrameCounter(0)
	{
	}

	FTX_SystemManager::~FTX_SystemManager()
	{
	}

	bool FTX_SystemManager::initialize()
	{
		if (mInitialized)
			return true;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
		{
			RMX_ERROR("Failed to initialize SDL: " << SDL_GetError(), return false);
		}

		mInitialized = true;
		return true;
	}

	void FTX_SystemManager::exit()
	{
		if (!mInitialized)
			return;

		SDL_Quit();
		mInitialized = false;
	}

	void FTX_SystemManager::mainLoop()
	{
		mRunning = true;
		while (mRunning)
		{
			run();
		}
	}

	void FTX_SystemManager::run(GuiBase& app)
	{
		mRoot.addChild(&app);
		mainLoop();
		mRoot.removeChild(&app);
	}

	void FTX_SystemManager::quit()
	{
		mRunning = false;
	}

	void FTX_SystemManager::warpMouse(int x, int y)
	{
		if (FTX::Video->isActive())
		{
			SDL_WarpMouseInWindow(FTX::Video->getMainWindow(), x, y);
		}
	}

	void FTX_SystemManager::run()
	{
#if defined(PLATFORM_PS3) || defined(__PS3__) || defined(__CELLOS_LV2__)
		static uint32 lastFtxHeartbeat = 0;
		uint32 now = SDL_GetTicks();

		if (now - lastFtxHeartbeat > 1000)
		{
			printf("PS3 Heartbeat: FTX_SystemManager::run\n");
			fflush(stdout);
			lastFtxHeartbeat = now;
		}
#endif
		startTick();
		checkSDLEvents();
		update();
		render();
	}

	void FTX_SystemManager::startTick()
	{
		const uint32 newTicks = SDL_GetTicks();
		if (mTicks != 0)
		{
			mTimeDifference = (float)(newTicks - mTicks) / 1000.0f;
			mTotalTime += mTimeDifference;

			if (mTimeDifference > 0.0f)
			{
				const float newFrameRate = 1.0f / mTimeDifference;
				mFrameRate = mFrameRate * 0.95f + newFrameRate * 0.05f;
			}
		}
		mTicks = newTicks;
		++mFrameCounter;
	}

	void FTX_SystemManager::checkSDLEvents()
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
				case SDL_QUIT:
					quit();
					break;

				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
						reshape(ev.window.data1, ev.window.data2);
					break;

				case SDL_KEYDOWN:
				case SDL_KEYUP:
					keyboard(ev.key);
					break;

				case SDL_TEXTINPUT:
					textinput(ev.text);
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					mouse(ev.button);
					break;

				case SDL_MOUSEWHEEL:
					mousewheel(ev.wheel);
					break;

				case SDL_MOUSEMOTION:
					{
						MouseEvent mouseEv;
						mouseEv.position.set(ev.motion.x, ev.motion.y);
						mouseEv.state = (ev.motion.state != 0);
						mInputContext.applyEvent(mouseEv);
						mRoot.mouse(mouseEv);
					}
					break;
			}
		}
	}

	void FTX_SystemManager::reshape(int width, int height)
	{
		FTX::Video->reshape(width, height);
	}

	void FTX_SystemManager::keyboard(const SDL_KeyboardEvent& ev)
	{
		KeyboardEvent keyboardEv;
		keyboardEv.key = (int)ev.keysym.sym;
		keyboardEv.scancode = (uint32)ev.keysym.scancode;
		keyboardEv.modifiers = ev.keysym.mod;
		keyboardEv.state = (ev.state == SDL_PRESSED);
		keyboardEv.repeat = (ev.repeat != 0);

		mInputContext.applyEvent(keyboardEv);
		mRoot.keyboard(keyboardEv);
	}

	void FTX_SystemManager::textinput(const SDL_TextInputEvent& ev)
	{
		TextInputEvent textInputEv;
		textInputEv.text.fromUTF8(ev.text, strlen(ev.text));
		mRoot.textinput(textInputEv);
	}

	void FTX_SystemManager::mouse(const SDL_MouseButtonEvent& ev)
	{
		MouseEvent mouseEv;
		switch (ev.button)
		{
			case SDL_BUTTON_LEFT:   mouseEv.button = MouseButton_Left;   break;
			case SDL_BUTTON_RIGHT:  mouseEv.button = MouseButton_Right;  break;
			case SDL_BUTTON_MIDDLE: mouseEv.button = MouseButton_Middle; break;
			case SDL_BUTTON_X1:     mouseEv.button = MouseButton_Button4; break;
			case SDL_BUTTON_X2:     mouseEv.button = MouseButton_Button5; break;
		}
		mouseEv.state = (ev.state == SDL_PRESSED);
		mouseEv.position.set(ev.x, ev.y);

		mInputContext.applyEvent(mouseEv);
		mRoot.mouse(mouseEv);
	}

	void FTX_SystemManager::mousewheel(const SDL_MouseWheelEvent& ev)
	{
		mInputContext.mMouseWheel += ev.y;
	}

	void FTX_SystemManager::update()
	{
		mRoot.update(mTimeDifference);
	}

	void FTX_SystemManager::render()
	{
		if (FTX::Video->isActive())
		{
			FTX::Video->beginRendering();
			mRoot.render();
			FTX::Video->endRendering();
		}
	}



	FTX_VideoManager::FTX_VideoManager() : mInitialized(false), mReshaped(false), mMainWindow(nullptr)
	{
	}

	FTX_VideoManager::~FTX_VideoManager()
	{
		if (nullptr != mMainWindow)
			SDL_DestroyWindow(mMainWindow);
	}

	bool FTX_VideoManager::initialize(const VideoConfig& videoconfig)
	{
		mVideoConfig = videoconfig;

		if (!setVideoMode(mVideoConfig))
			return false;

		mInitialized = true;
		return true;
	}

	void FTX_VideoManager::setInitialized(const VideoConfig& videoconfig, SDL_Window* window)
	{
		mVideoConfig = videoconfig;
		mMainWindow = window;
		mInitialized = true;
	}

	void FTX_VideoManager::reshape(int width, int height)
	{
		mVideoConfig.mWindowRect.width = width;
		mVideoConfig.mWindowRect.height = height;
		mReshaped = true;
	}

	void FTX_VideoManager::beginRendering()
	{
		if (mVideoConfig.mAutoClearScreen)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void FTX_VideoManager::endRendering()
	{
		if (mVideoConfig.mAutoSwapBuffers)
		{
			SDL_GL_SwapWindow(mMainWindow);
		}
		mReshaped = false;
	}

	void FTX_VideoManager::setPixelView()
	{
		glViewport(0, 0, mVideoConfig.mWindowRect.width, mVideoConfig.mWindowRect.height);
	}

	void FTX_VideoManager::setPerspective2D(double fov, double dnear, double dfar)
	{
		// TODO
	}

	void FTX_VideoManager::getScreenBitmap(Bitmap& bitmap)
	{
		bitmap.create(mVideoConfig.mWindowRect.width, mVideoConfig.mWindowRect.height);
		glReadPixels(0, 0, bitmap.getWidth(), bitmap.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, bitmap.getData());
		bitmap.mirrorVertical();
	}

	uint64 FTX_VideoManager::getNativeWindowHandle() const
	{
#if defined(PLATFORM_WINDOWS)
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(mMainWindow, &wmInfo);
		return (uint64)wmInfo.info.win.window;
#else
		return 0;
#endif
	}

	bool FTX_VideoManager::setVideoMode(const VideoConfig& videoconfig)
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (videoconfig.mFullscreen) flags |= SDL_WINDOW_FULLSCREEN;
		if (videoconfig.mResizeable) flags |= SDL_WINDOW_RESIZABLE;
		if (videoconfig.mBorderless) flags |= SDL_WINDOW_BORDERLESS;

		mMainWindow = SDL_CreateWindow(*videoconfig.mCaption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, videoconfig.mWindowRect.width, videoconfig.mWindowRect.height, flags);
		if (nullptr == mMainWindow)
		{
			RMX_ERROR("Failed to create window: " << SDL_GetError(), return false);
		}

		SDL_GLContext glContext = SDL_GL_CreateContext(mMainWindow);
		if (nullptr == glContext)
		{
			RMX_ERROR("Failed to create OpenGL context: " << SDL_GetError(), return false);
		}

		if (videoconfig.mVSync)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

#ifdef RMX_USE_GLEW
		if (glewInit() != GLEW_OK)
		{
			RMX_ERROR("Failed to initialize GLEW", return false);
		}
#endif

		return true;
	}

}
