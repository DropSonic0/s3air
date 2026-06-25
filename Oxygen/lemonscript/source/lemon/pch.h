/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>

#if defined(__CELLOS_LV2__) || defined(__PPU__) || defined(__SN_TARGET_PS3__) || defined(__cell__)
	#ifndef PLATFORM_PS3
		#define PLATFORM_PS3
	#endif
#endif

#include <string>
#include <vector>
#include <map>
#include <set>

#if !defined(PLATFORM_PS3)
	#include <unordered_map>
	#include <unordered_set>
	#include <optional>
	#include <string_view>
#endif

#if defined(PLATFORM_PS3)
	#define LEMON_UNORDERED_MAP std::map
	#define LEMON_UNORDERED_SET std::set
#else
	#define LEMON_UNORDERED_MAP std::unordered_map
	#define LEMON_UNORDERED_SET std::unordered_set
#endif

#include "lemon/compiler/Utility.h"
