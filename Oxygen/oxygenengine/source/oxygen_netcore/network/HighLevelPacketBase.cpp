/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen_netcore/pch.h"
#include "oxygen_netcore/network/HighLevelPacketBase.h"

#if defined(PLATFORM_PS3)
namespace highlevel
{
	std::map<uint32, std::string> PacketBase::mPacketTypeRegistry;
}
#endif

// No more manual static member definitions needed if using the function-local static trick
