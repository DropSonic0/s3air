/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/application/input/InputConfig.h"
#include "oxygen/application/input/InputManager.h"


namespace
{
	struct AssignmentLookup
	{
#if defined(__CELLOS_LV2__) || defined(__SNC__)
		static const std::vector<std::pair<String, uint32>>& getKeysAndIdentifiers()
		{
			static std::vector<std::pair<String, uint32>> list;
			if (list.empty())
			{
				list.push_back(std::make_pair(String("Enter"), (uint32)SDLK_RETURN));
				list.push_back(std::make_pair(String("Esc"), (uint32)SDLK_ESCAPE));
				list.push_back(std::make_pair(String("Backspace"), (uint32)SDLK_BACKSPACE));
				list.push_back(std::make_pair(String("Tab"), (uint32)SDLK_TAB));
				list.push_back(std::make_pair(String("Space"), (uint32)SDLK_SPACE));
				list.push_back(std::make_pair(String("Exclaim"), (uint32)SDLK_EXCLAIM));
				list.push_back(std::make_pair(String("QuoteDbl"), (uint32)SDLK_QUOTEDBL));
				list.push_back(std::make_pair(String("Hash"), (uint32)SDLK_HASH));
				list.push_back(std::make_pair(String("Percent"), (uint32)SDLK_PERCENT));
				list.push_back(std::make_pair(String("Dollar"), (uint32)SDLK_DOLLAR));
				list.push_back(std::make_pair(String("Ampersand"), (uint32)SDLK_AMPERSAND));
				list.push_back(std::make_pair(String("Quote"), (uint32)SDLK_QUOTE));
				list.push_back(std::make_pair(String("LeftParen"), (uint32)SDLK_LEFTPAREN));
				list.push_back(std::make_pair(String("RightParen"), (uint32)SDLK_RIGHTPAREN));
				list.push_back(std::make_pair(String("Asterisk"), (uint32)SDLK_ASTERISK));
				list.push_back(std::make_pair(String("Plus"), (uint32)SDLK_PLUS));
				list.push_back(std::make_pair(String("Comma"), (uint32)SDLK_COMMA));
				list.push_back(std::make_pair(String("Minus"), (uint32)SDLK_MINUS));
				list.push_back(std::make_pair(String("Period"), (uint32)SDLK_PERIOD));
				list.push_back(std::make_pair(String("Slash"), (uint32)SDLK_SLASH));
				list.push_back(std::make_pair(String("0"), (uint32)SDLK_0));
				list.push_back(std::make_pair(String("1"), (uint32)SDLK_1));
				list.push_back(std::make_pair(String("2"), (uint32)SDLK_2));
				list.push_back(std::make_pair(String("3"), (uint32)SDLK_3));
				list.push_back(std::make_pair(String("4"), (uint32)SDLK_4));
				list.push_back(std::make_pair(String("5"), (uint32)SDLK_5));
				list.push_back(std::make_pair(String("6"), (uint32)SDLK_6));
				list.push_back(std::make_pair(String("7"), (uint32)SDLK_7));
				list.push_back(std::make_pair(String("8"), (uint32)SDLK_8));
				list.push_back(std::make_pair(String("9"), (uint32)SDLK_9));
				list.push_back(std::make_pair(String("Colon"), (uint32)SDLK_COLON));
				list.push_back(std::make_pair(String("Semicolon"), (uint32)SDLK_SEMICOLON));
				list.push_back(std::make_pair(String("Less"), (uint32)SDLK_LESS));
				list.push_back(std::make_pair(String("Equals"), (uint32)SDLK_EQUALS));
				list.push_back(std::make_pair(String("Greater"), (uint32)SDLK_GREATER));
				list.push_back(std::make_pair(String("Question"), (uint32)SDLK_QUESTION));
				list.push_back(std::make_pair(String("At"), (uint32)SDLK_AT));
				list.push_back(std::make_pair(String("LeftBracket"), (uint32)SDLK_LEFTBRACKET));
				list.push_back(std::make_pair(String("Backslash"), (uint32)SDLK_BACKSLASH));
				list.push_back(std::make_pair(String("RightBracket"), (uint32)SDLK_RIGHTBRACKET));
				list.push_back(std::make_pair(String("Caret"), (uint32)SDLK_CARET));
				list.push_back(std::make_pair(String("Underscore"), (uint32)SDLK_UNDERSCORE));
				list.push_back(std::make_pair(String("BackQuote"), (uint32)SDLK_BACKQUOTE));
				list.push_back(std::make_pair(String("A"), (uint32)SDLK_a));
				list.push_back(std::make_pair(String("B"), (uint32)SDLK_b));
				list.push_back(std::make_pair(String("C"), (uint32)SDLK_c));
				list.push_back(std::make_pair(String("D"), (uint32)SDLK_d));
				list.push_back(std::make_pair(String("E"), (uint32)SDLK_e));
				list.push_back(std::make_pair(String("F"), (uint32)SDLK_f));
				list.push_back(std::make_pair(String("G"), (uint32)SDLK_g));
				list.push_back(std::make_pair(String("H"), (uint32)SDLK_h));
				list.push_back(std::make_pair(String("I"), (uint32)SDLK_i));
				list.push_back(std::make_pair(String("J"), (uint32)SDLK_j));
				list.push_back(std::make_pair(String("K"), (uint32)SDLK_k));
				list.push_back(std::make_pair(String("L"), (uint32)SDLK_l));
				list.push_back(std::make_pair(String("M"), (uint32)SDLK_m));
				list.push_back(std::make_pair(String("N"), (uint32)SDLK_n));
				list.push_back(std::make_pair(String("O"), (uint32)SDLK_o));
				list.push_back(std::make_pair(String("P"), (uint32)SDLK_p));
				list.push_back(std::make_pair(String("Q"), (uint32)SDLK_q));
				list.push_back(std::make_pair(String("R"), (uint32)SDLK_r));
				list.push_back(std::make_pair(String("S"), (uint32)SDLK_s));
				list.push_back(std::make_pair(String("T"), (uint32)SDLK_t));
				list.push_back(std::make_pair(String("U"), (uint32)SDLK_u));
				list.push_back(std::make_pair(String("V"), (uint32)SDLK_v));
				list.push_back(std::make_pair(String("W"), (uint32)SDLK_w));
				list.push_back(std::make_pair(String("X"), (uint32)SDLK_x));
				list.push_back(std::make_pair(String("Y"), (uint32)SDLK_y));
				list.push_back(std::make_pair(String("Z"), (uint32)SDLK_z));
				list.push_back(std::make_pair(String("CapsLock"), (uint32)SDLK_CAPSLOCK));
				list.push_back(std::make_pair(String("Insert"), (uint32)SDLK_INSERT));
				list.push_back(std::make_pair(String("Home"), (uint32)SDLK_HOME));
				list.push_back(std::make_pair(String("PageUp"), (uint32)SDLK_PAGEUP));
				list.push_back(std::make_pair(String("Delete"), (uint32)SDLK_DELETE));
				list.push_back(std::make_pair(String("End"), (uint32)SDLK_END));
				list.push_back(std::make_pair(String("PageDown"), (uint32)SDLK_PAGEDOWN));
				list.push_back(std::make_pair(String("Up"), (uint32)SDLK_UP));
				list.push_back(std::make_pair(String("Down"), (uint32)SDLK_DOWN));
				list.push_back(std::make_pair(String("Left"), (uint32)SDLK_LEFT));
				list.push_back(std::make_pair(String("Right"), (uint32)SDLK_RIGHT));
				list.push_back(std::make_pair(String("NumpadDivide"), (uint32)SDLK_KP_DIVIDE));
				list.push_back(std::make_pair(String("NumpadMultiply"), (uint32)SDLK_KP_MULTIPLY));
				list.push_back(std::make_pair(String("NumpadMinus"), (uint32)SDLK_KP_MINUS));
				list.push_back(std::make_pair(String("NumpadPlus"), (uint32)SDLK_KP_PLUS));
				list.push_back(std::make_pair(String("NumpadEnter"), (uint32)SDLK_KP_ENTER));
				list.push_back(std::make_pair(String("Numpad1"), (uint32)SDLK_KP_1));
				list.push_back(std::make_pair(String("Numpad2"), (uint32)SDLK_KP_2));
				list.push_back(std::make_pair(String("Numpad3"), (uint32)SDLK_KP_3));
				list.push_back(std::make_pair(String("Numpad4"), (uint32)SDLK_KP_4));
				list.push_back(std::make_pair(String("Numpad5"), (uint32)SDLK_KP_5));
				list.push_back(std::make_pair(String("Numpad6"), (uint32)SDLK_KP_6));
				list.push_back(std::make_pair(String("Numpad7"), (uint32)SDLK_KP_7));
				list.push_back(std::make_pair(String("Numpad8"), (uint32)SDLK_KP_8));
				list.push_back(std::make_pair(String("Numpad9"), (uint32)SDLK_KP_9));
				list.push_back(std::make_pair(String("Numpad0"), (uint32)SDLK_KP_0));
				list.push_back(std::make_pair(String("NumpadPeriod"), (uint32)SDLK_KP_PERIOD));
				list.push_back(std::make_pair(String("LeftShift"), (uint32)SDLK_LSHIFT));
				list.push_back(std::make_pair(String("RightShift"), (uint32)SDLK_RSHIFT));
				list.push_back(std::make_pair(String("LeftCtrl"), (uint32)SDLK_LCTRL));
				list.push_back(std::make_pair(String("RightCtrl"), (uint32)SDLK_RCTRL));
				list.push_back(std::make_pair(String("LeftAlt"), (uint32)SDLK_LALT));
				list.push_back(std::make_pair(String("RightAlt"), (uint32)SDLK_RALT));
			}
			return list;
		}
		static std::map<String, uint32> mKeyByIdentifier;
		static std::map<uint32, String> mIdentifierByKey;
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
			{ "Period",			SDLK_PERIOD },
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
		static inline std::map<String, uint32> mKeyByIdentifier;	// Uses lowercase strings for better comparability
		static inline std::map<uint32, String> mIdentifierByKey;
#endif

		static uint32 getKeyByIdentifier(const String& identifier)
		{
			if (mKeyByIdentifier.empty())
			{
				// Lazy initialization
#if defined(__CELLOS_LV2__) || defined(__SNC__)
				for (const auto& pair : getKeysAndIdentifiers())
#else
				for (const auto& pair : KEYS_AND_IDENTIFIERS)
#endif
				{
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
			if (mIdentifierByKey.empty())
			{
				// Lazy initialization
#if defined(__CELLOS_LV2__) || defined(__SNC__)
				for (const auto& pair : getKeysAndIdentifiers())
#else
				for (const auto& pair : KEYS_AND_IDENTIFIERS)
#endif
				{
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

#if defined(__CELLOS_LV2__) || defined(__SNC__)
	std::map<String, uint32> AssignmentLookup::mKeyByIdentifier;
	std::map<uint32, String> AssignmentLookup::mIdentifierByKey;

	static const std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>>& getFixedAssignments()
	{
		static std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>> list;
		if (list.empty())
		{
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::UP, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_UP)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::DOWN, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_DOWN)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::LEFT, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_LEFT)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::RIGHT, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_RIGHT)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::START, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_RETURN)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::BACK, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_ESCAPE)));
		}
		return list;
	}

	static const std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>>& getModifyableAssignments()
	{
		static std::vector<std::pair<InputConfig::DeviceDefinition::Button, InputConfig::Assignment>> list;
		if (list.empty())
		{
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::A, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_a)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::B, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_s)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::X, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_d)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::X, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_q)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::Y, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_w)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::BACK, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_BACKSPACE)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::L, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_e)));
			list.push_back(std::make_pair(InputConfig::DeviceDefinition::Button::R, InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, SDLK_r)));
		}
		return list;
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
	for (int keyboardIndex = 0; keyboardIndex < InputManager::NUM_PLAYERS; ++keyboardIndex)
	{
		DeviceDefinition& deviceDefinition = vectorAdd(outDeviceDefinitions);
		deviceDefinition.mDeviceType = DeviceType::KEYBOARD;
		deviceDefinition.mIdentifier = InputManager::KEYBOARD_DEVICE_NAMES[keyboardIndex];
		setupDefaultKeyboardMappings(deviceDefinition, keyboardIndex);
	}
}

void InputConfig::setupDefaultKeyboardMappings(DeviceDefinition& outDeviceDefinition, int keyboardIndex)
{
	ControlMapping* mappings = outDeviceDefinition.mMappings;
	for (size_t k = 0; k < (size_t)DeviceDefinition::NUM_BUTTONS; ++k)
	{
		mappings[k].mAssignments.clear();
	}

	if (keyboardIndex == 0)
	{
		// Setup fixed and modifiable assignments for keyboard 1
#if defined(__CELLOS_LV2__) || defined(__SNC__)
		for (const auto& pair : getFixedAssignments())
#else
		for (const auto& pair : DEFAULT_KB1_FIXED_ASSIGNMENTS)
#endif
		{
			mappings[(size_t)pair.first].mAssignments.push_back(pair.second);
		}
		for (size_t k = 0; k < (size_t)DeviceDefinition::NUM_BUTTONS; ++k)
		{
			mappings[k].mNumFixedAssignments = mappings[k].mAssignments.size();
		}
#if defined(__CELLOS_LV2__) || defined(__SNC__)
		for (const auto& pair : getModifyableAssignments())
#else
		for (const auto& pair : DEFAULT_KB1_MODIFYABLE_ASSIGNMENTS)
#endif
		{
			mappings[(size_t)pair.first].mAssignments.push_back(pair.second);
		}
	}
	else
	{
		// Leave other keyboards empty
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
