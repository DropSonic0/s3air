/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/DataType.h"


namespace lemon
{
	struct DataTypeDefinition;

	struct CompileOptions
	{
		// Options to be set before compilation
#if defined(PLATFORM_PS3)
		const DataTypeDefinition* mExternalAddressType;
#else
		const DataTypeDefinition* mExternalAddressType = &PredefinedDataTypes::UINT_64;
#endif
		std::wstring mOutputCombinedSource;
		std::wstring mOutputNativizedSource;
		std::wstring mOutputTranslatedSource;
#if defined(PLATFORM_PS3)
		bool mConsumeProcessedPragmas;
#else
		bool mConsumeProcessedPragmas = true;
#endif

		// Set during compilation
#if defined(PLATFORM_PS3)
		uint32 mScriptFeatureLevel;
#else
		uint32 mScriptFeatureLevel = 1;
#endif

#if defined(PLATFORM_PS3)
		inline CompileOptions() : mExternalAddressType(&PredefinedDataTypes::UINT_64), mConsumeProcessedPragmas(true), mScriptFeatureLevel(1) {}
#endif
	};

	enum class Keyword : uint8
	{
		_INVALID = 0,
		BLOCK_BEGIN,
		BLOCK_END,
		FUNCTION,
		GLOBAL,
		CONSTANT,
		DEFINE,
		DECLARE,
		RETURN,
		CALL,
		JUMP,
		BREAK,
		CONTINUE,
		IF,
		ELSE,
		WHILE,
		FOR,
		ADDRESSOF
	};

}
