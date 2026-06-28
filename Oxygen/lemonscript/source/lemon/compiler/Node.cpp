/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/compiler/Node.h"

#if defined(PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__PPU__) || defined(__SN_TARGET_PS3__) || defined(__cell__)
template<> genericmanager::detail::ElementFactoryMap<lemon::Node> genericmanager::Manager<lemon::Node>::mFactoryMap = genericmanager::detail::ElementFactoryMap<lemon::Node>();
#endif
