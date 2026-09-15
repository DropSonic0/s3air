/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/application/input/InputConfig.h"


namespace
{
	struct AssignmentLookup
	{
#if defined(PLATFORM_PS3)
		static std::vector<std::pair<String, uint32> > KEYS_AND_IDENTIFIERS;
		static void setup()
		{
			if (!KEYS_AND_IDENTIFIERS.empty()) return;
			// This list uses the same sorting as the SDLK_* definitions
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Enter",			(uint32)SDLK_RETURN));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Esc",			(uint32)SDLK_ESCAPE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Backspace",		(uint32)SDLK_BACKSPACE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Tab",			(uint32)SDLK_TAB));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Space",			(uint32)SDLK_SPACE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Exclaim",		(uint32)SDLK_EXCLAIM));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("QuoteDbl",		(uint32)SDLK_QUOTEDBL));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Hash",			(uint32)SDLK_HASH));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Percent",		(uint32)SDLK_PERCENT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Dollar",			(uint32)SDLK_DOLLAR));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Ampersand",		(uint32)SDLK_AMPERSAND));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Quote",			(uint32)SDLK_QUOTE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("LeftParen",		(uint32)SDLK_LEFTPAREN));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("RightParen",		(uint32)SDLK_RIGHTPAREN));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Asterisk",		(uint32)SDLK_ASTERISK));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Plus",			(uint32)SDLK_PLUS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Comma",			(uint32)SDLK_COMMA));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Minus",			(uint32)SDLK_MINUS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Perios",			(uint32)SDLK_PERIOD));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Slash",			(uint32)SDLK_SLASH));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("0",				(uint32)SDLK_0));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("1",				(uint32)SDLK_1));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("2",				(uint32)SDLK_2));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("3",				(uint32)SDLK_3));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("4",				(uint32)SDLK_4));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("5",				(uint32)SDLK_5));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("6",				(uint32)SDLK_6));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("7",				(uint32)SDLK_7));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("8",				(uint32)SDLK_8));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("9",				(uint32)SDLK_9));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Colon",			(uint32)SDLK_COLON));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Semicolon",		(uint32)SDLK_SEMICOLON));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Less",			(uint32)SDLK_LESS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Equals",			(uint32)SDLK_EQUALS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Greater",		(uint32)SDLK_GREATER));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Question",		(uint32)SDLK_QUESTION));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("At",				(uint32)SDLK_AT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("LeftBracket",	(uint32)SDLK_LEFTBRACKET));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Backslash",		(uint32)SDLK_BACKSLASH));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("RightBracket",	(uint32)SDLK_RIGHTBRACKET));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Caret",			(uint32)SDLK_CARET));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Underscore",		(uint32)SDLK_UNDERSCORE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("BackQuote",		(uint32)SDLK_BACKQUOTE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("A",				(uint32)SDLK_a));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("B",				(uint32)SDLK_b));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("C",				(uint32)SDLK_c));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("D",				(uint32)SDLK_d));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("E",				(uint32)SDLK_e));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("F",				(uint32)SDLK_f));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("G",				(uint32)SDLK_g));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("H",				(uint32)SDLK_h));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("I",				(uint32)SDLK_i));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("J",				(uint32)SDLK_j));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("K",				(uint32)SDLK_k));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("L",				(uint32)SDLK_l));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("M",				(uint32)SDLK_m));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("N",				(uint32)SDLK_n));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("O",				(uint32)SDLK_o));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("P",				(uint32)SDLK_p));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Q",				(uint32)SDLK_q));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("R",				(uint32)SDLK_r));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("S",				(uint32)SDLK_s));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("T",				(uint32)SDLK_t));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("U",				(uint32)SDLK_u));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("V",				(uint32)SDLK_v));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("W",				(uint32)SDLK_w));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("X",				(uint32)SDLK_x));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Y",				(uint32)SDLK_y));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Z",				(uint32)SDLK_z));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("CapsLock",		(uint32)SDLK_CAPSLOCK));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Insert",			(uint32)SDLK_INSERT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Home",			(uint32)SDLK_HOME));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("PageUp",			(uint32)SDLK_PAGEUP));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Delete",			(uint32)SDLK_DELETE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("End",			(uint32)SDLK_END));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("PageDown",		(uint32)SDLK_PAGEDOWN));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Up",				(uint32)SDLK_UP));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Down",			(uint32)SDLK_DOWN));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Left",			(uint32)SDLK_LEFT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Right",			(uint32)SDLK_RIGHT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadDivide",	(uint32)SDLK_KP_DIVIDE));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadMultiply", (uint32)SDLK_KP_MULTIPLY));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadMinus",	(uint32)SDLK_KP_MINUS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadPlus",		(uint32)SDLK_KP_PLUS));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadEnter",	(uint32)SDLK_KP_ENTER));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad1",		(uint32)SDLK_KP_1));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad2",		(uint32)SDLK_KP_2));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad3",		(uint32)SDLK_KP_3));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad4",		(uint32)SDLK_KP_4));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad5",		(uint32)SDLK_KP_5));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad6",		(uint32)SDLK_KP_6));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad7",		(uint32)SDLK_KP_7));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad8",		(uint32)SDLK_KP_8));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad9",		(uint32)SDLK_KP_9));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("Numpad0",		(uint32)SDLK_KP_0));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("NumpadPeriod",	(uint32)SDLK_KP_PERIOD));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("LeftShift",		(uint32)SDLK_LSHIFT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("RightShift",		(uint32)SDLK_RSHIFT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("LeftCtrl",		(uint32)SDLK_LCTRL));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("RightCtrl",		(uint32)SDLK_RCTRL));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("LeftAlt",		(uint32)SDLK_LALT));
			KEYS_AND_IDENTIFIERS.push_back(std::make_pair("RightAlt",		(uint32)SDLK_RALT));
		}
#else
		static inline const std::vector<std::pair<String, uint32>> KEYS_AND_IDENTIFIERS =
		{
			// This list uses the same sorting as the SDLK_* definitions
			{ "Enter",			SDLK_RETURN },
			{ "Esc",			SDLK_ESCAPE },
			{ "Backspace",		SDLK_BACKSPACE },
			{ "Tab",			SDLK_TAB },
			{ "Space",			SDLK_SPACE },
			{ "Exclaim",		SDLK_EXCLAIM },
			{ "QuoteDbl",		SDLK_QUOTEDBL },
			{ "Hash",			SDLK_HASH },
			{ "Percent",		SDLK_PERCENT },
			{ "Dollar",			SDLK_DOLLAR },
			{ "Ampersand",		SDLK_AMPERSAND },
			{ "Quote",			SDLK_QUOTE },
			{ "LeftParen",		SDLK_LEFTPAREN },
			{ "RightParen",		SDLK_RIGHTPAREN },
			{ "Asterisk",		SDLK_ASTERISK },
			{ "Plus",			SDLK_PLUS },
			{ "Comma",			SDLK_COMMA },
			{ "Minus",			SDLK_MINUS },
			{ "Perios",			SDLK_PERIOD },
			{ "Slash",			SDLK_SLASH },
			{ "0",				SDLK_0 },
			{ "1",				SDLK_1 },
			{ "2",				SDLK_2 },
			{ "3",				SDLK_3 },
			{ "4",				SDLK_4 },
			{ "5",				SDLK_5 },
			{ "6",				SDLK_6 },
			{ "7",				SDLK_7 },
			{ "8",				SDLK_8 },
			{ "9",				SDLK_9 },
			{ "Colon",			SDLK_COLON },
			{ "Semicolon",		SDLK_SEMICOLON },
			{ "Less",			SDLK_LESS },
			{ "Equals",			SDLK_EQUALS },
			{ "Greater",		SDLK_GREATER },
			{ "Question",		SDLK_QUESTION },
			{ "At",				SDLK_AT },
			{ "LeftBracket",	SDLK_LEFTBRACKET },
			{ "Backslash",		SDLK_BACKSLASH },
			{ "RightBracket",	SDLK_RIGHTBRACKET },
			{ "Caret",			SDLK_CARET },
			{ "Underscore",		SDLK_UNDERSCORE },
			{ "BackQuote",		SDLK_BACKQUOTE },
			{ "A",				SDLK_a },
			{ "B",				SDLK_b },
			{ "C",				SDLK_c },
			{ "D",				SDLK_d },
			{ "E",				SDLK_e },
			{ "F",				SDLK_f },
			{ "G",				SDLK_g },
			{ "H",				SDLK_h },
			{ "I",				SDLK_i },
			{ "J",				SDLK_j },
			{ "K",				SDLK_k },
			{ "L",				SDLK_l },
			{ "M",				SDLK_m },
			{ "N",				SDLK_n },
			{ "O",				SDLK_o },
			{ "P",				SDLK_p },
			{ "Q",				SDLK_q },
			{ "R",				SDLK_r },
			{ "S",				SDLK_s },
			{ "T",				SDLK_t },
			{ "U",				SDLK_u },
			{ "V",				SDLK_v },
			{ "W",				SDLK_w },
			{ "X",				SDLK_x },
			{ "Y",				SDLK_y },
			{ "Z",				SDLK_z },
			{ "CapsLock",		SDLK_CAPSLOCK },
			// Function keys and some others like PrintScreen intentionally not available
			{ "Insert",			SDLK_INSERT },
			{ "Home",			SDLK_HOME },
			{ "PageUp",			SDLK_PAGEUP },
			{ "Delete",			SDLK_DELETE },
			{ "End",			SDLK_END },
			{ "PageDown",		SDLK_PAGEDOWN },
			{ "Up",				SDLK_UP },
			{ "Down",			SDLK_DOWN },
			{ "Left",			SDLK_LEFT },
			{ "Right",			SDLK_RIGHT },
			{ "NumpadDivide",	SDLK_KP_DIVIDE },
			{ "NumpadMultiply", SDLK_KP_MULTIPLY },
			{ "NumpadMinus",	SDLK_KP_MINUS },
			{ "NumpadPlus",		SDLK_KP_PLUS },
			{ "NumpadEnter",	SDLK_KP_ENTER },
			{ "Numpad1",		SDLK_KP_1 },
			{ "Numpad2",		SDLK_KP_2 },
			{ "Numpad3",		SDLK_KP_3 },
			{ "Numpad4",		SDLK_KP_4 },
			{ "Numpad5",		SDLK_KP_5 },
			{ "Numpad6",		SDLK_KP_6 },
			{ "Numpad7",		SDLK_KP_7 },
			{ "Numpad8",		SDLK_KP_8 },
			{ "Numpad9",		SDLK_KP_9 },
			{ "Numpad0",		SDLK_KP_0 },
			{ "NumpadPeriod",	SDLK_KP_PERIOD },
			// Most of the rest here is left out, as it does not seem important enough
			{ "LeftShift",		SDLK_LSHIFT },
			{ "RightShift",		SDLK_RSHIFT },
			{ "LeftCtrl",		SDLK_LCTRL },
			{ "RightCtrl",		SDLK_RCTRL },
			{ "LeftAlt",		SDLK_LALT },
			{ "RightAlt",		SDLK_RALT },
		};
#endif
#if defined(PLATFORM_PS3)
		static std::map<String, uint32> mKeyByIdentifier;	// Uses lowercase strings for better comparability
		static std::map<uint32, String> mIdentifierByKey;
#else
		static inline std::map<String, uint32> mKeyByIdentifier;	// Uses lowercase strings for better comparability
		static inline std::map<uint32, String> mIdentifierByKey;
#endif

		static uint32 getKeyByIdentifier(const String& identifier)
		{
#if defined(PLATFORM_PS3)
			setup();
#endif
			if (mKeyByIdentifier.empty())
			{
				// Lazy initialization
				for (size_t i = 0; i < KEYS_AND_IDENTIFIERS.size(); ++i)
				{
					const std::pair<String, uint32>& pair = KEYS_AND_IDENTIFIERS[i];
					String key = pair.first;
					key.lowerCase();
					mKeyByIdentifier[key] = pair.second;
				}
			}

			// Search in the lookup map
			{
				static String key;		// Static to avoid reallocations
				key = identifier;
				key.lowerCase();
				const auto it = mKeyByIdentifier.find(key);
				return (it == mKeyByIdentifier.end()) ? 0 : it->second;
			}
		}

		static const String& getIdentifierByKey(uint32 key)
		{
#if defined(PLATFORM_PS3)
			setup();
#endif
			if (mIdentifierByKey.empty())
			{
				// Lazy initialization
				for (size_t i = 0; i < KEYS_AND_IDENTIFIERS.size(); ++i)
				{
					const std::pair<String, uint32>& pair = KEYS_AND_IDENTIFIERS[i];
					mIdentifierByKey[pair.second] = pair.first;
				}
			}

			// Search in the lookup map
			{
				static String EMPTY_STRING;
				const auto it = mIdentifierByKey.find(key);
				return (it == mIdentifierByKey.end()) ? EMPTY_STRING : it->second;
			}
		}
	};

#if defined(PLATFORM_PS3)
	std::vector<std::pair<String, uint32> > AssignmentLookup::KEYS_AND_IDENTIFIERS;
	std::map<String, uint32> AssignmentLookup::mKeyByIdentifier;
	std::map<uint32, String> AssignmentLookup::mIdentifierByKey;

	static std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment> > DEFAULT_KB1_FIXED_ASSIGNMENTS;
	static std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment> > DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS;
	static void setupAssignments()
	{
		if (!DEFAULT_KB1_FIXED_ASSIGNMENTS.empty()) return;

		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::UP,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_UP)));
		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::DOWN,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_DOWN)));
		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::LEFT,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_LEFT)));
		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::RIGHT,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_RIGHT)));
		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::START,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_RETURN)));
		DEFAULT_KB1_FIXED_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::BACK,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_ESCAPE)));

		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::A,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_a)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::B,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_s)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::X,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_d)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::X,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_q)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::Y,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_w)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::BACK,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_BACKSPACE)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::L,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_e)));
		DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::R,	InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, (uint32)SDLK_r)));
	}
#else
	static const std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>> DEFAULT_KB1_FIXED_ASSIGNMENTS =
	{
		{ InputConfig::DeviceDefinition::Button::UP,	{ InputConfig::Assignment::Type::BUTTON, SDLK_UP     } },
		{ InputConfig::DeviceDefinition::Button::DOWN,	{ InputConfig::Assignment::Type::BUTTON, SDLK_DOWN   } },
		{ InputConfig::DeviceDefinition::Button::LEFT,	{ InputConfig::Assignment::Type::BUTTON, SDLK_LEFT   } },
		{ InputConfig::DeviceDefinition::Button::RIGHT,	{ InputConfig::Assignment::Type::BUTTON, SDLK_RIGHT  } },
		{ InputConfig::DeviceDefinition::Button::START,	{ InputConfig::Assignment::Type::BUTTON, SDLK_RETURN } },
		{ InputConfig::DeviceDefinition::Button::BACK,	{ InputConfig::Assignment::Type::BUTTON, SDLK_ESCAPE } },
	};

	static const std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>> DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS =
	{
		{ InputConfig::DeviceDefinition::Button::A,		{ InputConfig::Assignment::Type::BUTTON, SDLK_a } },
		{ InputConfig::DeviceDefinition::Button::B,		{ InputConfig::Assignment::Type::BUTTON, SDLK_s } },
		{ InputConfig::DeviceDefinition::Button::X,		{ InputConfig::Assignment::Type::BUTTON, SDLK_d } },
		{ InputConfig::DeviceDefinition::Button::X,		{ InputConfig::Assignment::Type::BUTTON, SDLK_q } },
		{ InputConfig::DeviceDefinition::Button::Y,		{ InputConfig::Assignment::Type::BUTTON, SDLK_w } },
		{ InputConfig::DeviceDefinition::Button::BACK,	{ InputConfig::Assignment::Type::BUTTON, SDLK_BACKSPACE } },
		{ InputConfig::DeviceDefinition::Button::L,		{ InputConfig::Assignment::Type::BUTTON, SDLK_e } },
		{ InputConfig::DeviceDefinition::Button::R,		{ InputConfig::Assignment::Type::BUTTON, SDLK_r } },
	};
#endif
}



const std::string InputConfig::DeviceDefinition::BUTTON_NAME[InputConfig::DeviceDefinition::NUM_BUTTONS] = { "Up", "Down", "Left", "Right", "A", "B", "X", "Y", "Start", "Back", "L", "R" };


void InputConfig::Assignment::getMappingString(String& outString, DeviceType deviceType) const
{
	switch (deviceType)
	{
		case DeviceType::KEYBOARD:
		{
			outString = AssignmentLookup::getIdentifierByKey(mIndex);
			if (outString.empty())
			{
				outString << "Key" << mIndex;
			}
			break;
		}

		case DeviceType::GAMEPAD:
		{
			switch (mType)
			{
				case Type::AXIS:
				{
					outString.clear() << "Axis" << mIndex;
					break;
				}
				case Type::BUTTON:
				{
					outString.clear() << "Button" << mIndex;
					break;
				}
				case Type::POV:
				{
					outString.clear() << "Pov" << rmx::log2(mIndex & 0xff);
					break;
				}
			}
			break;
		}
	}
}

bool InputConfig::Assignment::setFromMappingString(Assignment& output, const String& mappingString, DeviceType deviceType)
{
	switch (deviceType)
	{
		case DeviceType::KEYBOARD:
		{
			const uint32 key = AssignmentLookup::getKeyByIdentifier(mappingString);
			if (key != 0)
			{
				output = Assignment(Type::BUTTON, key);
				return true;
			}
			else if (mappingString.startsWith("Key"))
			{
				String str = mappingString.getSubString(3, mappingString.length() - 3);
				output = Assignment(Type::BUTTON, str.parseInt());
				return true;
			}
			break;
		}

		case DeviceType::GAMEPAD:
		{
			if (mappingString.startsWith("Axis"))
			{
				String str = mappingString.getSubString(4, mappingString.length() - 4);
				output = Assignment(Type::AXIS, str.parseInt());
				return true;
			}
			else if (mappingString.startsWith("Button"))
			{
				String str = mappingString.getSubString(6, mappingString.length() - 6);
				output = Assignment(Type::BUTTON, str.parseInt());
				return true;
			}
			else if (mappingString.startsWith("Pov"))
			{
				String str = mappingString.getSubString(3, mappingString.length() - 3);
				output = Assignment(Type::POV, (1 << str.parseInt()));
				return true;
			}
			break;
		}
	}
	return false;
}



void InputConfig::setupDefaultDeviceDefinitions(std::vector<DeviceDefinition>& outDeviceDefinitions)
{
	outDeviceDefinitions.clear();
	{
		DeviceDefinition& deviceDefinition = vectorAdd(outDeviceDefinitions);
		deviceDefinition.mDeviceType = DeviceType::KEYBOARD;
		deviceDefinition.mIdentifier = "Keyboard1";
		setupDefaultKeyboardMappings(deviceDefinition, 0);
	}
	{
		DeviceDefinition& deviceDefinition = vectorAdd(outDeviceDefinitions);
		deviceDefinition.mDeviceType = DeviceType::KEYBOARD;
		deviceDefinition.mIdentifier = "Keyboard2";
		setupDefaultKeyboardMappings(deviceDefinition, 1);
	}

#if defined(PLATFORM_PS3)
	{
		DeviceDefinition& deviceDefinition = vectorAdd(outDeviceDefinitions);
		deviceDefinition.mDeviceType = DeviceType::GAMEPAD;
		deviceDefinition.mIdentifier = "PLAYSTATION(R)3 Controller";
		deviceDefinition.mDeviceNames[rmx::getMurmur2_64("playstation(r)3 controller")] = "PLAYSTATION(R)3 Controller";

		// Map buttons
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::UP].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_DPAD_UP));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::DOWN].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_DPAD_DOWN));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::LEFT].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_DPAD_LEFT));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::RIGHT].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_DPAD_RIGHT));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::A].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_A));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::B].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_B));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::X].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_X));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::Y].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_Y));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::START].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_START));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::BACK].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_BACK));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::L].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_LEFTSHOULDER));
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::R].mAssignments.push_back(Assignment(Assignment::Type::BUTTON, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER));

		// Axes (Left stick)
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::UP].mAssignments.push_back(Assignment(Assignment::Type::AXIS, 1 * 2)); // LY-
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::DOWN].mAssignments.push_back(Assignment(Assignment::Type::AXIS, 1 * 2 + 1)); // LY+
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::LEFT].mAssignments.push_back(Assignment(Assignment::Type::AXIS, 0 * 2)); // LX-
		deviceDefinition.mMappings[(size_t)DeviceDefinition::Button::RIGHT].mAssignments.push_back(Assignment(Assignment::Type::AXIS, 0 * 2 + 1)); // LX+
	}
#endif
}

void InputConfig::setupDefaultKeyboardMappings(DeviceDefinition& outDeviceDefinition, int keyboardIndex)
{
#if defined(PLATFORM_PS3)
	setupAssignments();
#endif
	ControlMapping* mappings = outDeviceDefinition.mMappings;
	for (size_t k = 0; k < (size_t)DeviceDefinition::NUM_BUTTONS; ++k)
	{
		mappings[k].mAssignments.clear();
	}

	if (keyboardIndex == 0)
	{
		// Setup fixed and modifiable assignments for keyboard 1
		for (const auto& pair : DEFAULT_KB1_FIXED_ASSIGNMENTS)
		{
			mappings[(size_t)pair.first].mAssignments.push_back(pair.second);
		}
		for (size_t k = 0; k < (size_t)DeviceDefinition::NUM_BUTTONS; ++k)
		{
			mappings[k].mNumFixedAssignments = mappings[k].mAssignments.size();
		}
		for (const auto& pair : DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS)
		{
			mappings[(size_t)pair.first].mAssignments.push_back(pair.second);
		}
	}
	else
	{
		// Leave keyboard 2 empty
	}
}

void InputConfig::clearAssignments(DeviceDefinition& deviceDefinition, size_t buttonIndex)
{
	// Remove all assignments, except for the fixed ones at the start
	ControlMapping& mapping = getMapping(deviceDefinition, buttonIndex);
	mapping.mAssignments.resize(mapping.mNumFixedAssignments);
}

void InputConfig::addAssignment(DeviceDefinition& deviceDefinition, size_t buttonIndex, const Assignment& newAssignment, bool removeDuplicates)
{
	ControlMapping& mapping = getMapping(deviceDefinition, buttonIndex);

	// Check for duplicates in same button
	{
		std::vector<Assignment>& buttonMappings = mapping.mAssignments;
		if (std::count(buttonMappings.begin(), buttonMappings.end(), newAssignment) != 0)
		{
			// Already added
			return;
		}
	}

	// Check for duplicates in other buttons
	bool canBeAdded = true;
	if (removeDuplicates)
	{
		for (size_t k = 0; k < (size_t)DeviceDefinition::NUM_BUTTONS; ++k)
		{
			if (k != buttonIndex)
			{
				std::vector<Assignment>& buttonMappings = deviceDefinition.mMappings[k].mAssignments;
				for (size_t j = 0; j < buttonMappings.size(); ++j)
				{
					if (buttonMappings[j] == newAssignment)
					{
						// Remove this duplicate if possible, or don't if it's a fixed assignment
						if (j < deviceDefinition.mMappings[k].mNumFixedAssignments)
						{
							// Existing assignment can't be replaced
							canBeAdded = false;
						}
						else
						{
							// Remove existing assignment
							buttonMappings.erase(buttonMappings.begin() + j);
						}
						break;
					}
				}
			}
		}
	}

	if (canBeAdded)
	{
		// Add assignment
		mapping.mAssignments.push_back(newAssignment);
	}
}

void InputConfig::setAssignments(DeviceDefinition& deviceDefinition, size_t buttonIndex, const std::vector<Assignment>& assignments, bool removeDuplicates)
{
	clearAssignments(deviceDefinition, buttonIndex);
	for (const Assignment& newAssignment : assignments)
	{
		addAssignment(deviceDefinition, buttonIndex, newAssignment, removeDuplicates);
	}
}

InputConfig::ControlMapping& InputConfig::getMapping(DeviceDefinition& deviceDefinition, size_t buttonIndex)
{
	RMX_ASSERT(buttonIndex < (size_t)DeviceDefinition::NUM_BUTTONS, "Invalid button index " << buttonIndex);
	return deviceDefinition.mMappings[buttonIndex];
}
