/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "lemon/program/function/NativeFunction.h"
#include "lemon/runtime/Runtime.h"
#include "lemon/utility/AnyBaseValue.h"


namespace lemon
{

	struct AnyTypeWrapper
	{
		AnyBaseValue mValue;
		const DataTypeDefinition* mType = nullptr;

		inline AnyTypeWrapper() {}
		explicit AnyTypeWrapper(uint64 value);

		void pushToStack(ControlFlow& controlFlow) const;
		void popFromStack(ControlFlow& controlFlow);
		void readFromStack(ControlFlow& controlFlow);
	};


	namespace traits
	{
		template<typename T> const DataTypeDefinition* getDataType()  { return T::UNKNOWN_TYPE; }
		template<> const DataTypeDefinition* getDataType<void>();
		template<> const DataTypeDefinition* getDataType<bool>();
		template<> const DataTypeDefinition* getDataType<int8>();
		template<> const DataTypeDefinition* getDataType<uint8>();
		template<> const DataTypeDefinition* getDataType<int16>();
		template<> const DataTypeDefinition* getDataType<uint16>();
		template<> const DataTypeDefinition* getDataType<int32>();
		template<> const DataTypeDefinition* getDataType<uint32>();
		template<> const DataTypeDefinition* getDataType<int64>();
		template<> const DataTypeDefinition* getDataType<uint64>();
		template<> const DataTypeDefinition* getDataType<float>();
		template<> const DataTypeDefinition* getDataType<double>();
		template<> const DataTypeDefinition* getDataType<StringRef>();
		template<> const DataTypeDefinition* getDataType<ArrayBaseWrapper>();
		template<> const DataTypeDefinition* getDataType<AnyTypeWrapper>();
	}


	namespace internal
	{

		// Stack interactions templates for base types - these functions are used as a basis for return type and parameter type handling

		template<typename R>
		void pushStackGeneric(R value, const NativeFunction::Context context)
		{
			context.mControlFlow.pushValueStack<R>(value);
		};

		template<typename T>
		T popStackGeneric(const NativeFunction::Context context)
		{
			return static_cast<T>(context.mControlFlow.popValueStack<T>());
		}



		// Template specializations for StringRef, representing the "string" type in script

		template<>
		void pushStackGeneric<StringRef>(StringRef value, const NativeFunction::Context context);

		template<>
		StringRef popStackGeneric(const NativeFunction::Context context);



		// Template specializations for ArrayBaseWrapper, representing all array types in script

		template<>
		void pushStackGeneric<ArrayBaseWrapper>(ArrayBaseWrapper value, const NativeFunction::Context context);

		template<>
		ArrayBaseWrapper popStackGeneric(const NativeFunction::Context context);



		// Template specializations for AnyTypeWrapper, representing the "any" type in script

		template<>
		void pushStackGeneric<AnyTypeWrapper>(AnyTypeWrapper value, const NativeFunction::Context context);

		template<>
		AnyTypeWrapper popStackGeneric(const NativeFunction::Context context);
	}



#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	namespace internal
	{
		// Parameter builder helper class

		template<typename CLASS, bool WITH_CONTEXT, typename... Tuple>
		struct ParameterBuilder
		{
			using ClassPart   = std::conditional_t<std::is_void_v<CLASS>, std::tuple<>, std::tuple<CLASS*>>;
			using ContextPart = std::conditional_t<WITH_CONTEXT, std::tuple<const NativeFunction::Context*>, std::tuple<>>;
			using FullTuple   = decltype(std::tuple_cat(std::declval<ClassPart>(), std::declval<ContextPart>(), std::declval<std::tuple<Tuple...>>()));

			FullTuple mTuple;
			const NativeFunction::Context& mContext;

			ParameterBuilder(const NativeFunction::Context& context) : mContext(context)
			{
				if constexpr (WITH_CONTEXT)
				{
					std::get<0>(mTuple) = &context;
				}
			}

			ParameterBuilder(const NativeFunction::Context& context, CLASS* object) : mContext(context)
			{
				if constexpr (std::is_void_v<CLASS>)
				{
					if constexpr (WITH_CONTEXT)
						std::get<0>(mTuple) = &context;
				}
				else
				{
					std::get<0>(mTuple) = object;
					if constexpr (WITH_CONTEXT)
						std::get<1>(mTuple) = &context;
				}
			}

			template<typename T, typename... Args>
			void popStackInReverseOrder()
			{
				// Recursive call right at the start, so we get the reverse order
				if constexpr (sizeof...(Args) > 0)
				{
					popStackInReverseOrder<Args...>();
				}

				// Pop from stack
				constexpr size_t numFixedEntries = (std::is_void_v<CLASS> ? 0 : 1) + (WITH_CONTEXT ? 1 : 0);
				constexpr size_t index = sizeof...(Tuple) - sizeof...(Args) - 1 + numFixedEntries;
				std::get<index>(mTuple) = popStackGeneric<T>(mContext);
			}
		};



		// Function wrappers

		template<bool WITH_CONTEXT, typename R, typename... Args>
		class FunctionWrapper : public NativeFunction::FunctionWrapper
		{
		public:
			using Pointer = std::conditional_t<WITH_CONTEXT,
											   R(*)(const NativeFunction::Context*, Args...),
											   R(*)(Args...) >;

			explicit FunctionWrapper(Pointer pointer) : mPointer(pointer) {}

		protected:
			void execute(const NativeFunction::Context context) const override
			{
				ParameterBuilder<void, WITH_CONTEXT, Args...> parameters(context);
				if constexpr (sizeof...(Args) > 0)
				{
					parameters.template popStackInReverseOrder<Args...>();
				}

				if constexpr (std::is_void_v<R>)
				{
					std::apply(mPointer, parameters.mTuple);
				}
				else
				{
					pushStackGeneric(std::apply(mPointer, parameters.mTuple), context);
				}
			}

			virtual const DataTypeDefinition* getReturnType() const override
			{
				return traits::getDataType<R>();
			}

			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override
			{
				return { traits::getDataType<Args>()... };
			}

		private:
			Pointer mPointer;
		};



		// Class method wrapper

		template<bool WITH_CONTEXT, typename CLASS, typename R, typename... Args>
		class MethodWrapper : public NativeFunction::FunctionWrapper
		{
		public:
			using Pointer = std::conditional_t<WITH_CONTEXT,
											   R(CLASS::*)(const NativeFunction::Context*, Args...),
											   R(CLASS::*)(Args...) >;

			explicit MethodWrapper(CLASS& object, Pointer pointer) : mObject(object), mPointer(pointer) {}

		protected:
			void execute(const NativeFunction::Context context) const override
			{
				ParameterBuilder<CLASS, WITH_CONTEXT, Args...> parameters(context, &mObject);
				if constexpr (sizeof...(Args) > 0)
				{
					parameters.template popStackInReverseOrder<Args...>();
				}

				if constexpr (std::is_void_v<R>)
				{
					std::apply(mPointer, parameters.mTuple);
				}
				else
				{
					pushStackGeneric(std::apply(mPointer, parameters.mTuple), context);
				}
			}

			virtual const DataTypeDefinition* getReturnType() const override
			{
				return traits::getDataType<R>();
			}

			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override
			{
				return { traits::getDataType<Args>()... };
			}

		private:
			CLASS& mObject;
			Pointer mPointer;
		};

	}


	// --- Function wrapper functionality ---

	template<typename R>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)())
	{
		return *new internal::FunctionWrapper<false, R>(pointer);
	}

	template<typename R>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*))
	{
		return *new internal::FunctionWrapper<true, R>(pointer);
	}

	template<typename R, typename... Args>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(Args...))
	{
		return *new internal::FunctionWrapper<false, R, Args...>(pointer);
	}

	template<typename R, typename... Args>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*, Args...))
	{
		return *new internal::FunctionWrapper<true, R, Args...>(pointer);
	}


	template<typename CLASS, typename R>
	static NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)())
	{
		return *new internal::MethodWrapper<false, CLASS, R>(object, pointer);
	}

	template<typename CLASS, typename R>
	static NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(const NativeFunction::Context*))
	{
		return *new internal::MethodWrapper<true, CLASS, R>(object, pointer);
	}

	template<typename CLASS, typename R, typename... Args>
	static NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(Args...))
	{
		return *new internal::MethodWrapper<false, CLASS, R, Args...>(object, pointer);
	}

	template<typename CLASS, typename R, typename... Args>
	static NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(const NativeFunction::Context*, Args...))
	{
		return *new internal::MethodWrapper<true, CLASS, R, Args...>(object, pointer);
	}
#else
	namespace internal
	{
		// 0 script args, no context
		template<typename R>
		class FunctionWrapper0 : public NativeFunction::FunctionWrapper {
			R(*mPointer)();
		public:
			explicit FunctionWrapper0(R(*pointer)()) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				pushStackGeneric(mPointer(), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				return std::vector<const DataTypeDefinition*>();
			}
		};

		// 1 script arg, no context
		template<typename R, typename A1>
		class FunctionWrapper1 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1);
		public:
			explicit FunctionWrapper1(R(*pointer)(A1)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		// 2 script args, no context
		template<typename R, typename A1, typename A2>
		class FunctionWrapper2 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2);
		public:
			explicit FunctionWrapper2(R(*pointer)(A1, A2)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				return res;
			}
		};

		// 3 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3>
		class FunctionWrapper3 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3);
		public:
			explicit FunctionWrapper3(R(*pointer)(A1, A2, A3)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				return res;
			}
		};

		// 3 script args void, no context
		template<typename A1, typename A2, typename A3>
		class FunctionWrapper3Void : public NativeFunction::FunctionWrapper {
			void(*mPointer)(A1, A2, A3);
		public:
			explicit FunctionWrapper3Void(void(*pointer)(A1, A2, A3)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				mPointer(a1, a2, a3);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				return res;
			}
		};

		// 1 script arg + context
		template<typename R, typename A1>
		class FunctionWrapper1Context : public NativeFunction::FunctionWrapper {
			R(*mPointer)(const NativeFunction::Context*, A1);
		public:
			explicit FunctionWrapper1Context(R(*pointer)(const NativeFunction::Context*, A1)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(&context, a1), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		// 2 script args + context
		template<typename R, typename A1, typename A2>
		class FunctionWrapper2Context : public NativeFunction::FunctionWrapper {
			R(*mPointer)(const NativeFunction::Context*, A1, A2);
		public:
			explicit FunctionWrapper2Context(R(*pointer)(const NativeFunction::Context*, A1, A2)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(&context, a1, a2), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				return res;
			}
		};

		// 3 script args void + context
		template<typename A1, typename A2, typename A3>
		class FunctionWrapper3ContextVoid : public NativeFunction::FunctionWrapper {
			void(*mPointer)(const NativeFunction::Context*, A1, A2, A3);
		public:
			explicit FunctionWrapper3ContextVoid(void(*pointer)(const NativeFunction::Context*, A1, A2, A3)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				mPointer(&context, a1, a2, a3);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				return res;
			}
		};

		// 4 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4>
		class FunctionWrapper4 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4);
		public:
			explicit FunctionWrapper4(R(*pointer)(A1, A2, A3, A4)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				return res;
			}
		};

		// 5 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		class FunctionWrapper5 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5);
		public:
			explicit FunctionWrapper5(R(*pointer)(A1, A2, A3, A4, A5)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4, a5), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				res.push_back(traits::getDataType<A5>());
				return res;
			}
		};

		// 6 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		class FunctionWrapper6 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6);
		public:
			explicit FunctionWrapper6(R(*pointer)(A1, A2, A3, A4, A5, A6)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4, a5, a6), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				res.push_back(traits::getDataType<A5>());
				res.push_back(traits::getDataType<A6>());
				return res;
			}
		};

		// 7 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
		class FunctionWrapper7 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6, A7);
		public:
			explicit FunctionWrapper7(R(*pointer)(A1, A2, A3, A4, A5, A6, A7)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A7 a7 = popStackGeneric<A7>(context);
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4, a5, a6, a7), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				res.push_back(traits::getDataType<A5>());
				res.push_back(traits::getDataType<A6>());
				res.push_back(traits::getDataType<A7>());
				return res;
			}
		};

		// 8 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
		class FunctionWrapper8 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6, A7, A8);
		public:
			explicit FunctionWrapper8(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A8 a8 = popStackGeneric<A8>(context);
				A7 a7 = popStackGeneric<A7>(context);
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4, a5, a6, a7, a8), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				res.push_back(traits::getDataType<A5>());
				res.push_back(traits::getDataType<A6>());
				res.push_back(traits::getDataType<A7>());
				res.push_back(traits::getDataType<A8>());
				return res;
			}
		};

		// 9 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
		class FunctionWrapper9 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
		public:
			explicit FunctionWrapper9(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A9 a9 = popStackGeneric<A9>(context);
				A8 a8 = popStackGeneric<A8>(context);
				A7 a7 = popStackGeneric<A7>(context);
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric(mPointer(a1, a2, a3, a4, a5, a6, a7, a8, a9), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				res.push_back(traits::getDataType<A5>());
				res.push_back(traits::getDataType<A6>());
				res.push_back(traits::getDataType<A7>());
				res.push_back(traits::getDataType<A8>());
				res.push_back(traits::getDataType<A9>());
				return res;
			}
		};
	}

	// 0 script args, no context
	template<typename R>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)()) {
		return *new internal::FunctionWrapper0<R>(pointer);
	}

	// 1 script arg, no context
	template<typename R, typename A1>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1)) {
		return *new internal::FunctionWrapper1<R, A1>(pointer);
	}

	// 2 script args, no context
	template<typename R, typename A1, typename A2>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2)) {
		return *new internal::FunctionWrapper2<R, A1, A2>(pointer);
	}

	// 3 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3)) {
		return *new internal::FunctionWrapper3<R, A1, A2, A3>(pointer);
	}

	// 3 script args void, no context
	template<typename A1, typename A2, typename A3>
	static NativeFunction::FunctionWrapper& wrap(void(*pointer)(A1, A2, A3)) {
		return *new internal::FunctionWrapper3Void<A1, A2, A3>(pointer);
	}

	// 1 script arg + context
	template<typename R, typename A1>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*, A1)) {
		return *new internal::FunctionWrapper1Context<R, A1>(pointer);
	}

	// 2 script args + context
	template<typename R, typename A1, typename A2>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*, A1, A2)) {
		return *new internal::FunctionWrapper2Context<R, A1, A2>(pointer);
	}

	// 3 script args void + context
	template<typename A1, typename A2, typename A3>
	static NativeFunction::FunctionWrapper& wrap(void(*pointer)(const NativeFunction::Context*, A1, A2, A3)) {
		return *new internal::FunctionWrapper3ContextVoid<A1, A2, A3>(pointer);
	}

	// 4 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4)) {
		return *new internal::FunctionWrapper4<R, A1, A2, A3, A4>(pointer);
	}

	// 5 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5)) {
		return *new internal::FunctionWrapper5<R, A1, A2, A3, A4, A5>(pointer);
	}

	// 6 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6)) {
		return *new internal::FunctionWrapper6<R, A1, A2, A3, A4, A5, A6>(pointer);
	}

	// 7 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7)) {
		return *new internal::FunctionWrapper7<R, A1, A2, A3, A4, A5, A6, A7>(pointer);
	}

	// 8 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8)) {
		return *new internal::FunctionWrapper8<R, A1, A2, A3, A4, A5, A6, A7, A8>(pointer);
	}

	// 9 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	static NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9)) {
		return *new internal::FunctionWrapper9<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(pointer);
	}
#endif

}
