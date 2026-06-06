/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen_netcore/pch.h"
#include "oxygen_netcore/network/LowLevelPackets.h"

#if defined(PLATFORM_PS3)
const VersionRange<uint8> lowlevel::PacketBase::LOWLEVEL_PROTOCOL_VERSIONS = VersionRange<uint8>(1, 1);
#endif
