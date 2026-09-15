/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <optional>
#else
struct MarkedRangeStartFallback
{
	size_t mValue = 0xffffffff;
	bool has_value() const { return mValue != 0xffffffff; }
	size_t operator*() const { return mValue; }
	void reset() { mValue = 0xffffffff; }
	MarkedRangeStartFallback& operator=(size_t val) { mValue = val; return *this; }
};
#endif


class TextInputHandler
{
public:
	inline const std::wstring& getText() const   { return mText; }
	void setText(std::wstring_view text, bool moveCursorToEnd = false);

	inline size_t getCursorPosition() const  { return mCursorPosition; }
	void setCursorPosition(size_t position);

#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	const std::optional<size_t>& getMarkedRangeStart() const  { return mMarkedRangeStart; }
#else
	inline const MarkedRangeStartFallback& getMarkedRangeStart() const { return mMarkedRangeStart; }
#endif

	void keyboard(const rmx::KeyboardEvent& ev);
	void textinput(const rmx::TextInputEvent& ev);

private:
	void insertText(std::wstring_view text);
	void moveCursorTo(size_t position, bool considerShift = true);
	void deleteMarkedRange();

private:
	std::wstring mText;
	size_t mCursorPosition = 0;
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	std::optional<size_t> mMarkedRangeStart;
#else
	MarkedRangeStartFallback mMarkedRangeStart;
#endif
};
