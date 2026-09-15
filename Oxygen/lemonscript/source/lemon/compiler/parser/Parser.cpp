/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/compiler/parser/Parser.h"
#include "lemon/compiler/parser/ParserHelper.h"
#include "lemon/compiler/parser/ParserTokens.h"

#if defined(PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__PPU__) || defined(__SN_TARGET_PS3__) || defined(__cell__)
template<> genericmanager::detail::ElementFactoryMap<lemon::ParserToken> genericmanager::Manager<lemon::ParserToken>::mFactoryMap = genericmanager::detail::ElementFactoryMap<lemon::ParserToken>();
#endif


namespace lemon
{
	namespace
	{

		static std::map<uint64, const DataTypeDefinition*> varTypeLookup;
		static std::map<uint64, Keyword> keywordLookup;
		static std::map<uint64, std::string> reservedKeywordLookup;

		void initializeLookups()
		{
			if (!varTypeLookup.empty())
				return;

			varTypeLookup[rmx::getMurmur2_64(String("void"))]   = &PredefinedDataTypes::VOID;
			varTypeLookup[rmx::getMurmur2_64(String("s8"))]     = &PredefinedDataTypes::INT_8;
			varTypeLookup[rmx::getMurmur2_64(String("s16"))]    = &PredefinedDataTypes::INT_16;
			varTypeLookup[rmx::getMurmur2_64(String("s32"))]    = &PredefinedDataTypes::INT_32;
			varTypeLookup[rmx::getMurmur2_64(String("s64"))]    = &PredefinedDataTypes::INT_64;
			varTypeLookup[rmx::getMurmur2_64(String("bool"))]   = &PredefinedDataTypes::UINT_8;
			varTypeLookup[rmx::getMurmur2_64(String("u8"))]     = &PredefinedDataTypes::UINT_8;
			varTypeLookup[rmx::getMurmur2_64(String("u16"))]    = &PredefinedDataTypes::UINT_16;
			varTypeLookup[rmx::getMurmur2_64(String("u32"))]    = &PredefinedDataTypes::UINT_32;
			varTypeLookup[rmx::getMurmur2_64(String("u64"))]    = &PredefinedDataTypes::UINT_64;
			varTypeLookup[rmx::getMurmur2_64(String("float"))]  = &PredefinedDataTypes::FLOAT;
			varTypeLookup[rmx::getMurmur2_64(String("double"))] = &PredefinedDataTypes::DOUBLE;
			varTypeLookup[rmx::getMurmur2_64(String("string"))] = &PredefinedDataTypes::STRING;

			keywordLookup[rmx::getMurmur2_64(String("function"))]  = Keyword::FUNCTION;
			keywordLookup[rmx::getMurmur2_64(String("global"))]    = Keyword::GLOBAL;
			keywordLookup[rmx::getMurmur2_64(String("constant"))]  = Keyword::CONSTANT;
			keywordLookup[rmx::getMurmur2_64(String("define"))]    = Keyword::DEFINE;
			keywordLookup[rmx::getMurmur2_64(String("declare"))]   = Keyword::DECLARE;
			keywordLookup[rmx::getMurmur2_64(String("return"))]    = Keyword::RETURN;
			keywordLookup[rmx::getMurmur2_64(String("call"))]      = Keyword::CALL;
			keywordLookup[rmx::getMurmur2_64(String("jump"))]      = Keyword::JUMP;
			keywordLookup[rmx::getMurmur2_64(String("break"))]     = Keyword::BREAK;
			keywordLookup[rmx::getMurmur2_64(String("continue"))]  = Keyword::CONTINUE;
			keywordLookup[rmx::getMurmur2_64(String("if"))]        = Keyword::IF;
			keywordLookup[rmx::getMurmur2_64(String("else"))]      = Keyword::ELSE;
			keywordLookup[rmx::getMurmur2_64(String("while"))]     = Keyword::WHILE;
			keywordLookup[rmx::getMurmur2_64(String("for"))]       = Keyword::FOR;
			keywordLookup[rmx::getMurmur2_64(String("addressof"))] = Keyword::ADDRESSOF;

			const char* reservedKeywords[] =
			{
				"local", "auto", "switch", "case", "select", "choose", "do", "const", "fixed", "static",
				"virtual", "override", "enum", "struct", "class", "foreach", "in", "out", "ref", "typeof",
			};
			for (size_t i = 0; i < sizeof(reservedKeywords) / sizeof(reservedKeywords[0]); ++i)
			{
				reservedKeywordLookup[rmx::getMurmur2_64(reservedKeywords[i])] = reservedKeywords[i];
			}
		}

		void analyseIdentifier(const std::string_view& identifier, ParserTokenList& outTokens, uint32 lineNumber)
		{
			initializeLookups();
			const uint64 identifierHash = rmx::getMurmur2_64(identifier);

			// Check for variable type
			{
				const auto it = varTypeLookup.find(identifierHash);
				if (it != varTypeLookup.end())
				{
					VarTypeParserToken& token = outTokens.create<VarTypeParserToken>();
					token.mDataType = it->second;
					return;
				}
			}

			// Check for keyword
			{
				const auto it = keywordLookup.find(identifierHash);
				if (it != keywordLookup.end())
				{
					KeywordParserToken& token = outTokens.create<KeywordParserToken>();
					token.mKeyword = it->second;
					return;
				}
			}

			// Check for reserved identifier
			{
				if (reservedKeywordLookup.count(identifierHash) > 0)
				{
					CHECK_ERROR(false, "Reserved keyword '" << reservedKeywordLookup[identifierHash] << "' cannot be used as an identifier, please rename", lineNumber);
					return;
				}
			}

			// Check for "true", "false"
			static const uint64 trueHash  = rmx::getMurmur2_64(std::string_view("true"));
			static const uint64 falseHash = rmx::getMurmur2_64(std::string_view("false"));
			if (identifierHash == trueHash || identifierHash == falseHash)
			{
				ConstantParserToken& token = outTokens.create<ConstantParserToken>();
				token.mValue.set(identifierHash == trueHash);
				token.mBaseType = BaseType::INT_CONST;
				return;
			}

			// Just an identifier
			IdentifierParserToken& token = outTokens.create<IdentifierParserToken>();
			token.mName.set(identifier);
		}
	}


	void Parser::splitLineIntoTokens(std::string_view input, uint32 lineNumber, ParserTokenList& outTokens)
	{
		const size_t length = input.length();

		// Do the actual parsing
		for (size_t pos = 0; pos < length; )
		{
			// Look at next character
			const char firstCharacter = input[pos];

			if (firstCharacter == '{')
			{
				outTokens.create<KeywordParserToken>().mKeyword = Keyword::BLOCK_BEGIN;
				++pos;
			}
			else if (firstCharacter == '}')
			{
				outTokens.create<KeywordParserToken>().mKeyword = Keyword::BLOCK_END;
				++pos;
			}
			else if (ParserHelper::isDigitOrDot(firstCharacter))
			{
				// It is a number (integer or floating point)
				const std::string_view rest = input.substr(pos);
				const ParserHelper::ParseNumberResult result = ParserHelper::collectNumber(rest);
				switch (result.mType)
				{
					case ParserHelper::ParseNumberResult::Type::INTEGER:
					{
						ConstantParserToken& token = outTokens.create<ConstantParserToken>();
						token.mValue = result.mValue;
						token.mBaseType = BaseType::INT_CONST;
						break;
					}

					case ParserHelper::ParseNumberResult::Type::FLOAT:
					{
						ConstantParserToken& token = outTokens.create<ConstantParserToken>();
						token.mValue = result.mValue;
						token.mBaseType = BaseType::FLOAT;
						break;
					}

					case ParserHelper::ParseNumberResult::Type::DOUBLE:
					{
						ConstantParserToken& token = outTokens.create<ConstantParserToken>();
						token.mValue = result.mValue;
						token.mBaseType = BaseType::DOUBLE;
						break;
					}

					default:
						CHECK_ERROR(false, "Invalid number '" << rest.substr(0, result.mBytesRead) << "'", lineNumber);
				}
				pos += result.mBytesRead;
			}
			else if (ParserHelper::isLetter(firstCharacter) || (firstCharacter == '_'))
			{
				// It is an identifier or keyword
				const std::string_view rest = input.substr(pos);
				const size_t identifierLength = ParserHelper::collectIdentifier(rest);
				pos += identifierLength;
				analyseIdentifier(rest.substr(0, identifierLength), outTokens, lineNumber);
			}
			else if (firstCharacter == '@')
			{
				// It is a label
				const std::string_view rest = input.substr(pos);
				++pos;
				const size_t identifierLength = ParserHelper::collectIdentifier(rest.substr(1));
				pos += identifierLength;
				LabelParserToken& token = outTokens.create<LabelParserToken>();
				token.mName = rest.substr(0, identifierLength + 1);
			}
			else if (ParserHelper::isOperatorCharacter(firstCharacter))
			{
				// It is a single operator or multiple of them
				const std::string_view rest = input.substr(pos);
				const size_t operatorsLength = ParserHelper::collectOperators(rest);
				const char* start = &input[pos];

				Operator op;
				for (size_t i = 0; i < operatorsLength; )
				{
					const size_t operatorLength = ParserHelper::findOperator(input.substr(pos + i, operatorsLength - i), op);
					CHECK_ERROR(operatorLength > 0, "Operator not recognized", lineNumber);

					if (op == Operator::BINARY_DIVIDE)
					{
						// Check for comments
						if (start[i+1] == '/')
						{
							// Line comment: Pragma or not?
							pos += i+2;
							if (pos < input.size() && input[pos] == '#')
							{
								// Jump over '#' and ignore whitespace
								do
								{
									++pos;
								}
								while (input[pos] == ' ' || input[pos] == '\t');

								PragmaParserToken& token = outTokens.create<PragmaParserToken>();
									const std::string_view pragmaContent = input.substr(pos);
									token.mContent.assign(pragmaContent.data(), pragmaContent.size());
							}

							// We're done with this line
							return;
						}
					}

					OperatorParserToken& token = outTokens.create<OperatorParserToken>();
					token.mOperator = op;

					i += operatorLength;
				}

				pos += operatorsLength;
			}
			else if (firstCharacter == '"')
			{
				// It is a string
				++pos;
				size_t charactersRead;
				ParserHelper::collectStringLiteral(input.substr(pos), mBufferString, charactersRead, lineNumber);
				StringLiteralParserToken& token = outTokens.create<StringLiteralParserToken>();
				token.mString.set(mBufferString);
				pos += charactersRead + 1;
			}
			else
			{
				// Just skip all other characters
				++pos;
			}
		}
	}

}
