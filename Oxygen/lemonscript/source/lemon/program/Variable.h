/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/DataType.h"
#include "lemon/utility/FlyweightString.h"

#include <functional>


namespace lemon
{
	class Environment;

#if defined(__CELLOS_LV2__) || defined(__PPU__) || defined(__SN_TARGET_PS3__) || defined(__cell__)
	typedef int64 (*VariableGetterType)();
	typedef void (*VariableSetterType)(int64);
	typedef int64* (*VariableAccessorType)();
#else
	 typedef ::std::function<int64()> VariableGetterType;
	typedef ::std::function<void(int64)> VariableSetterType;
	typedef ::std::function<int64*()> VariableAccessorType;
#endif


	class API_EXPORT Variable
	{
	public: virtual ~Variable() {}

	friend class Module;
	friend class Runtime;
	friend class ScriptFunction;

	public:
		enum class Type : uint8
		{
			LOCAL    = 0,		// Variable IDs 0x00000000...0x0fffffff
			GLOBAL   = 1,		// Variable IDs 0x10000000...0x1fffffff
			USER     = 2,		// Variable IDs 0x20000000...0x2fffffff
			EXTERNAL = 3		// Variable IDs 0x30000000...0x3fffffff
		};

	public:
		virtual int64 getValue() const = 0;
		virtual void setValue(int64 value) = 0;

		inline Type getType() const							 { return mType; }
		inline FlyweightString getName() const				 { return mName; }
		inline uint32 getID() const							 { return mID; }
		inline const DataTypeDefinition* getDataType() const { return mDataType; }
		inline size_t getStaticMemoryOffset() const			 { return mStaticMemoryOffset; }
		inline size_t getStaticMemorySize() const			 { return mStaticMemorySize; }

	protected:
		inline Variable(Type type) : mType(type), mID(0), mDataType(nullptr), mStaticMemoryOffset(0), mStaticMemorySize(0) {}

	private:
		Type mType;
		FlyweightString mName;
		uint32 mID;
		const DataTypeDefinition* mDataType;
		size_t mStaticMemoryOffset;
		size_t mStaticMemorySize;
	};


	class API_EXPORT LocalVariable : public Variable
	{
	public: virtual ~LocalVariable() {}
	public:
		inline LocalVariable() : Variable(Type::LOCAL) {}

		// Do not use these for variables, instead look it up in the current state's variables stack
		int64 getValue() const override		 { return 0; }
		void setValue(int64 value) override  {}
	};


	class API_EXPORT GlobalVariable : public Variable
	{
	public: virtual ~GlobalVariable() {}
	public:
		inline GlobalVariable() : Variable(Type::GLOBAL), mInitialValue(0) {}

		// Do not use these for variables, instead look it up in the runtime's global variables list
		int64 getValue() const override		 { return 0; }
		void setValue(int64 value) override  {}

	public:
		int64 mInitialValue;
	};


	class API_EXPORT UserDefinedVariable : public Variable
	{
	public:
		virtual ~UserDefinedVariable() {}
		inline UserDefinedVariable() : Variable(Type::USER) {}

		int64 getValue() const override;
		void setValue(int64 value) override;

	public:
		VariableGetterType mGetter;
		VariableSetterType mSetter;
	};


	class API_EXPORT ExternalVariable : public Variable
	{
	public:
		virtual ~ExternalVariable() {}
		inline ExternalVariable() : Variable(Type::EXTERNAL) {}

		// Do not use these for variables, instead directly access the pointer with the right data type
		int64 getValue() const override		 { return 0; }
		void setValue(int64 value) override  {}

	public:
		VariableAccessorType mAccessor;
	};

	inline int64 UserDefinedVariable::getValue() const { return (this->mGetter) ? this->mGetter() : 0; }
	inline void UserDefinedVariable::setValue(int64 value) { if (this->mSetter) this->mSetter(value); }

}
