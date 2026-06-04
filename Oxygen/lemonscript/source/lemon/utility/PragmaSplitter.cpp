/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/utility/PragmaSplitter.h"


namespace lemon
{
	PragmaSplitter::PragmaSplitter(std::string_view input)
	{
		size_t pos = 0;
		while (pos < input.length())
		{
			// Skip all spaces
			while (pos < input.length() && input[pos] == 32)
				++pos;

			if (pos < input.length())
			{
				const size_t startPos = pos;
				while (pos < input.length() && input[pos] != 32)
					++pos;

				mEntries.push_back(Entry());

				// Split part string into argument and value
				const std::string_view part = input.substr(startPos, pos - startPos);
#if defined(PLATFORM_PS3)
				size_t left = std::string::npos;
				for (size_t i = 0; i < part.length(); ++i)
				{
					if (part[i] == '(')
					{
						left = i;
						break;
					}
				}
#else
				const size_t left = part.find_first_of('(');
#endif
				if (left != std::string::npos)
				{
					RMX_CHECK(part[part.length() - 1] == ')', "No matching parentheses in pragma found", continue);
					mEntries.back().mArgument = part.substr(0, left);
					mEntries.back().mValue = part.substr(left + 1, part.length() - left - 2);
				}
				else
				{
					mEntries.back().mArgument = part;
				}
			}
		}
	}
}
