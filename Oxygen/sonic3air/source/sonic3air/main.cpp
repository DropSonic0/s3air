/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/EngineDelegate.h"
#include "sonic3air/GameArgumentsReader.h"
#include "sonic3air/helper/PackageBuilder.h"
#include "sonic3air/platform/PlatformSpecifics.h"

#include "oxygen/platform/PlatformFunctions.h"

#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
#include <sys/process.h>
#include <sys/spu_initialize.h>
#include <cell/sysmodule.h>
SYS_PROCESS_PARAM(1001, 1024 * 1024)
#endif


// [Added for Switch platform] HJW: I know it's sloppy to put this here... it'll get moved afterwards
// Building with my env (msys2,gcc) requires this stub for some reason
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#ifndef pathconf
long pathconf(const char* path, int name)
{
	errno = ENOSYS;
	return -1;
}
#endif
#endif

#if defined(PLATFORM_WINDOWS) & !defined(__GNUC__)
extern "C"
{
	// Tell graphics drivers to prefer the dedicated GPU, if there's integrated graphics as well
	_declspec(dllexport) uint32 NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#if defined(PLATFORM_VITA)
extern "C"
{
	// Any value higher than 324 MB will make the game either boot without sound or just crash the PSVITA due to lack of physical RAM
	int _newlib_heap_size_user = 324 * 1024 * 1024;
	unsigned int sceUserMainThreadStackSize = 4 * 1024 * 1024;
}
#endif


int main(int argc, char** argv)
{
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	sys_spu_initialize(6, 1);
	cellSysmoduleInitialize();
	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
	cellSysmoduleLoadModule(CELL_SYSMODULE_GCM_SYS);
	cellSysmoduleLoadModule(CELL_SYSMODULE_AUDIO);
	cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL);
	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_GAME);

	if (argc > 0 && argv && argv[0] && argv[0][0])
	{
		char path_buf[256];
		strncpy(path_buf, argv[0], sizeof(path_buf) - 1);
		path_buf[sizeof(path_buf) - 1] = '\0';
		char* slash = strrchr(path_buf, '/');
		if (!slash) slash = strrchr(path_buf, '\\');
		if (slash)
		{
			*slash = '\0';
			ps3_set_usrdir(path_buf);
		}
	}

	ps3_log("[PS3] main() entered");
#endif
	EngineMain::earlySetup();
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	ps3_log("[PS3] earlySetup() completed");
#endif
	PlatformSpecifics::platformStartup();
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
	ps3_log("[PS3] platformStartup() completed");
#endif

	GameArgumentsReader arguments;

#if defined(PLATFORM_VITA)
	argc = 0;
#else
	// Read command line arguments
	arguments.read(argc, argv);

	// For certain arguments, just try to forward them to an already running instance of S3AIR
	if (!arguments.mUrl.empty())
	{
		if (CommandForwarder::trySendCommand("ForwardedCommand:Url:" + arguments.mUrl))
			return 0;
	}

	// Make sure we're in the correct working directory
	PlatformFunctions::changeWorkingDirectory(arguments.mExecutableCallPath);
#endif

#if defined(PLATFORM_WINDOWS)
	// Check if the user has an old version of "audioremaster.bin", and remove it if that's the case
	//  -> As the newer installations don't include that file, it is most likely an out-dated one, and could cause problems (at least an assert) down the line
	if (FTX::FileSystem->exists(L"data/audioremaster.bin"))
		FTX::FileSystem->removeFile(L"data/audioremaster.bin");
#endif

#if !defined(PLATFORM_ANDROID) && !defined(PLATFORM_VITA)
	if (arguments.mPack)
	{
		PackageBuilder::performPacking();
		if (!arguments.mNativize && !arguments.mDumpCppDefinitions)		// In case multiple arguments got combined, the others would get ignored without this check
			return 0;
	}
#endif

	// Randomization is quite important for server communication
	randomize();

#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	try
#endif
	{
		// Create engine delegate and engine main instance
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
		ps3_log("[PS3] Creating EngineDelegate & EngineMain");
#endif
		EngineDelegate myDelegate;
		EngineMain myMain(myDelegate, arguments);
#if defined(__CELLOS_LV2__) || defined(__SNC__) || defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3)
		ps3_log("[PS3] Calling myMain.execute()");
#endif

		// Evaluate some more arguments
		Configuration& config = Configuration::instance();
		if (arguments.mNativize)
		{
			config.mRunScriptNativization = 1;
			config.mScriptNativizationOutput = L"source/sonic3air/_nativized/NativizedCode.inc";
			config.mExitAfterScriptLoading = true;
		}
		if (arguments.mDumpCppDefinitions)
		{
			config.mDumpCppDefinitionsOutput = L"scripts/_reference/cpp_core_functions.lemon";
			config.mExitAfterScriptLoading = true;
		}

		// Now run the game
		myMain.execute();
	}
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	catch (const std::exception& e)
	{
		RMX_ERROR("Caught unhandled exception in main loop: " << e.what(), );
	}
#endif

	return 0;
}
