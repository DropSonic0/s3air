/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/EngineDelegate.h"
#include "sonic3air/ConfigurationImpl.h"
#include "sonic3air/audio/AudioOut.h"
#include "sonic3air/menu/GameApp.h"
#include "sonic3air/menu/SharedResources.h"
#include "sonic3air/version.inc"
#if 0
	#include "sonic3air/generator/ResourceScriptGenerator.h"
#endif

#include "oxygen/application/Application.h"
#include "oxygen/application/GameProfile.h"
#include "oxygen/platform/CrashHandler.h"
#include "oxygen/simulation/CodeExec.h"
#include "oxygen/simulation/EmulatorInterface.h"

#include <lemon/program/Program.h>
#include <lemon/runtime/provider/NativizedOpcodeProvider.h>


namespace lemon
{
	// Forward declaration of the nativized code lookup builder function
	extern void createNativizedCodeLookup(Nativizer::LookupDictionary& dict);
}


const EngineDelegateInterface::AppMetaData& EngineDelegate::getAppMetaData()
{
	if (mAppMetaData.mTitle.empty())
	{
		mAppMetaData.mTitle = "Sonic 3 A.I.R.";
		mAppMetaData.mIconFile = L"data/images/icon.png";
		mAppMetaData.mWindowsIconResource = 101;
		mAppMetaData.mBuildVersionString = BUILD_STRING;
		mAppMetaData.mBuildVersionNumber = BUILD_NUMBER;
	#if defined(PLATFORM_PS3)
		mAppMetaData.mAppDataFolder = L"SONIC3AIR";
	#else
		mAppMetaData.mAppDataFolder = L"Sonic3AIR";
	#endif
	}
	return mAppMetaData;
}

GuiBase& EngineDelegate::createGameApp()
{
	return *new GameApp();
}

AudioOutBase& EngineDelegate::createAudioOut()
{
	return *new AudioOut();
}

bool EngineDelegate::onEnginePreStartup()
{
	CrashHandler::setApplicationInfo(std::string("Sonic 3 A.I.R. v") + BUILD_STRING);
	oxygen::Logging::setAssertBreakCaption(std::string("Sonic 3 A.I.R. - v") + BUILD_STRING);

	// Sanity check if the game is even extracted
	{
		// One of these two files must exist
	#if (defined(PLATFORM_MAC) || defined(PLATFORM_IOS)) && defined(ENDUSER)
		Configuration& config = Configuration::instance();
		const bool check = FTX::FileSystem->exists(config.mGameDataPath + L"/gamedata.bin");
	#elif defined(PLATFORM_PS3)
		RMX_LOG_INFO("Checking for game data...");
		const std::wstring appDataPath = Configuration::instance().mAppDataPath;
		RMX_LOG_INFO("App data path: " << WString(appDataPath).toStdString());

		const std::wstring path1 = L"data/content.json";
		const std::wstring path2 = L"data/gamedata.bin";
		const std::wstring path3 = appDataPath + L"data/content.json";
		const std::wstring path4 = appDataPath + L"data/gamedata.bin";

		const bool check1 = FTX::FileSystem->exists(path1);
		const bool check2 = FTX::FileSystem->exists(path2);
		const bool check3 = FTX::FileSystem->exists(path3);
		const bool check4 = FTX::FileSystem->exists(path4);

		RMX_LOG_INFO("Check 1 (" << WString(path1).toStdString() << "): " << (check1 ? "FOUND" : "NOT FOUND"));
		RMX_LOG_INFO("Check 2 (" << WString(path2).toStdString() << "): " << (check2 ? "FOUND" : "NOT FOUND"));
		RMX_LOG_INFO("Check 3 (" << WString(path3).toStdString() << "): " << (check3 ? "FOUND" : "NOT FOUND"));
		RMX_LOG_INFO("Check 4 (" << WString(path4).toStdString() << "): " << (check4 ? "FOUND" : "NOT FOUND"));

		const bool check = (check1 || check2 || check3 || check4);
		RMX_LOG_INFO("Final check result: " << (check ? "SUCCESS" : "FAILED"));
	#else
		const bool check = (FTX::FileSystem->exists(L"data/content.json") || FTX::FileSystem->exists(L"data/gamedata.bin"));
	#endif
		if (!check)
		{
		#if defined(PLATFORM_WINDOWS)
			RMX_ERROR("Seems like you launched the Sonic3AIR.exe from inside the downloaded ZIP file.\n\nMake sure to first extract the ZIP somewhere like on your desktop, then start the Sonic3AIR.exe in the extracted folder.", );
		#elif defined(PLATFORM_PS3)
			RMX_ERROR("Could not find the game data in the 'data' folder.\n\nMake sure that all required files (including 'gamedata.bin' and the 'data' folder itself) are correctly placed in your game's USRDIR.", );
		#else
			RMX_ERROR("Seems like you launched the Sonic3AIR executable from inside the downloaded ZIP file.\n\nMake sure to first extract the ZIP somewhere like on your desktop, then start the Sonic3AIR executable in the extracted folder.", );
		#endif
			return false;
		}
	}

	return true;
}

bool EngineDelegate::setupCustomGameProfile()
{
	GameProfile& gameProfile = GameProfile::instance();

	if (FTX::FileSystem->exists(L"./oxygenproject.json"))
	{
		RMX_LOG_INFO("Loading game profile from 'oxygenproject.json'");
		// Load from the oxygenproject.json file
		gameProfile.loadOxygenProjectFromFile(L"./oxygenproject.json");
	}
	else
	{
		RMX_LOG_INFO("Setting up default game profile for Sonic 3 A.I.R.");
		// Setup game profile data -- this is done so that no oxygenproject.json is needed for the end-user version of S3AIR
		ConfigurationImpl::fillDefaultGameProfile(gameProfile);

		gameProfile.mAsmStackRange.first = 0xfffffd00;
		gameProfile.mAsmStackRange.second = 0xfffffe00;

		gameProfile.mDataPackages.clear();
		gameProfile.mDataPackages.push_back(GameProfile::DataPackage(L"enginedata.bin",    true));
		gameProfile.mDataPackages.push_back(GameProfile::DataPackage(L"gamedata.bin",      true));
		gameProfile.mDataPackages.push_back(GameProfile::DataPackage(L"audiodata.bin",     true));
		gameProfile.mDataPackages.push_back(GameProfile::DataPackage(L"audioremaster.bin", false));	// Optional package
	}

	// Return true, so the engine won't load the oxygenprofile.json by itself
	return true;
}

void EngineDelegate::startupGame(EmulatorInterface& emulatorInterface)
{
	mGame.startup(emulatorInterface);
	mCommandForwarder.startup();
}

void EngineDelegate::shutdownGame()
{
	mGame.shutdown();
	mCommandForwarder.shutdown();
}

void EngineDelegate::updateGame(float timeElapsed)
{
	mGame.update(timeElapsed);
	mCommandForwarder.update(timeElapsed);
}

void EngineDelegate::registerScriptBindings(lemon::Module& module)
{
	Game::instance().registerScriptBindings(module);
}

void EngineDelegate::registerNativizedCode(lemon::Program& program)
{
	static lemon::NativizedOpcodeProvider instance(&lemon::createNativizedCodeLookup);
	program.mNativizedOpcodeProvider = instance.isValid() ? &instance : nullptr;
}

void EngineDelegate::onRuntimeInit(CodeExec& codeExec)
{
#if 0
	// Generation of scripts from data in ROM or another source
	ResourceScriptGenerator::generateLevelObjectTableScript(codeExec);
	ResourceScriptGenerator::generateLevelRingsTableScript(codeExec);
	ResourceScriptGenerator::convertLevelObjectsBinToScript(L"E:/Projects/Workspace/Oxygen/_SonLVL/skdisasm-master/Levels/AIZ/Object Pos/1.bin", L"output_objects.lemon");
	ResourceScriptGenerator::convertLevelRingsBinToScript(L"E:/Projects/Workspace/Oxygen/_SonLVL/skdisasm-master/Levels/AIZ/Ring Pos/1.bin", L"output_rings.lemon");
#endif
}

void EngineDelegate::onPreFrameUpdate()
{
	mGame.onPreUpdateFrame();
}

void EngineDelegate::onPostFrameUpdate()
{
	mGame.onPostUpdateFrame();
}

void EngineDelegate::onControlsUpdate()
{
	mGame.onUpdateControls();
}

void EngineDelegate::onPreSaveStateLoad()
{
	mGame.resetCurrentMode();
}

void EngineDelegate::onApplicationLostFocus()
{
	// Automatic pause on focus change can be quite annoying in the desktop version (especially for development), so activate it only on mobile
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB) || defined(PLATFORM_IOS)
	if (mGame.shouldPauseOnFocusLoss())
	{
		Application::instance().enablePauseOnFocusLoss();
	}
#endif
}

bool EngineDelegate::mayLoadScriptMods()
{
	return allowModdedData();
}

bool EngineDelegate::allowModdedData()
{
	return !mGame.isInTimeAttackMode();
}

bool EngineDelegate::useDeveloperFeatures()
{
	return mConfiguration.mDevMode.mEnabled;
}

void EngineDelegate::onActiveModsChanged()
{
	mGame.onActiveModsChanged();
}

void EngineDelegate::onGameRecordingHeaderLoaded(const std::string& buildString, const std::vector<uint8>& buffer)
{
	mGame.onGameRecordingHeaderLoaded(buildString, buffer);
}

void EngineDelegate::onGameRecordingHeaderSave(std::vector<uint8>& buffer)
{
	mGame.onGameRecordingHeaderSave(buffer);
}

Font& EngineDelegate::getDebugFont(int size)
{
	return (size >= 10) ? global::mOxyfontRegular : global::mSmallfontSemiOutlined;
}

void EngineDelegate::fillDebugVisualization(Bitmap& bitmap, int& mode)
{
	mGame.fillDebugVisualization(bitmap, mode);
}
