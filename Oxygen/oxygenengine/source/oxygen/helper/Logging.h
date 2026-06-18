/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>


namespace oxygen
{
	class Logging
	{
	public:
		static void startup(const std::wstring& filename);
		static void shutdown();

#if defined(PLATFORM_PS3) || defined(__PS3__) || defined(__CELLOS_LV2__)
		static void setDiagnosticFrame(int frame);
		static int getDiagnosticFrame();
		static bool shouldLogDiagnostics();
#endif
		static void setAssertBreakCaption(const std::string& caption);
	};
}
