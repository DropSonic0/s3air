/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/application/input/InputManager.h"
#include "oxygen/application/modding/ModManager.h"
#include "oxygen/application/overlays/TouchControlsOverlay.h"
#include "oxygen/application/Configuration.h"
#include "oxygen/menu/imgui/ImGuiIntegration.h"
#include "oxygen/helper/Logging.h"
#include "oxygen/rendering/utils/RenderUtils.h"
#include "oxygen/simulation/LogDisplay.h"


namespace
{

	const char* getJoystickName(SDL_Joystick* joystick)
	{
		if (nullptr == joystick)
			return "Unnamed controller";

		const char* name = SDL_JoystickName(joystick);
		return (nullptr == name) ? "Unnamed controller" : name;
	}

	const char* getGameControllerName(SDL_GameController* gameController)
	{
		if (nullptr == gameController)
			return "";		// Empty string by intent, so this case can easily be checked afterwards

		const char* name = SDL_GameControllerName(gameController);
		return (nullptr == name) ? "Unnamed controller" : name;
	}

	bool isBlacklistedName(const std::string& controllerOrJoystickName)
	{
		static std::vector<uint64> blacklistedHashes;
		if (blacklistedHashes.empty())
		{
#if defined(__CELLOS_LV2__) || defined(__SNC__)
			std::vector<std::string> blacklist;
			blacklist.push_back("virtual-search");
			blacklist.push_back("IPControl_UPnP_RemoteService");
			blacklist.push_back("shield-ask-remote");
			blacklist.push_back("uinput-fpc");
#else
			const std::vector<std::string> blacklist =
			{
				"virtual-search", "IPControl_UPnP_RemoteService", "shield-ask-remote",	// Dummy controllers that Nvidia Shield seems to create
				"uinput-fpc"															// Some other device
			};
#endif
			for (const std::string& str : blacklist)
			{
				blacklistedHashes.push_back(rmx::getMurmur2_64(str));
			}
		}

		const uint64 nameHash = rmx::getMurmur2_64(controllerOrJoystickName);
		for (uint64 blacklistedHash : blacklistedHashes)
		{
			if (blacklistedHash == nameHash)
				return true;
		}
		return false;
	}

	void collectControls(InputManager::ControllerScheme& controller, std::vector<InputManager::Control*>& outControls)
	{
		// This must match the "InputDeviceDefinition::Button" enum
		outControls.push_back(&controller.Up);
		outControls.push_back(&controller.Down);
		outControls.push_back(&controller.Left);
		outControls.push_back(&controller.Right);
		outControls.push_back(&controller.A);
		outControls.push_back(&controller.B);
		outControls.push_back(&controller.X);
		outControls.push_back(&controller.Y);
		outControls.push_back(&controller.Start);
		outControls.push_back(&controller.Back);
		outControls.push_back(&controller.L);
		outControls.push_back(&controller.R);
	}

	bool getControlAssignmentBySDLBinding(InputConfig::Assignment& output, const SDL_GameControllerButtonBind& binding, int axisDirection)
	{
		switch (binding.bindType)
		{
			case SDL_CONTROLLER_BINDTYPE_NONE:
				return false;

			case SDL_CONTROLLER_BINDTYPE_AXIS:
				output.mType = InputConfig::Assignment::Type::AXIS;
				output.mIndex = binding.value.axis * 2 + axisDirection;
				return true;

			case SDL_CONTROLLER_BINDTYPE_BUTTON:
				output.mType = InputConfig::Assignment::Type::BUTTON;
				output.mIndex = binding.value.button;
				return true;

			case SDL_CONTROLLER_BINDTYPE_HAT:
				output.mType = InputConfig::Assignment::Type::POV;
				output.mIndex = (binding.value.hat.hat * 0x100) + binding.value.hat.hat_mask;
				return true;
		}
		return false;
	}

	void setupRealDeviceInputMapping(InputManager::RealDevice& device, const InputConfig::DeviceDefinition& inputDeviceDefinition)
	{
		device.mControlMappings.resize(InputConfig::DeviceDefinition::NUM_BUTTONS);
		for (size_t controlIndex = 0; controlIndex < device.mControlMappings.size(); ++controlIndex)
		{
			device.mControlMappings[controlIndex] = inputDeviceDefinition.mMappings[controlIndex];
		}
	}

	void setupRealDeviceInputMapping(InputManager::RealDevice& device, SDL_GameController& gameController)
	{
		using Button = InputConfig::DeviceDefinition::Button;
		std::vector<SDL_GameControllerButtonBind> bindings[InputConfig::DeviceDefinition::NUM_BUTTONS];

		bindings[(size_t)Button::UP]   .push_back(SDL_GameControllerGetBindForAxis  (&gameController, SDL_CONTROLLER_AXIS_LEFTY));
		bindings[(size_t)Button::UP]   .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_DPAD_UP));
		bindings[(size_t)Button::DOWN] .push_back(SDL_GameControllerGetBindForAxis  (&gameController, SDL_CONTROLLER_AXIS_LEFTY));
		bindings[(size_t)Button::DOWN] .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN));
		bindings[(size_t)Button::LEFT] .push_back(SDL_GameControllerGetBindForAxis  (&gameController, SDL_CONTROLLER_AXIS_LEFTX));
		bindings[(size_t)Button::LEFT] .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT));
		bindings[(size_t)Button::RIGHT].push_back(SDL_GameControllerGetBindForAxis  (&gameController, SDL_CONTROLLER_AXIS_LEFTX));
		bindings[(size_t)Button::RIGHT].push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

		bindings[(size_t)Button::A]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_A));
		bindings[(size_t)Button::B]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_B));
		bindings[(size_t)Button::X]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_X));
		bindings[(size_t)Button::Y]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_Y));
		bindings[(size_t)Button::START].push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_START));
		bindings[(size_t)Button::START].push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_GUIDE));
		bindings[(size_t)Button::BACK] .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_BACK));
		bindings[(size_t)Button::L]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER));
		bindings[(size_t)Button::R]    .push_back(SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER));

		device.mControlMappings.resize(InputConfig::DeviceDefinition::NUM_BUTTONS);
		for (size_t controlIndex = 0; controlIndex < device.mControlMappings.size(); ++controlIndex)
		{
			std::vector<InputConfig::Assignment>& assignments = device.mControlMappings[controlIndex].mAssignments;
			for (const SDL_GameControllerButtonBind& binding : bindings[controlIndex])
			{
				InputConfig::Assignment assignment;
				if (getControlAssignmentBySDLBinding(assignment, binding, controlIndex % 2))
				{
					assignments.push_back(assignment);
				}
			}
		}
	}

	void processGamepadInputMapping(InputConfig::DeviceDefinition& inputDeviceDefinition, SDL_GameController& gameController)
	{
		// Special handling for L/R buttons (which got added later than the rest): Manually add the bindings, if none were set before
		if (inputDeviceDefinition.mMappings[(size_t)InputConfig::DeviceDefinition::Button::L].mAssignments.empty() &&
			inputDeviceDefinition.mMappings[(size_t)InputConfig::DeviceDefinition::Button::R].mAssignments.empty())
		{
			InputConfig::Assignment assignmentL;
			InputConfig::Assignment assignmentR;
			getControlAssignmentBySDLBinding(assignmentL, SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER), 0);
			getControlAssignmentBySDLBinding(assignmentR, SDL_GameControllerGetBindForButton(&gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER), 0);
			inputDeviceDefinition.mMappings[(size_t)InputConfig::DeviceDefinition::Button::L].mAssignments.push_back(assignmentL);
			inputDeviceDefinition.mMappings[(size_t)InputConfig::DeviceDefinition::Button::R].mAssignments.push_back(assignmentR);
		}
	}

	void getMatchingInputDeviceDefinition(const InputManager::RealDevice& device, int index, std::vector<InputConfig::DeviceDefinition>& definitions, InputConfig::DeviceDefinition*& outMatchingDefinition, InputConfig::DeviceDefinition*& outFallbackDefinition)
	{
		String joystickName = getJoystickName(device.mSDLJoystick);
		String controllerName = getGameControllerName(device.mSDLGameController);
		if (controllerName.nonEmpty() && controllerName != joystickName)
		{
			if (index >= 0)
				RMX_LOG_INFO("Controller #" << (index+1) << ": \"" << *joystickName << "\" (alternative name: \"" << *controllerName << "\")");
			controllerName.lowerCase();
		}
		else
		{
			if (index >= 0)
				RMX_LOG_INFO("Controller #" << (index+1) << ": \"" << *joystickName << "\"");
			controllerName.clear();
		}
		joystickName.lowerCase();
		const uint64 nameHashes[2] = { rmx::getMurmur2_64(joystickName), controllerName.empty() ? 0 : rmx::getMurmur2_64(controllerName) };
#if defined(__CELLOS_LV2__) || defined(__SNC__)
		static const uint64 WILDCARD_HASH = rmx::constMurmur2_64("*");
#else
		constexpr uint64 WILDCARD_HASH = rmx::constMurmur2_64("*");
#endif

		for (size_t k = 0; k < definitions.size(); ++k)
		{
			InputConfig::DeviceDefinition& inputDeviceDefinition = definitions[k];
			if (!String(inputDeviceDefinition.mIdentifier).startsWith("Keyboard"))		// Ignore keyboard definitions here
			{
				for (const auto& pair : inputDeviceDefinition.mDeviceNames)
				{
					const uint64 deviceNameHash = pair.first;
					if (deviceNameHash == nameHashes[0] || (nameHashes[1] != 0 && deviceNameHash == nameHashes[1]))
					{
						outMatchingDefinition = &inputDeviceDefinition;
						return;
					}
					else if (deviceNameHash == WILDCARD_HASH)
					{
						outFallbackDefinition = &inputDeviceDefinition;
					}
				}
			}
		}
	}
}




InputFeeder::~InputFeeder()
{
	unregisterAtInputManager();
}

void InputFeeder::registerAtInputManager(InputManager& inputManager)
{
	if (mInputManager != &inputManager)
	{
		unregisterAtInputManager();
		mInputManager = &inputManager;
		mInputManager->registerInputFeeder(*this);
	}
}

void InputFeeder::unregisterAtInputManager()
{
	if (nullptr != mInputManager)
	{
		mInputManager->unregisterInputFeeder(*this);
		mInputManager = nullptr;
	}
}



void InputManager::Control::clearInputs()
{
	mInputs.clear();
}

void InputManager::Control::addInput(RealDevice& device, InputConfig::Assignment::Type type, uint32 index)
{
	ControlInput& input = vectorAdd(mInputs);
	input.mDevice = &device;
	input.mType = type;
	input.mIndex = index;
}

void InputManager::Control::addInput(RealDevice& device, const String& mappingString)
{
	InputConfig::Assignment assignment;
	if (InputConfig::Assignment::setFromMappingString(assignment, mappingString, device.mType))
	{
		addInput(device, assignment.mType, assignment.mIndex);
	}
}



const char* InputManager::RealDevice::getName() const
{
	const char* controllerName = getGameControllerName(mSDLGameController);
	if (nullptr != controllerName && controllerName[0] != 0)
	{
		return controllerName;
	}
	else
	{
		return getJoystickName(mSDLJoystick);
	}
}



const std::string InputManager::KEYBOARD_DEVICE_NAMES[NUM_PLAYERS] = { "Keyboard1", "Keyboard2", "Keyboard3", "Keyboard4" };

InputManager::InputManager()
{
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	mPS3PadsInitialized = false;
	mPS3KbInitialized = false;
	mPS3KeyboardModifiers = 0;
	memset(mPS3KbConnected, 0, sizeof(mPS3KbConnected));
	memset(mPS3KeyboardState, 0, sizeof(mPS3KeyboardState));
	memset(mPS3CachedPadValid, 0, sizeof(mPS3CachedPadValid));
#endif

	mKeyboards.reserve(NUM_PLAYERS);
	mGamepads.reserve(8);	// That's quite a lot, but we have to make sure this is never exceeded by the actual number of devices

	// Register controls
	std::vector<Control*> controls;
	for (int playerIndex = 0; playerIndex < (int)NUM_PLAYERS; ++playerIndex)
	{
		collectControls(mPlayers[playerIndex].mController, mPlayers[playerIndex].mPlayerControls);
		for (Control* control : mPlayers[playerIndex].mPlayerControls)
		{
			control->mPlayerIndex = playerIndex;
			mAllControls.push_back(control);
		}
	}
}

#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
#ifndef CELL_KB_MAX_KEYCODES
#define CELL_KB_MAX_KEYCODES 62
#endif
#ifndef CELL_KB_CODETYPE_RAW
#define CELL_KB_CODETYPE_RAW 0
#endif
#ifndef CELL_KB_RMODE_PACKET
#define CELL_KB_RMODE_PACKET 1
#endif

#ifndef CELL_KB_MKEY_L_SHIFT
#define CELL_KB_MKEY_L_SHIFT  (1<<1)
#endif
#ifndef CELL_KB_MKEY_R_SHIFT
#define CELL_KB_MKEY_R_SHIFT  (1<<5)
#endif
#ifndef CELL_KB_MKEY_L_CTRL
#define CELL_KB_MKEY_L_CTRL   (1<<0)
#endif
#ifndef CELL_KB_MKEY_R_CTRL
#define CELL_KB_MKEY_R_CTRL   (1<<4)
#endif
#ifndef CELL_KB_MKEY_L_ALT
#define CELL_KB_MKEY_L_ALT    (1<<2)
#endif
#ifndef CELL_KB_MKEY_R_ALT
#define CELL_KB_MKEY_R_ALT    (1<<6)
#endif
#ifndef CELL_KB_MKEY_L_GUI
#define CELL_KB_MKEY_L_GUI    (1<<3)
#endif
#ifndef CELL_KB_MKEY_R_GUI
#define CELL_KB_MKEY_R_GUI    (1<<7)
#endif

extern "C" {
	extern int cellKbSetCodeType(uint32_t port_no, uint32_t type);
	extern int cellKbSetReadMode(uint32_t port_no, uint32_t mode);
}

static struct {
	uint32_t mask;
	SDL_Keycode key;
} ps3_modifiers_map[8] = {
	{ CELL_KB_MKEY_L_SHIFT, SDLK_LSHIFT },
	{ CELL_KB_MKEY_R_SHIFT, SDLK_RSHIFT },
	{ CELL_KB_MKEY_L_CTRL, SDLK_LCTRL },
	{ CELL_KB_MKEY_R_CTRL, SDLK_RCTRL },
	{ CELL_KB_MKEY_L_ALT, SDLK_LALT },
	{ CELL_KB_MKEY_R_ALT, SDLK_RALT },
	{ CELL_KB_MKEY_L_GUI, SDLK_LGUI },
	{ CELL_KB_MKEY_R_GUI, SDLK_RGUI }
};

int32_t InputManager::getPS3Axis(const CellPadData* data, int offset)
{
	int val = (int)data->button[offset];
	int centered = val - 128;
	if (centered > -20 && centered < 20)
		return 0;
	return centered * 256;
}

int32_t InputManager::convertPS3HIDToKeycode(uint8_t code)
{
	if (code >= 0x04 && code <= 0x1D) return 'a' + (code - 0x04);
	if (code >= 0x1E && code <= 0x26) return '1' + (code - 0x1E);
	if (code == 0x27) return '0';
	if (code >= 0x3A && code <= 0x43) return SDLK_F1 + (code - 0x3A);
	if (code == 0x44) return SDLK_F11;
	if (code == 0x45) return SDLK_F12;

	switch (code)
	{
		case 0x28: return SDLK_RETURN;
		case 0x29: return SDLK_ESCAPE;
		case 0x2A: return SDLK_BACKSPACE;
		case 0x2B: return SDLK_TAB;
		case 0x2C: return SDLK_SPACE;
		case 0x2D: return SDLK_MINUS;
		case 0x2E: return SDLK_EQUALS;
		case 0x2F: return '[';
		case 0x30: return ']';
		case 0x31: return '\\';
		case 0x33: return ';';
		case 0x34: return '\'';
		case 0x35: return '`';
		case 0x36: return ',';
		case 0x37: return '.';
		case 0x38: return '/';
		case 0x39: return SDLK_CAPSLOCK;
		case 0x47: return SDLK_SCROLLLOCK;
		case 0x48: return SDLK_PAUSE;
		case 0x49: return SDLK_INSERT;
		case 0x4A: return SDLK_HOME;
		case 0x4B: return SDLK_PAGEUP;
		case 0x4C: return SDLK_DELETE;
		case 0x4D: return SDLK_END;
		case 0x4E: return SDLK_PAGEDOWN;
		case 0x4F: return SDLK_RIGHT;
		case 0x50: return SDLK_LEFT;
		case 0x51: return SDLK_DOWN;
		case 0x52: return SDLK_UP;
		case 0x53: return SDLK_NUMLOCKCLEAR;
		case 0x54: return SDLK_KP_DIVIDE;
		case 0x55: return SDLK_KP_MULTIPLY;
		case 0x56: return SDLK_KP_MINUS;
		case 0x57: return SDLK_KP_PLUS;
		case 0x58: return SDLK_KP_ENTER;
		case 0x59: return SDLK_KP_1;
		case 0x5A: return SDLK_KP_2;
		case 0x5B: return SDLK_KP_3;
		case 0x5C: return SDLK_KP_4;
		case 0x5D: return SDLK_KP_5;
		case 0x5E: return SDLK_KP_6;
		case 0x5F: return SDLK_KP_7;
		case 0x60: return SDLK_KP_8;
		case 0x61: return SDLK_KP_9;
		case 0x62: return SDLK_KP_0;
		case 0x63: return SDLK_KP_PERIOD;
		default:   break;
	}
	return 0;
}

void InputManager::initPS3Input()
{
	if (!mPS3PadsInitialized)
	{
		if (cellPadInit(7) == CELL_PAD_OK)
		{
			mPS3PadsInitialized = true;
			memset(mPS3CachedPadData, 0, sizeof(mPS3CachedPadData));
			memset(mPS3CachedPadValid, 0, sizeof(mPS3CachedPadValid));
		}
	}
	if (!mPS3KbInitialized)
	{
		if (cellKbInit(2) == 0)
		{
			mPS3KbInitialized = true;
			memset(mPS3KeyboardState, 0, sizeof(mPS3KeyboardState));
			mPS3KeyboardModifiers = 0;
			memset(mPS3KbConnected, 0, sizeof(mPS3KbConnected));
			memset(mPS3LastKbState, 0, sizeof(mPS3LastKbState));
		}
	}
}

void InputManager::pollPS3Input()
{
	cellSysutilCheckCallback();

	if (mPS3PadsInitialized)
	{
		for (int port = 0; port < 7; port++)
		{
			CellPadData data;
			if (cellPadGetData(port, &data) == CELL_PAD_OK && data.len > 0)
			{
				mPS3CachedPadData[port] = data;
				mPS3CachedPadValid[port] = true;
			}
			else
			{
				mPS3CachedPadValid[port] = false;
			}
		}
	}

	if (mPS3KbInitialized)
	{
		CellKbInfo kbInfo;
		if (cellKbGetInfo(&kbInfo) == 0)
		{
			uint8_t currentHeld[256];
			memset(currentHeld, 0, sizeof(currentHeld));
			uint32_t currentModifiers = 0;

			for (int i = 0; i < 2; i++)
			{
				if (i < (int)kbInfo.max_connect && kbInfo.status[i] != 0)
				{
					if (!mPS3KbConnected[i])
					{
						cellKbSetCodeType(i, CELL_KB_CODETYPE_RAW);
						cellKbSetReadMode(i, CELL_KB_RMODE_PACKET);
						mPS3KbConnected[i] = 1;
						memset(&mPS3LastKbState[i], 0, sizeof(CellKbData));
					}

					int safety = 0;
					CellKbData kbData;
					while (safety < 64)
					{
						kbData.len = 0xFFFFFFFF;
						kbData.mkey = 0xFFFFFFFF;
						int readRes = cellKbRead(i, &kbData);
						if (readRes == CELL_KB_ERROR_NO_DEVICE || readRes == CELL_KB_ERROR_UNINITIALIZED)
						{
							memset(&mPS3LastKbState[i], 0, sizeof(CellKbData));
							mPS3KbConnected[i] = 0;
							break;
						}
						if (readRes != 0 || kbData.len == 0xFFFFFFFF || kbData.mkey == 0xFFFFFFFF)
						{
							break;
						}
						mPS3LastKbState[i] = kbData;
						safety++;
					}
				}
				else
				{
					if (mPS3KbConnected[i])
					{
						memset(&mPS3LastKbState[i], 0, sizeof(CellKbData));
						mPS3KbConnected[i] = 0;
					}
				}

				if (mPS3KbConnected[i])
				{
					int len = mPS3LastKbState[i].len;
					if (len > CELL_KB_MAX_KEYCODES) len = CELL_KB_MAX_KEYCODES;
					for (int k = 0; k < len; k++)
					{
						uint8_t code = mPS3LastKbState[i].keycode[k] & 0xFF;
						if (code > 0) currentHeld[code] = 1;
					}
					if (mPS3LastKbState[i].mkey != 0xFFFFFFFF)
					{
						currentModifiers |= mPS3LastKbState[i].mkey;
					}
				}
			}

			for (int i = 0; i < 256; i++)
			{
				if (currentHeld[i] && !mPS3KeyboardState[i])
				{
					SDL_Keycode sym = convertPS3HIDToKeycode(i);
					if (sym != 0)
					{
						mOneFrameKeyboardInputs.insert(sym);
						mHasKeyboard = true;
					}
				}
				mPS3KeyboardState[i] = currentHeld[i];
			}

			for (int m = 0; m < 8; m++)
			{
				uint8_t newState = (currentModifiers & ps3_modifiers_map[m].mask) ? 1 : 0;
				uint8_t oldState = (mPS3KeyboardModifiers & ps3_modifiers_map[m].mask) ? 1 : 0;
				if (newState && !oldState)
				{
					mOneFrameKeyboardInputs.insert(ps3_modifiers_map[m].key);
					mHasKeyboard = true;
				}
			}
			mPS3KeyboardModifiers = currentModifiers;
		}
	}
}
#endif

void InputManager::startup()
{
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	initPS3Input();
#endif
	// Initialize gamepad
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	rescanRealDevices();
}

void InputManager::enableTouchInput(bool enable)
{
	mTouchInputEnabled = enable;
}

void InputManager::updateInput(float timeElapsed)
{
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	pollPS3Input();
#endif

#if 0
	if (!mGamepads.empty())
	{
		String str;
		SDL_Joystick* joystick = mGamepads[0].mSDLJoystick;
		for (int i = 0; i < 20; ++i)
			str << SDL_JoystickGetButton(joystick, i) << " ";
		str << "-- ";
		for (int i = 0; i < 8; ++i)
			str << SDL_JoystickGetAxis(joystick, i) << " ";
		str << "-- ";
		for (int i = 0; i < 12; ++i)
			str << SDL_JoystickGetHat(joystick, i) << " ";
		LogDisplay::instance().setLogDisplay(str);
	}
#endif

	mAnythingPressed = false;

	// Update touches
	mActiveTouches.clear();
	if (mTouchInputEnabled && !FTX::System->wasEventConsumed())
	{
		const int touchDevices = SDL_GetNumTouchDevices();
		for (int k = 0; k < touchDevices; ++k)
		{
			const SDL_TouchID touchId = SDL_GetTouchDevice(k);
			const int numFingers = SDL_GetNumTouchFingers(touchId);
			for (int i = 0; i < numFingers; ++i)
			{
				const SDL_Finger* finger = SDL_GetTouchFinger(touchId, i);
				if (nullptr != finger)
				{
					vectorAdd(mActiveTouches).mPosition.set(finger->x, finger->y);
				}
			}
		}

		// Also consider left mouse click
		if (FTX::mouseState(rmx::MouseButton::Left))
		{
			vectorAdd(mActiveTouches).mPosition = Vec2f(FTX::mousePos()) / Vec2f(FTX::screenSize());
		}

		if (!mActiveTouches.empty())
		{
			mLastInputType = InputType::TOUCH;
			mAnythingPressed = true;
		}
	}

	// Update controls
	{
		// Update all controls internally (i.e. the part not processed by input feeders)
		for (Control* control : mAllControls)
		{
			control->mPrevState = control->mState;
			control->mState = isPressed(*control);
		}

		// Update input feeders
		for (InputFeeder* inputFeeder : mInputFeeders)
		{
			inputFeeder->updateControls();
		}

		// Finalize controls
		for (Control* control : mAllControls)
		{
			control->mChange = (control->mState != control->mPrevState);

			// Update repeat
			control->mRepeat = false;
			if (control->mState)
			{
				if (control->mChange)
				{
					control->mRepeatTimeout = 0.4f;
				}
				else
				{
					control->mRepeatTimeout -= timeElapsed;
					if (control->mRepeatTimeout <= 0.0f)
					{
						control->mRepeat = true;
						control->mRepeatTimeout = std::max(control->mRepeatTimeout + 0.1f, 0.04f);
					}
				}
				mAnythingPressed = true;
			}
		}
	}

	// Reset one-frame inputs
	mOneFrameKeyboardInputs.clear();

	// Update touch input mode specific behavior
	if (mTouchInputMode == TouchInputMode::FULLSCREEN_START && mWaitingForSingleInput != WaitInputState::NONE)
	{
		switch (mWaitingForSingleInput)
		{
			case WaitInputState::WAIT_FOR_RELEASE:
			{
				if (!mAnythingPressed)
				{
					mWaitingForSingleInput = WaitInputState::WAIT_FOR_PRESS;
				}
				mAnythingPressed = false;
				break;
			}

			case WaitInputState::WAIT_FOR_PRESS:
			{
				if (mAnythingPressed)
				{
					// Inject a Start button press
					mPlayers[0].mController.Start.mState = true;
					mPlayers[0].mController.Start.mChange = true;

					// Leave this touch input mode, and by default show controls now (this may be overwritten later in the frame again)
					setTouchInputMode(TouchInputMode::NORMAL_CONTROLS);
				}
				break;
			}

			default:
				break;
		}
	}

	// Update controller rumble
	const uint32 currentTicks = SDL_GetTicks();
	for (int playerIndex = 0; playerIndex < (int)NUM_PLAYERS; ++playerIndex)
	{
		// Check if rumble intensity has changed - or if the player switched to a different input device
		Player& player = mPlayers[playerIndex];
		if (player.mLastInputDevice != player.mRumblingDevice && nullptr != player.mRumblingDevice)
		{
			stopControllerRumbleForDevice(*player.mRumblingDevice);
			player.mRumblingDevice = player.mLastInputDevice;
			reapplyControllerRumble(playerIndex);
		}
		else if (player.mRumbleEffectQueue.removeExpiredEffects(currentTicks))
		{
			reapplyControllerRumble(playerIndex);
		}
	}
}

void InputManager::injectSDLInputEvent(const SDL_Event& ev)
{
	switch (ev.type)
	{
		case SDL_KEYDOWN:
		{
			if (ev.key.state == SDL_PRESSED)	// This check may be unnecessary
			{
				// Add as one-frame input
				//  -> This is done so that very short key pressed get registered for one frame even if there is no "updateInput" call between key down and key up
				mOneFrameKeyboardInputs.insert(ev.key.keysym.sym);
				mHasKeyboard = true;
			}
			break;
		}
	}
}

const InputManager::ControllerScheme& InputManager::getController(size_t index) const
{
	RMX_ASSERT(index < NUM_PLAYERS, "Invalid index");
	return mPlayers[index].mController;
}

InputManager::ControllerScheme& InputManager::accessController(size_t index)
{
	RMX_ASSERT(index < NUM_PLAYERS, "Invalid index");
	return mPlayers[index].mController;
}

void InputManager::getPressedGamepadInputs(std::vector<InputConfig::Assignment>& outInputs, const RealDevice& device)
{
	outInputs.clear();
	if (device.mType != InputConfig::DeviceType::GAMEPAD)
		return;

	for (int k = 0; k < SDL_JoystickNumButtons(device.mSDLJoystick); ++k)
	{
		if (SDL_JoystickGetButton(device.mSDLJoystick, k) != 0)
		{
			outInputs.push_back(InputConfig::Assignment(InputConfig::Assignment::Type::BUTTON, k));
		}
	}

	for (int k = 0; k < SDL_JoystickNumAxes(device.mSDLJoystick); ++k)
	{
		const int16 value = SDL_JoystickGetAxis(device.mSDLJoystick, k);
		if (value < -0x6000)
		{
			outInputs.push_back(InputConfig::Assignment(InputConfig::Assignment::Type::AXIS, k*2));
		}
		else if (value > 0x6000)
		{
			outInputs.push_back(InputConfig::Assignment(InputConfig::Assignment::Type::AXIS, k*2+1));
		}
	}

	for (int k = 0; k < SDL_JoystickNumHats(device.mSDLJoystick); ++k)
	{
		uint8 value = SDL_JoystickGetHat(device.mSDLJoystick, k);
		if (value != 0)
		{
			for (int bit = 1; bit < 0x100; bit *= 2)
			{
				if (value & bit)
				{
					outInputs.push_back(InputConfig::Assignment(InputConfig::Assignment::Type::POV, k * 0x100 + bit));
					break;
				}
			}
		}
	}}

InputManager::RescanResult InputManager::rescanRealDevices()
{
	RescanResult result;
	result.mPreviousGamepadsFound = (uint32)mGamepads.size();
	result.mGamepadsFound = result.mPreviousGamepadsFound;

	// Anything changed at all?
	const int joysticks = SDL_NumJoysticks();
	if (joysticks == mLastCheckJoysticks && !mKeyboards.empty())
		return result;

	mLastCheckJoysticks = joysticks;
	Configuration& config = Configuration::instance();
	++mGamepadsChangeCounter;

	if (mKeyboards.empty())
	{
		// First-time setup for keyboards
		//  -> Though only one physical keyboard is supported, these are several "real devices", to allow for multiple players using one keyboard together
		for (size_t i = 0; i < NUM_PLAYERS; ++i)
		{
			RealDevice& device = vectorAdd(mKeyboards);
			device.mType = InputConfig::DeviceType::KEYBOARD;
			device.mSDLJoystick = nullptr;
			device.mSDLGameController = nullptr;

			using Button = InputConfig::DeviceDefinition::Button;
			const std::string& key = KEYBOARD_DEVICE_NAMES[i];
			InputConfig::DeviceDefinition* inputDeviceDefinition = getInputDeviceDefinitionByIdentifier(key);
			if (nullptr == inputDeviceDefinition)
			{
				// Fallback in case something went wrong in config for whatever reason...
				RMX_ASSERT(false, "This shouldn't happen for keyboards");

				inputDeviceDefinition = &vectorAdd(config.mInputDeviceDefinitions);
				inputDeviceDefinition->mIdentifier = key;
				inputDeviceDefinition->mDeviceType = InputConfig::DeviceType::KEYBOARD;
				InputConfig::setupDefaultKeyboardMappings(*inputDeviceDefinition, (int)i);
			}

			::setupRealDeviceInputMapping(device, *inputDeviceDefinition);
		}
	}

	for (RealDevice& gamepad : mGamepads)
	{
		gamepad.mDirty = true;
	}
	for (int i = 0; i < joysticks; ++i)
	{
		// Respect the fixed limit of gamepads
		if (mGamepads.size() >= mGamepads.capacity())
			break;

		// Is this gamepad already in our list?
		SDL_Joystick* joystick = SDL_JoystickOpen(i);
		const int32 joystickInstanceId = SDL_JoystickInstanceID(joystick);
		{
			RealDevice* existingGamepad = findGamepadBySDLJoystickInstanceId(joystickInstanceId);
			if (nullptr != existingGamepad)
			{
				// Mark as still existing, and go to the next
				existingGamepad->mDirty = false;
				continue;
			}
		}

		// It's a newly connected (or maybe reconnected) device
		SDL_GameController* controller = SDL_GameControllerOpen(i);
		const std::string joystickName = getJoystickName(joystick);
		const std::string controllerName = getGameControllerName(controller);

		// Skip it if it's blacklisted
		if (isBlacklistedName(joystickName) || isBlacklistedName(controllerName))
			continue;

		RealDevice& device = vectorAdd(mGamepads);
		device.mType = InputConfig::DeviceType::GAMEPAD;
		device.mSDLJoystick = joystick;
		device.mSDLGameController = controller;
		device.mSDLJoystickInstanceId = joystickInstanceId;
	#if SDL_VERSION_ATLEAST(2, 0, 18)
		device.mSupportsRumble = SDL_JoystickHasRumble(joystick);
	#endif

		// Try to find a matching device definition in configuration
		InputConfig::DeviceDefinition* matchingInputDeviceDefinition = nullptr;
		InputConfig::DeviceDefinition* fallbackInputDeviceDefinition = nullptr;
		::getMatchingInputDeviceDefinition(device, (uint32)i, config.mInputDeviceDefinitions, matchingInputDeviceDefinition, fallbackInputDeviceDefinition);

		if (nullptr != matchingInputDeviceDefinition)
		{
			// Use device definition from config
			if (nullptr != device.mSDLGameController)
				::processGamepadInputMapping(*matchingInputDeviceDefinition, *device.mSDLGameController);
			::setupRealDeviceInputMapping(device, *matchingInputDeviceDefinition);
		}
		else if (nullptr != device.mSDLGameController)
		{
			// Use SDL2 game controller lookup
			::setupRealDeviceInputMapping(device, *device.mSDLGameController);
		}
		else if (nullptr != fallbackInputDeviceDefinition)
		{
			// Use fallback from config
			::setupRealDeviceInputMapping(device, *fallbackInputDeviceDefinition);
		}
		else
		{
			continue;
		}

		// Log input mapping as JSON
		{
			RMX_LOG_INFO("{");
			for (size_t controlIndex = 0; controlIndex < device.mControlMappings.size(); ++controlIndex)
			{
				String line = String("\t\"") + InputConfig::DeviceDefinition::BUTTON_NAME[controlIndex] + "\": [ ";
				const std::vector<InputConfig::Assignment>& assignments = device.mControlMappings[controlIndex].mAssignments;
				bool first = true;
				for (const InputConfig::Assignment& assignment : assignments)
				{
					if (first)
						first = false;
					else
						line << ", ";

					switch (assignment.mType)
					{
						case InputConfig::Assignment::Type::AXIS:
						{
							line << "\"Axis" << assignment.mIndex << '"';
							break;
						}
						case InputConfig::Assignment::Type::BUTTON:
						{
							line << "\"Button" << assignment.mIndex << '"';
							break;
						}
						case InputConfig::Assignment::Type::POV:
						{
							line << "\"Pov" << rmx::log2(assignment.mIndex & 0xff) << '"';
							break;
						}
					}
				}
				line << " ]";
				if (controlIndex < device.mControlMappings.size() - 1)
					line << ",";
				RMX_LOG_INFO(*line);
			}
			RMX_LOG_INFO("}");
			RMX_LOG_INFO("");
		}

		// Add to device definitions
		if (nullptr == matchingInputDeviceDefinition)
		{
			InputConfig::DeviceDefinition& inputDeviceDefinition = vectorAdd(config.mInputDeviceDefinitions);
			inputDeviceDefinition.mIdentifier = controllerName.empty() ? joystickName : controllerName;
			inputDeviceDefinition.mDeviceType = InputConfig::DeviceType::GAMEPAD;

			inputDeviceDefinition.mDeviceNames[rmx::getMurmur2_64(joystickName)] = joystickName;
			if (!controllerName.empty() && controllerName != joystickName)
			{
				inputDeviceDefinition.mDeviceNames[rmx::getMurmur2_64(controllerName)] = controllerName;
			}

			for (size_t controlIndex = 0; controlIndex < device.mControlMappings.size(); ++controlIndex)
			{
				inputDeviceDefinition.mMappings[controlIndex].mAssignments = device.mControlMappings[controlIndex].mAssignments;
			}
		}

		// Check if this is a preferred gamepad for one of the players
		for (int playerIndex = 0; playerIndex < (int)NUM_PLAYERS; ++playerIndex)
		{
			// This is only relevant if the player does not have a preferred gamepad already
			if (mPlayers[playerIndex].mPreferredGamepad.mSDLJoystickInstanceId < 0)
			{
				const std::string& preferredGamepad = Configuration::instance().mPreferredGamepad[playerIndex];
				if (!preferredGamepad.empty() && (preferredGamepad == joystickName || preferredGamepad == controllerName))
				{
					mPlayers[playerIndex].mPreferredGamepad.mSDLJoystickInstanceId = joystickInstanceId;
				}
			}
		}
	}

	// Remove all gamepads still marked as dirty, those got disconnected
	for (size_t i = 0; i < mGamepads.size(); ++i)
	{
		if (mGamepads[i].mDirty)
		{
			mGamepads.erase(mGamepads.begin() + i);
			--i;
		}
	}

	updatePlayerGamepadAssignments();

	result.mGamepadsFound = (uint32)mGamepads.size();
	return result;
}

void InputManager::updatePlayerGamepadAssignments()
{
	// Try to map real devices to players
	RMX_ASSERT(mKeyboards.size() == NUM_PLAYERS, "Wrong number of keyboards");
	std::vector<RealDevice*> devicesByPlayer[NUM_PLAYERS];
	for (size_t i = 0; i < mKeyboards.size(); ++i)
	{
		devicesByPlayer[i].push_back(&mKeyboards[i]);
		mKeyboards[i].mAssignedPlayer = (int)i;
	}

	for (RealDevice& gamepad : mGamepads)
	{
		gamepad.mAssignedPlayer = Configuration::instance().mAutoAssignGamepadPlayerIndex;
	}
	for (int playerIndex = NUM_PLAYERS - 1; playerIndex >= 0; --playerIndex)	// Reverse order to make sure player 1 overwrites player 2
	{
		RealDevice* gamepad = findGamepadBySDLJoystickInstanceId(mPlayers[playerIndex].mPreferredGamepad.mSDLJoystickInstanceId);
		if (nullptr != gamepad)
		{
			gamepad->mAssignedPlayer = playerIndex;
		}
	}
	for (RealDevice& gamepad : mGamepads)
	{
		if (gamepad.mAssignedPlayer >= 0)
		{
			devicesByPlayer[gamepad.mAssignedPlayer].push_back(&gamepad);
		}
	}

	// Re-assign controls for the controller schemes
	for (int playerIndex = 0; playerIndex < (int)NUM_PLAYERS; ++playerIndex)
	{
		std::vector<Control*> controls;
		collectControls(mPlayers[playerIndex].mController, controls);
		RMX_ASSERT(controls.size() == InputConfig::DeviceDefinition::NUM_BUTTONS, "Collect controls did not get all buttons");

		for (size_t controlIndex = 0; controlIndex < controls.size(); ++controlIndex)
		{
			Control& control = *controls[controlIndex];
			control.clearInputs();

			for (RealDevice* device : devicesByPlayer[playerIndex])
			{
				if (!device->mControlMappings.empty())
				{
					std::vector<InputConfig::Assignment>& assignments = device->mControlMappings[controlIndex].mAssignments;
					for (InputConfig::Assignment& assignment : assignments)
					{
						control.addInput(*device, assignment.mType, assignment.mIndex);
					}
				}
			}
		}
	}

#ifdef PLATFORM_ANDROID
	// Explicitly add the Android back button
	mPlayers[0].mController.Back.addInput(mKeyboards[0], InputConfig::Assignment::Type::BUTTON, SDLK_AC_BACK);
#endif
}

const InputManager::RealDevice* InputManager::getGamepadByJoystickInstanceId(int32 joystickInstanceId) const
{
	return const_cast<InputManager*>(this)->findGamepadBySDLJoystickInstanceId(joystickInstanceId);
}

int32 InputManager::getPreferredGamepadByJoystickInstanceId(int playerIndex) const
{
	RMX_ASSERT(playerIndex >= 0 && playerIndex < NUM_PLAYERS, "Invalid player index " << playerIndex);
	return mPlayers[playerIndex].mPreferredGamepad.mSDLJoystickInstanceId;
}

void InputManager::setPreferredGamepad(int playerIndex, const RealDevice* gamepad)
{
	RMX_ASSERT(playerIndex >= 0 && playerIndex < NUM_PLAYERS, "Invalid player index " << playerIndex);
	PreferredGamepad& preferredGamepad = mPlayers[playerIndex].mPreferredGamepad;
	if (nullptr != gamepad)
	{
		preferredGamepad.mSDLJoystickInstanceId = gamepad->mSDLJoystickInstanceId;
		Configuration::instance().mPreferredGamepad[playerIndex] = gamepad->getName();
	}
	else
	{
		preferredGamepad.mSDLJoystickInstanceId = -1;
		Configuration::instance().mPreferredGamepad[playerIndex].clear();
	}
	updatePlayerGamepadAssignments();
}

InputConfig::DeviceDefinition* InputManager::getDeviceDefinition(const RealDevice& device) const
{
	if (device.mType == InputConfig::DeviceType::KEYBOARD)
	{
		size_t keyboardIndex = 0;
		for (size_t k = 1; k < NUM_PLAYERS; ++k)
		{
			if (&device == &mKeyboards[k])
			{
				keyboardIndex = k;
				break;
			}
		}
		const std::string& key = KEYBOARD_DEVICE_NAMES[keyboardIndex];
		return getInputDeviceDefinitionByIdentifier(key);
	}
	else
	{
		InputConfig::DeviceDefinition* matchingInputDeviceDefinition = nullptr;
		InputConfig::DeviceDefinition* fallbackInputDeviceDefinition = nullptr;
		::getMatchingInputDeviceDefinition(device, -1, Configuration::instance().mInputDeviceDefinitions, matchingInputDeviceDefinition, fallbackInputDeviceDefinition);
		return matchingInputDeviceDefinition;
	}
}

const std::vector<InputConfig::Assignment>* InputManager::getControlMapping(const RealDevice& device, InputConfig::DeviceDefinition::Button button) const
{
	const InputConfig::DeviceDefinition* inputDeviceDefinition = getDeviceDefinition(device);
	return (nullptr == inputDeviceDefinition) ? nullptr : &inputDeviceDefinition->mMappings[(size_t)button].mAssignments;
}

void InputManager::redefineControlMapping(const RealDevice& device, InputConfig::DeviceDefinition::Button button, const std::vector<InputConfig::Assignment>& newAssignments)
{
	InputConfig::DeviceDefinition* inputDeviceDefinition = getDeviceDefinition(device);
	if (nullptr != inputDeviceDefinition)
	{
		InputConfig::setAssignments(*inputDeviceDefinition, (size_t)button, newAssignments, true);

		// Now also reload mappings from config and apply them
		::setupRealDeviceInputMapping(const_cast<RealDevice&>(device), *inputDeviceDefinition);
		updatePlayerGamepadAssignments();
		++mMappingsChangeCounter;
	}
}

void InputManager::registerInputFeeder(InputFeeder& inputFeeder)
{
	mInputFeeders.insert(&inputFeeder);
}

void InputManager::unregisterInputFeeder(InputFeeder& inputFeeder)
{
	mInputFeeders.erase(&inputFeeder);
}

void InputManager::setControlState(Control& control, bool pressed)
{
	control.mChange = (pressed != control.mState);
	control.mState = pressed;
}

void InputManager::setTouchInputMode(TouchInputMode mode)
{
	if (mTouchInputMode != mode)
	{
		const bool enableSingleInput = (mode == TouchInputMode::FULLSCREEN_START);
		const bool showOverlay = (mode == TouchInputMode::NORMAL_CONTROLS);
		mWaitingForSingleInput = enableSingleInput ? WaitInputState::WAIT_FOR_RELEASE : WaitInputState::NONE;
		if (TouchControlsOverlay::hasInstance())
		{
			TouchControlsOverlay::instance().setForceHidden(!showOverlay);
		}
		mTouchInputMode = mode;
	}
}

void InputManager::resetControllerRumbleForPlayer(int playerIndex)
{
	if (playerIndex >= 0 && playerIndex < NUM_PLAYERS)
	{
		mPlayers[playerIndex].mRumbleEffectQueue.reset();
		reapplyControllerRumble(playerIndex);
	}
}

void InputManager::setControllerRumbleForPlayer(int playerIndex, float lowFrequencyRumble, float highFrequencyRumble, uint32 milliseconds)
{
	if (playerIndex >= 0 && playerIndex < NUM_PLAYERS)
	{
		const uint32 endTicks = SDL_GetTicks() + milliseconds;
		if (mPlayers[playerIndex].mRumbleEffectQueue.addEffect(lowFrequencyRumble, highFrequencyRumble, endTicks))
		{
			reapplyControllerRumble(playerIndex);
		}
	}
}

void InputManager::setControllerLEDsForPlayer(int playerIndex, const Color& color) const
{
#if SDL_VERSION_ATLEAST(2, 0, 14)
	// TODO: Remove some of these exclusions where possible
	#if !defined(PLATFORM_WEB) && !defined(PLATFORM_SWITCH) && !defined(PLATFORM_VITA) && !(defined(PLATFORM_WINDOWS) && defined(__GNUC__))
		for (size_t i = 0; i < mGamepads.size(); ++i)
		{
			if (mGamepads[i].mAssignedPlayer == playerIndex && nullptr != mGamepads[i].mSDLGameController)
			{
				SDL_GameControllerSetLED(mGamepads[i].mSDLGameController, (uint8)roundToInt(color.r * 255.0f), (uint8)roundToInt(color.g * 255.0f), (uint8)roundToInt(color.b * 255.0f));
			}
		}
	#endif
#endif
}

void InputManager::handleActiveModsChanged()
{
#if defined(__CELLOS_LV2__) || defined(__SNC__)
	static const uint64 FEATURE_NAME_HASH = rmx::constMurmur2_64("Controls_LR");
#else
	constexpr uint64 FEATURE_NAME_HASH = rmx::constMurmur2_64("Controls_LR");
#endif
	mUsingControlsLR = ModManager::instance().anyActiveModUsesFeature(FEATURE_NAME_HASH);

	if (TouchControlsOverlay::hasInstance())
	{
		TouchControlsOverlay::instance().buildTouchControls();
	}
}

InputManager::RealDevice* InputManager::findGamepadBySDLJoystickInstanceId(int32 joystickInstanceId)
{
	for (RealDevice& gamepad : mGamepads)
	{
		if (gamepad.mSDLJoystickInstanceId == joystickInstanceId)
			return &gamepad;
	}
	return nullptr;
}

InputConfig::DeviceDefinition* InputManager::getInputDeviceDefinitionByIdentifier(std::string_view identifier) const
{
	Configuration& config = Configuration::instance();
	for (size_t k = 0; k < config.mInputDeviceDefinitions.size(); ++k)
	{
		if (config.mInputDeviceDefinitions[k].mIdentifier == identifier)
		{
			return &config.mInputDeviceDefinitions[k];
		}
	}
	return nullptr;
}

bool InputManager::isPressed(const Control& control)
{
	// Handle key and button assignments
	for (const ControlInput& input : control.mInputs)
	{
		if (isPressed(input))
		{
			mLastInputType = (input.mDevice->mType == InputConfig::DeviceType::GAMEPAD) ? InputType::GAMEPAD : InputType::KEYBOARD;
			mPlayers[control.mPlayerIndex].mLastInputDevice = input.mDevice;
			return true;
		}
	}
	return false;
}

bool InputManager::isPressed(const ControlInput& input)
{
	if (nullptr == input.mDevice)
		return false;

	switch (input.mDevice->mType)
	{
		case InputConfig::DeviceType::KEYBOARD:
		{
			if (FTX::keyState(input.mIndex) || mOneFrameKeyboardInputs.count(input.mIndex) > 0)
			{
				// Ignore key presses while Alt is down
				if (!FTX::keyState(SDLK_LALT) && !FTX::keyState(SDLK_RALT))
				{
					if (!ImGuiIntegration::instance().isCapturingKeyboard())
						return true;
				}
			}
			break;
		}

		case InputConfig::DeviceType::GAMEPAD:
		{
			if (isPressed(input.mDevice->mSDLJoystick, input))
				return true;
			break;
		}
	}
	return false;
}

bool InputManager::isPressed(SDL_Joystick* joystick, const ControlInput& input)
{
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	CellPadData padData;
	int port = (nullptr != input.mDevice) ? input.mDevice->mAssignedPlayer : 0;
	if (port < 0 || port >= 7) port = 0;
	if (cellPadGetData(port, &padData) == CELL_PAD_OK && padData.len > 0)
	{
		uint16_t buttons = ((uint16_t)padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] << 8) | (uint16_t)padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1];
		switch (input.mType)
		{
			case InputConfig::Assignment::Type::AXIS:
			{
				int axisIndex = input.mIndex / 2;
				int axisVal = 0;
				if (axisIndex == 0) axisVal = getPS3Axis(&padData, CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X);
				else if (axisIndex == 1) axisVal = getPS3Axis(&padData, CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y);
				else if (axisIndex == 2) axisVal = getPS3Axis(&padData, CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X);
				else if (axisIndex == 3) axisVal = getPS3Axis(&padData, CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y);

				const float value = (float)axisVal / 32767.0f;
				if ((input.mIndex % 2) == 0)
					return (value < -0.25f);
				else
					return (value > 0.25f);
			}
			case InputConfig::Assignment::Type::BUTTON:
			{
				uint16_t digital1 = (uint16_t)padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1];
				uint16_t digital2 = (uint16_t)padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2];
				switch (input.mIndex)
				{
					case 0: return (digital2 & CELL_PAD_CTRL_CROSS) != 0;    // A / Cross
					case 1: return (digital2 & CELL_PAD_CTRL_CIRCLE) != 0;   // B / Circle
					case 2: return (digital2 & CELL_PAD_CTRL_SQUARE) != 0;   // X / Square
					case 3: return (digital2 & CELL_PAD_CTRL_TRIANGLE) != 0; // Y / Triangle
					case 4: return (digital1 & CELL_PAD_CTRL_START) != 0;    // Start
					case 5: return (digital1 & CELL_PAD_CTRL_SELECT) != 0;   // Back / Select
					case 6: return (digital2 & CELL_PAD_CTRL_L1) != 0;       // L1
					case 7: return (digital2 & CELL_PAD_CTRL_R1) != 0;       // R1
					case 8: return (digital2 & CELL_PAD_CTRL_L2) != 0;       // L2
					case 9: return (digital2 & CELL_PAD_CTRL_R2) != 0;       // R2
					case 10: return (digital1 & CELL_PAD_CTRL_L3) != 0;      // L3
					case 11: return (digital1 & CELL_PAD_CTRL_R3) != 0;      // R3
				}
				return (SDL_JoystickGetButton(joystick, input.mIndex) > 0);
			}
			case InputConfig::Assignment::Type::POV:
			{
				uint8 hatMask = 0;
				if (buttons & CELL_PAD_CTRL_UP) hatMask |= 1;
				if (buttons & CELL_PAD_CTRL_RIGHT) hatMask |= 2;
				if (buttons & CELL_PAD_CTRL_DOWN) hatMask |= 4;
				if (buttons & CELL_PAD_CTRL_LEFT) hatMask |= 8;
				return (hatMask & (input.mIndex & 0xff)) != 0;
			}
		}
	}
#endif

	if (nullptr != joystick)
	{
		switch (input.mType)
		{
			case InputConfig::Assignment::Type::AXIS:
			{
				// Use even number for negative axis direction, odd number for positive axis direction
				const float value = (float)SDL_JoystickGetAxis(joystick, input.mIndex / 2) / 32767.0f;
				if ((input.mIndex % 2) == 0)
				{
					return (value < -0.25f);
				}
				else
				{
					return (value > 0.25f);
				}
			}

			case InputConfig::Assignment::Type::BUTTON:
			{
				return (SDL_JoystickGetButton(joystick, input.mIndex) > 0);
			}

			case InputConfig::Assignment::Type::POV:
			{
				const uint8 hatMask = SDL_JoystickGetHat(joystick, input.mIndex >> 8);
				return (hatMask & input.mIndex) != 0;
			}
		}
	}
	return false;
}

void InputManager::reapplyControllerRumble(int playerIndex)
{
#if SDL_VERSION_ATLEAST(2, 0, 18)
	RealDevice* device = mPlayers[playerIndex].mLastInputDevice;
	if (nullptr != device && device->mType == InputConfig::DeviceType::GAMEPAD && nullptr != device->mSDLJoystick && device->mAssignedPlayer == playerIndex)
	{
		const float intensity = clamp(Configuration::instance().mControllerRumbleIntensity[playerIndex], 0.0f, 1.0f);
		const float lowFreqIntensity = mPlayers[playerIndex].mRumbleEffectQueue.getCurrentLowFreqIntensity();
		const float highFreqIntensity = mPlayers[playerIndex].mRumbleEffectQueue.getCurrentHighFreqIntensity();
		const uint16 lowFrequencyRumbleUint16 = (uint16)roundToInt(lowFreqIntensity * intensity * 65535.0f);
		const uint16 highFrequencyRumbleUint16 = (uint16)roundToInt(highFreqIntensity * intensity * 65535.0f);

		SDL_JoystickRumble(device->mSDLJoystick, lowFrequencyRumbleUint16, highFrequencyRumbleUint16, 0xffff);
		mPlayers[playerIndex].mRumblingDevice = device;
	}
#endif
}

void InputManager::stopControllerRumbleForDevice(RealDevice& device)
{
#if SDL_VERSION_ATLEAST(2, 0, 18)
	if (nullptr != device.mSDLJoystick)
	{
		SDL_JoystickRumble(device.mSDLJoystick, 0, 0, 0);
	}
#endif
}
