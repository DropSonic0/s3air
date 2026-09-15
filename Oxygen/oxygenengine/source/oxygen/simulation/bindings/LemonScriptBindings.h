/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "oxygen/simulation/DebuggingInterfaces.h"

namespace lemon
{
	class Module;
	class Runtime;
}


class LemonScriptBindings
{
public:
	void registerBindings(lemon::Module& module);
	void setDebugNotificationInterface(DebugNotificationInterface* debugNotificationInterface);

public:
#if defined(PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__PPU__) || defined(__SN_TARGET_PS3__) || defined(__cell__)
	static DebugNotificationInterface* mDebugNotificationInterface;
#else
	static inline DebugNotificationInterface* mDebugNotificationInterface = nullptr;
#endif
};
