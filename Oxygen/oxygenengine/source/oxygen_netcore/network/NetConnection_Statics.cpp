/*
*	Part of the Oxygen Engine / Sonic 3 A.I. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen_netcore/pch.h"
#include "oxygen_netcore/network/NetConnection.h"

#if defined(PLATFORM_PS3)
const int NetConnection::TIMEOUT_SECONDS = 30;
const int NetConnection::STALE_SECONDS = 5 * 60;
#endif
