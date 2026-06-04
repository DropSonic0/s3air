/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include <rmxbase.h>

#ifdef PLATFORM_PS3

#include "oxygen_netcore/network/HighLevelPacketBase.h"

namespace highlevel
{
	std::map<uint32, std::string> PacketBase::mPacketTypeRegistry;
}

#endif
