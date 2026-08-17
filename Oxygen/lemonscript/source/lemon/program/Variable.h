/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/DataType.h"
#include "lemon/utility/AnyBaseValue.h"
#include "lemon/utility/FlyweightString.h"

#include <functional>

#if defined(__CELLOS_LV2__) || defined(__SNC__)
#ifndef _STD_FUNCTION_DEFINED_PS3_
#define _STD_FUNCTION_DEFINED_PS3_
namespace std {
    // minimal C++11 std::function fallback for arity 0 and 1
    template<typename T>
    class function;

    template<typename R, typename Arg1>
    class function<R(Arg1)> {
    public:
        struct Invoker {
            virtual ~Invoker() {}
            virtual R invoke(Arg1) = 0;
            virtual Invoker* clone() const = 0;
        };

        template<typename F>
        struct FunctorInvoker : public Invoker {
            F mFunc;
            FunctorInvoker(const F& f) : mFunc(f) {}
            virtual R invoke(Arg1 a1) { return mFunc(a1); }
            virtual Invoker* clone() const { return new FunctorInvoker(mFunc); }
        };

        function() : mInvoker(nullptr) {}
        template<typename F>
        function(const F& f) : mInvoker(new FunctorInvoker<F>(f)) {}
        function(const function& other) : mInvoker(other.mInvoker ? other.mInvoker->clone() : nullptr) {}
        ~function() { delete mInvoker; }
        function& operator=(const function& other) {
            if (this != &other) {
                delete mInvoker;
                mInvoker = other.mInvoker ? other.mInvoker->clone() : nullptr;
            }
            return *this;
        }
        R operator()(Arg1 a1) const { return mInvoker->invoke(a1); }
        operator bool() const { return mInvoker != nullptr; }

    private:
        Invoker* mInvoker;
    };

    template<typename R>
    class function<R()> {
    public:
        struct Invoker {
            virtual ~Invoker() {}
            virtual R invoke() = 0;
            virtual Invoker* clone() const = 0;
        };

        template<typename F>
        struct FunctorInvoker : public Invoker {
            F mFunc;
            FunctorInvoker(const F& f) : mFunc(f) {}
            virtual R invoke() { return mFunc(); }
            virtual Invoker* clone() const { return new FunctorInvoker(mFunc); }
        };

        function() : mInvoker(nullptr) {}
        template<typename F>
        function(const F& f) : mInvoker(new FunctorInvoker<F>(f)) {}
        function(const function& other) : mInvoker(other.mInvoker ? other.mInvoker->clone() : nullptr) {}
        ~function() { delete mInvoker; }
        function& operator=(const function& other) {
            if (this != &other) {
                delete mInvoker;
                mInvoker = other.mInvoker ? other.mInvoker->clone() : nullptr;
            }
            return *this;
        }
        R operator()() const { return mInvoker->invoke(); }
        operator bool() const { return mInvoker != nullptr; }

    private:
        Invoker* mInvoker;
    };
}
#endif
#endif


namespace lemon
{
	class ControlFlow;
	class Environment;


	class API_EXPORT Variable
	{
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
		inline Type getType() const							 { return mType; }
		inline FlyweightString getName() const				 { return mName; }
		inline uint32 getID() const							 { return mID; }
		inline const DataTypeDefinition* getDataType() const { return mDataType; }

		template<typename T> bool isA() const		{ return getType() == T::TYPE; }
		template<typename T> T& as()				{ return static_cast<T&>(*this); }
		template<typename T> const T& as() const	{ return static_cast<const T&>(*this); }
		template<typename T> T* cast()				{ return isA<T>() ? static_cast<T*>(*this) : nullptr; }
		template<typename T> const T* cast() const	{ return isA<T>() ? static_cast<const T*>(*this) : nullptr; }

	protected:
		inline Variable(Type type) : mType(type) {}
		virtual ~Variable() {}

	private:
		const Type mType;
		FlyweightString mName;
		uint32 mID = 0;
		const DataTypeDefinition* mDataType = nullptr;
	};


	class API_EXPORT LocalVariable : public Variable
	{
	public:
		static const Type TYPE = Type::LOCAL;

	public:
		inline LocalVariable() : Variable(Type::LOCAL) {}

		inline size_t getLocalMemoryOffset() const	{ return mLocalMemoryOffset; }
		inline size_t getLocalMemorySize() const	{ return mLocalMemorySize; }

		// Local variables get accessed via the current state's variables stack

	public:
		size_t mLocalMemoryOffset = 0;
		size_t mLocalMemorySize = 0;
	};


	class API_EXPORT GlobalVariable : public Variable
	{
	public:
		static const Type TYPE = Type::GLOBAL;

	public:
		inline GlobalVariable() : Variable(Type::GLOBAL) {}

		inline size_t getStaticMemoryOffset() const	{ return mStaticMemoryOffset; }
		inline size_t getStaticMemorySize() const	{ return mStaticMemorySize; }

		// Global variables get accessed via the runtime's global variables list

	public:
		AnyBaseValue mInitialValue;
		size_t mStaticMemoryOffset = 0;
		size_t mStaticMemorySize = 0;
	};


	class API_EXPORT UserDefinedVariable : public Variable
	{
	public:
		static const Type TYPE = Type::USER;

	public:
		inline UserDefinedVariable() : Variable(Type::USER) {}

	public:
		// User defined variables get accessed using their getter and setter methods
		//  -> Note that these need to read from / write to the value stack, using the given runtime opcode context
		std::function<void(ControlFlow&)> mGetter;
		std::function<void(ControlFlow&)> mSetter;
	};


	class API_EXPORT ExternalVariable : public Variable
	{
	public:
		static const Type TYPE = Type::EXTERNAL;

	public:
		inline ExternalVariable() : Variable(Type::EXTERNAL) {}

	public:
		// External variables get accessed directly via the accessor the pointer, using the right data type
		std::function<int64*()> mAccessor;
	};

}
