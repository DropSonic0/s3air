/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/BaseType.h"
#include "lemon/utility/FlyweightString.h"


namespace lemon
{

	struct DataTypeDefinition
	{
		enum class Class : uint8
		{
			VOID,
			ANY,
			INTEGER,
			FLOAT,
			STRING,
			CUSTOM
		};

	public:
		inline DataTypeDefinition(const char* name, uint16 id, Class class_, size_t bytes, BaseType baseType) :
			mNameString(name),
			mID(id),
			mClass(class_),
			mBytes(bytes),
			mBaseType(baseType)
		{}
		virtual ~DataTypeDefinition() {}

		template<typename T> const T& as() const  { return static_cast<const T&>(*this); }

		FlyweightString getName() const;
		inline uint16 getID() const			{ return mID; }
		inline size_t getBytes() const		{ return mBytes; }
		inline Class getClass() const		{ return mClass; }
		inline BaseType getBaseType() const	{ return mBaseType; }
		inline bool isPredefined() const	{ return mClass > Class::STRING; }

		virtual uint16 getDataTypeHash() const  { return mID; }

	private:
		const char* mNameString;
		mutable FlyweightString mName;

		uint16 mID;
		const size_t mBytes;
		const Class mClass;
		const BaseType mBaseType;	// If compatible to a base type (from the runtime's point of view), set this to something different than VOID
	};


	struct VoidDataType : public DataTypeDefinition
	{
	public:
		inline VoidDataType() :
			DataTypeDefinition("void", 0, Class::VOID, 0, BaseType::VOID)
		{}
	};


	struct AnyDataType : public DataTypeDefinition
	{
	public:
		inline AnyDataType() :
			DataTypeDefinition("any", 1, Class::ANY, 0, BaseType::UINT_64)
		{}
	};


	struct IntegerDataType : public DataTypeDefinition
	{
	public:
		enum class Semantics
		{
			DEFAULT,
			CONSTANT,
			BOOLEAN
		};

		const Semantics mSemantics;
		const bool mIsSigned;
		const uint8 mSizeBits;	// 0 for 8-bit data types, 1 for 16-bit, 2 for 32-bit, 3 for 64-bit

	public:
		inline IntegerDataType(const char* name, uint16 id, size_t bytes, Semantics semantics, bool isSigned, BaseType baseType) :
			DataTypeDefinition(name, id, Class::INTEGER, bytes, baseType),
			mSemantics(semantics),
			mIsSigned(isSigned),
			mSizeBits((bytes == 1) ? 0 : (bytes == 2) ? 1 : (bytes == 4) ? 2 : 3)
		{}
	};


	struct FloatDataType : public DataTypeDefinition
	{
	public:
		inline FloatDataType(const char* name, uint16 id, size_t bytes) :
			DataTypeDefinition(name, id, Class::FLOAT, bytes, (bytes == 4) ? BaseType::FLOAT : BaseType::DOUBLE)
		{}
	};


	struct StringDataType : public DataTypeDefinition
	{
	public:
		inline explicit StringDataType(uint16 id) :
			DataTypeDefinition("string", id, Class::STRING, 8, BaseType::UINT_64)
		{}

		// Rather unfortunately, the data type hash for string needs to be the same as for u64, for feature level 1 compatibility regarding function overloading
		uint16 getDataTypeHash() const override;
	};


	struct CustomDataType : public DataTypeDefinition
	{
	public:
		explicit CustomDataType(const char* name, uint16 id, BaseType baseType);
	};


	struct PredefinedDataTypes
	{
		static const VoidDataType VOID;
		static const AnyDataType ANY;

		static const IntegerDataType BOOL;
		static const IntegerDataType UINT_8;
		static const IntegerDataType UINT_16;
		static const IntegerDataType UINT_32;
		static const IntegerDataType UINT_64;
		static const IntegerDataType INT_8;
		static const IntegerDataType INT_16;
		static const IntegerDataType INT_32;
		static const IntegerDataType INT_64;
		static const IntegerDataType CONST_INT;

		static const FloatDataType FLOAT;
		static const FloatDataType DOUBLE;

		static const StringDataType STRING;

		static void collectPredefinedDataTypes(std::vector<const DataTypeDefinition*>& outDataTypes);
	};


	struct DataTypeHelper
	{
		static size_t getSizeOfBaseType(BaseType baseType);
		static const DataTypeDefinition* getDataTypeDefinitionForBaseType(BaseType baseType);

		static bool isPureIntegerBaseCast(BaseCastType baseCastType);
	};

}
