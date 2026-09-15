/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/runtime/Runtime.h"

class EmulatorInterface;


struct RuntimeEnvironment : public lemon::Environment
{
#if defined(PLATFORM_PS3)
	static const uint64 TYPE = 0xb70bf0b0;
#else
	static const uint64 TYPE = rmx::compileTimeFNV_32("Oxygen_RuntimeEnvironment");
#endif

	inline RuntimeEnvironment() : lemon::Environment(TYPE) {}

	EmulatorInterface* mEmulatorInterface = nullptr;
};
