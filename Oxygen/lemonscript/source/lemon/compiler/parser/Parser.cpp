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


namespace lemon
{
	namespace
	{

		static std::map<uint64, const DataTypeDefinition*> varTypeLookup;
		static std::map<uint64, Keyword> keywordLookup;
		static std::vector<const char*> reservedKeywords;
		static std::map<uint64, std::string> reservedKeywordLookup;
		static bool lookupTablesInitialized = false;

		void initializeLookupTables()
		{
			if (lookupTablesInitialized)
				return;

			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("void")),   &PredefinedDataTypes::VOID));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("s8")),     &PredefinedDataTypes::INT_8));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("s16")),    &PredefinedDataTypes::INT_16));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("s32")),    &PredefinedDataTypes::INT_32));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("s64")),    &PredefinedDataTypes::INT_64));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("bool")),   &PredefinedDataTypes::UINT_8));		// Only a synonym for u8
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("u8")),     &PredefinedDataTypes::UINT_8));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("u16")),    &PredefinedDataTypes::UINT_16));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("u32")),    &PredefinedDataTypes::UINT_32));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("u64")),    &PredefinedDataTypes::UINT_64));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("float")),  &PredefinedDataTypes::FLOAT));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("double")), &PredefinedDataTypes::DOUBLE));
			varTypeLookup.insert(std::make_pair(rmx::getMurmur2_64(String("string")), &PredefinedDataTypes::STRING));

			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("function")),  Keyword::FUNCTION));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("global")),    Keyword::GLOBAL));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("constant")),  Keyword::CONSTANT));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("define")),    Keyword::DEFINE));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("declare")),   Keyword::DECLARE));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("return")),    Keyword::RETURN));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("call")),      Keyword::CALL));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("jump")),      Keyword::JUMP));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("break")),     Keyword::BREAK));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("continue")),  Keyword::CONTINUE));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("if")),        Keyword::IF));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("else")),      Keyword::ELSE));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("while")),     Keyword::WHILE));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("for")),       Keyword::FOR));
			keywordLookup.insert(std::make_pair(rmx::getMurmur2_64(String("addressof")), Keyword::ADDRESSOF));

			reservedKeywords.push_back("local");
			reservedKeywords.push_back("auto");
			reservedKeywords.push_back("switch");
			reservedKeywords.push_back("case");
			reservedKeywords.push_back("select");
			reservedKeywords.push_back("choose");
			reservedKeywords.push_back("do");
			reservedKeywords.push_back("const");
			reservedKeywords.push_back("fixed");
			reservedKeywords.push_back("static");
			reservedKeywords.push_back("virtual");
			reservedKeywords.push_back("override");
			reservedKeywords.push_back("enum");
			reservedKeywords.push_back("struct");
			reservedKeywords.push_back("class");
			reservedKeywords.push_back("foreach");
			reservedKeywords.push_back("in");
			reservedKeywords.push_back("out");
			reservedKeywords.push_back("ref");
			reservedKeywords.push_back("typeof");

			for (size_t i = 0; i < reservedKeywords.size(); ++i)
			{
				const char* str = reservedKeywords[i];
				reservedKeywordLookup.insert(std::make_pair(rmx::getMurmur2_64(str), std::string(str)));
			}

			lookupTablesInitialized = true;
		}

		void analyseIdentifier(const std::string_view& identifier, ParserTokenList& outTokens, uint32 lineNumber)
		{
			initializeLookupTables();
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
			static const uint64 trueHash  = rmx::getMurmur2_64(String("true"));
			static const uint64 falseHash = rmx::getMurmur2_64(String("false"));
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
				token.mName.set(rest.substr(0, identifierLength + 1));
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
								std::string_view pragmaStr = input.substr(pos);
								token.mContent.assign(pragmaStr.data(), pragmaStr.length());
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
