/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/helper/TextInputHandler.h"
#include "oxygen/platform/PlatformFunctions.h"


void TextInputHandler::setText(std::wstring_view text, bool moveCursorToEnd)
{
	mText.assign(text.data(), text.length());
	setCursorPosition(moveCursorToEnd ? mText.length() : mCursorPosition);
}

void TextInputHandler::setCursorPosition(size_t position)
{
	mCursorPosition = (size_t)clamp((int)position, 0, (int)mText.length());
#if defined(PLATFORM_PS3)
	mMarkedRangeStart = std::optional<size_t>();
#else
	mMarkedRangeStart.reset();
#endif
}

void TextInputHandler::keyboard(const rmx::KeyboardEvent& ev)
{
	if (!ev.state)
		return;

	switch (ev.key)
	{
		case SDLK_LEFT:
		{
			if (mCursorPosition > 0)
				moveCursorTo(mCursorPosition - 1);
			break;
		}

		case SDLK_RIGHT:
		{
			moveCursorTo(mCursorPosition + 1);
			break;
		}

		case SDLK_HOME:
		{
			moveCursorTo(0);
			break;
		}

		case SDLK_END:
		{
			moveCursorTo(mText.length());
			break;
		}

		case SDLK_BACKSPACE:
		{
			if ((bool)mMarkedRangeStart)
			{
				deleteMarkedRange();
			}
			else if (mCursorPosition > 0)
			{
				--mCursorPosition;
				mText.erase(mCursorPosition, 1);
			}
			break;
		}

		case SDLK_DELETE:
		{
			if ((bool)mMarkedRangeStart)
			{
				deleteMarkedRange();
			}
			else
			{
				mText.erase(mCursorPosition, 1);
			}
			break;
		}

		case 'a':
		{
			if ((SDL_GetModState() & KMOD_CTRL) != 0)
			{
				if (mText.empty())
				{
					mCursorPosition = 0;
#if defined(PLATFORM_PS3)
					mMarkedRangeStart = std::optional<size_t>();
#else
					mMarkedRangeStart.reset();
#endif
				}
				else
				{
					mCursorPosition = mText.length();
					mMarkedRangeStart = std::optional<size_t>(0);
				}
			}
			break;
		}

		case 'c':
		case 'x':
		{
			if (PlatformFunctions::hasClipboardSupport() && (SDL_GetModState() & KMOD_CTRL) != 0)
			{
				if ((bool)mMarkedRangeStart)
				{
					const size_t rangeStart = std::min(mMarkedRangeStart.value(), mCursorPosition);
					const size_t rangeEnd = std::max(mMarkedRangeStart.value(), mCursorPosition);
					const std::wstring markedText = std::wstring(mText).substr(rangeStart, rangeEnd - rangeStart);
					PlatformFunctions::copyToClipboard(markedText);

					if (ev.key == 'x')
					{
						deleteMarkedRange();
					}
				}
			}
			break;
		}

		case 'v':
		{
			if (PlatformFunctions::hasClipboardSupport() && (SDL_GetModState() & KMOD_CTRL) != 0)
			{
				WString text;
				if (PlatformFunctions::pasteFromClipboard(text))
				{
					insertText(std::wstring_view(*text, text.length()));
				}
			}
			break;
		}
	}
}

void TextInputHandler::textinput(const rmx::TextInputEvent& ev)
{
	insertText(std::wstring_view(*ev.text, ev.text.length()));
}

void TextInputHandler::insertText(std::wstring_view text)
{
	if ((bool)mMarkedRangeStart)
	{
		deleteMarkedRange();
	}

	mText.insert(mCursorPosition, text.data(), text.length());
	moveCursorTo(mCursorPosition + text.length(), false);
}

void TextInputHandler::moveCursorTo(size_t position, bool considerShift)
{
	const size_t oldCursorPosition = mCursorPosition;
	mCursorPosition = (size_t)clamp((int)position, 0, (int)mText.length());

	if (considerShift && (SDL_GetModState() & KMOD_SHIFT) != 0)
	{
		if ((bool)mMarkedRangeStart)
		{
			// Reset if marked range start is empty
			if (mCursorPosition == mMarkedRangeStart.value())
#if defined(PLATFORM_PS3)
				mMarkedRangeStart = std::optional<size_t>();
#else
				mMarkedRangeStart.reset();
#endif
		}
		else
		{
			mMarkedRangeStart = std::optional<size_t>(oldCursorPosition);
		}
	}
	else
	{
#if defined(PLATFORM_PS3)
		mMarkedRangeStart = std::optional<size_t>();
#else
		mMarkedRangeStart.reset();
#endif
	}
}

void TextInputHandler::deleteMarkedRange()
{
	const size_t rangeStart = std::min(mMarkedRangeStart.value(), mCursorPosition);
	const size_t rangeEnd = std::max(mMarkedRangeStart.value(), mCursorPosition);
	mText.erase(rangeStart, rangeEnd - rangeStart);
	mCursorPosition = rangeStart;
#if defined(PLATFORM_PS3)
	mMarkedRangeStart = std::optional<size_t>();
#else
	mMarkedRangeStart.reset();
#endif
}
