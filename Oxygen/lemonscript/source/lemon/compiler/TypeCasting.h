/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/function/Function.h"
#include "lemon/compiler/Operators.h"
#include "lemon/utility/AnyBaseValue.h"

#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <optional>
#else
#ifndef _STD_OPTIONAL_DEFINED_PS3_
#define _STD_OPTIONAL_DEFINED_PS3_
namespace std {
    struct nullopt_t {
        struct init {};
        explicit constexpr nullopt_t(init) {}
    };
    static const nullopt_t nullopt{nullopt_t::init{}};

    template<typename T>
    class optional {
    public:
        optional() : mHasValue(false) {}
        optional(const nullopt_t&) : mHasValue(false) {}
        optional(const T& value) : mHasValue(true), mValue(value) {}
        optional(const optional& other) : mHasValue(other.mHasValue), mValue(other.mValue) {}
        
        optional& operator=(const nullopt_t&) {
            mHasValue = false;
            return *this;
        }
        optional& operator=(const T& value) {
            mHasValue = true;
            mValue = value;
            return *this;
        }
        optional& operator=(const optional& other) {
            if (this != &other) {
                mHasValue = other.mHasValue;
                mValue = other.mValue;
            }
            return *this;
        }
        
        const T& operator*() const { return mValue; }
        T& operator*() { return mValue; }
        const T* operator->() const { return &mValue; }
        T* operator->() { return &mValue; }
        
        operator bool() const { return mHasValue; }
        bool has_value() const { return mHasValue; }
        const T& value() const { return mValue; }
        T& value() { return mValue; }
        void reset() { mHasValue = false; }
        
    private:
        bool mHasValue;
        T mValue;
    };
}
#endif
#endif


namespace lemon
{
	struct CompileOptions;
	struct DataTypeDefinition;

	class TypeCasting
	{
	public:
		static const constexpr uint8 CANNOT_CAST = 0xff;

		struct CastHandling
		{
			enum class Result
			{
				NO_CAST,	// No cast needed
				INVALID,	// Cast not possible
				BASE_CAST,	// Cast between base types
				ANY_CAST,	// Cast from source type to "any"
			};

			Result mResult = Result::INVALID;
			BaseCastType mBaseCastType = BaseCastType::INVALID;
			uint8 mCastPriority = 0xff;

			inline CastHandling() {}
			inline CastHandling(Result result, uint8 castPriority) : mResult(result), mCastPriority(castPriority) {}
			inline CastHandling(BaseCastType baseCastType, uint8 castPriority) : mResult(Result::BASE_CAST), mBaseCastType(baseCastType), mCastPriority(castPriority) {}
		};

		struct BinaryOperatorSignature
		{
			const DataTypeDefinition* mLeft;
			const DataTypeDefinition* mRight;
			const DataTypeDefinition* mResult;
			inline BinaryOperatorSignature(const DataTypeDefinition* left, const DataTypeDefinition* right, const DataTypeDefinition* result) : mLeft(left), mRight(right), mResult(result) {}
		};

	public:
		static const std::vector<BinaryOperatorSignature>& getBinarySignaturesForOperator(Operator op);

	public:
		inline explicit TypeCasting(const CompileOptions& compileOptions) : mCompileOptions(compileOptions) {}

		bool canImplicitlyCastTypes(const DataTypeDefinition& original, const DataTypeDefinition& target) const;
		bool canExplicitlyCastTypes(const DataTypeDefinition& original, const DataTypeDefinition& target) const;
		CastHandling getCastHandling(const DataTypeDefinition* original, const DataTypeDefinition* target, bool explicitCast) const;

		CastHandling castBaseValue(const AnyBaseValue& originalValue, const DataTypeDefinition* originalType, AnyBaseValue& outTargetValue, const DataTypeDefinition* targetType, bool explicitCast = false) const;

		bool canMatchSignature(const std::vector<const DataTypeDefinition*>& original, const Function::ParameterList& target, size_t* outFailedIndex = nullptr) const;
		uint16 getPriorityOfSignature(const BinaryOperatorSignature& signature, const DataTypeDefinition* left, const DataTypeDefinition* right) const;
		uint32 getPriorityOfSignature(const std::vector<const DataTypeDefinition*>& original, const Function::ParameterList& target) const;

		std::optional<size_t> getBestOperatorSignature(const std::vector<BinaryOperatorSignature>& signatures, bool exactMatchLeftRequired, const DataTypeDefinition* left, const DataTypeDefinition* right) const;

	private:
		uint8 getImplicitCastPriority(const DataTypeDefinition* original, const DataTypeDefinition* target) const;

	private:
		const CompileOptions& mCompileOptions;
	};
}