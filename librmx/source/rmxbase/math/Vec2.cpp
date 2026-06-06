/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"

#if defined(PLATFORM_PS3)
template<> const Vec2<float> Vec2<float>::ZERO(0, 0);
template<> const Vec2<float> Vec2<float>::UNIT_X(1, 0);
template<> const Vec2<float> Vec2<float>::UNIT_Y(0, 1);

template<> const Vec2<int> Vec2<int>::ZERO(0, 0);
template<> const Vec2<int> Vec2<int>::UNIT_X(1, 0);
template<> const Vec2<int> Vec2<int>::UNIT_Y(0, 1);
#endif
