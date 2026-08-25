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
		}

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
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)())
	{
		return *new internal::FunctionWrapper<false, R>(pointer);
	}

	template<typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*))
	{
		return *new internal::FunctionWrapper<true, R>(pointer);
	}

	template<typename R, typename... Args>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(Args...))
	{
		return *new internal::FunctionWrapper<false, R, Args...>(pointer);
	}

	template<typename R, typename... Args>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(const NativeFunction::Context*, Args...))
	{
		return *new internal::FunctionWrapper<true, R, Args...>(pointer);
	}


	template<typename CLASS, typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)())
	{
		return *new internal::MethodWrapper<false, CLASS, R>(object, pointer);
	}

	template<typename CLASS, typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(const NativeFunction::Context*))
	{
		return *new internal::MethodWrapper<true, CLASS, R>(object, pointer);
	}

	template<typename CLASS, typename R, typename... Args>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(Args...))
	{
		return *new internal::MethodWrapper<false, CLASS, R, Args...>(object, pointer);
	}

	template<typename CLASS, typename R, typename... Args>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(const NativeFunction::Context*, Args...))
	{
		return *new internal::MethodWrapper<true, CLASS, R, Args...>(object, pointer);
	}
#else
	namespace internal
	{
		template<typename R>
		struct CallWrapper {
			static void call(R (*p)(), const NativeFunction::Context context) {
				pushStackGeneric(p(), context);
			}
			template<typename A1>
			static void call(R (*p)(A1), A1 a1, const NativeFunction::Context context) {
				pushStackGeneric(p(a1), context);
			}
			template<typename A1, typename A2>
			static void call(R (*p)(A1, A2), A1 a1, A2 a2, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2), context);
			}
			template<typename A1, typename A2, typename A3>
			static void call(R (*p)(A1, A2, A3), A1 a1, A2 a2, A3 a3, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3), context);
			}
			template<typename A1, typename A2, typename A3, typename A4>
			static void call(R (*p)(A1, A2, A3, A4), A1 a1, A2 a2, A3 a3, A4 a4, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5>
			static void call(R (*p)(A1, A2, A3, A4, A5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6, A7), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6, a7), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6, A7, A8), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6, a7, a8), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6, a7, a8, a9), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), context);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
			static void call(R (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, const NativeFunction::Context context) {
				pushStackGeneric(p(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11), context);
			}
				static void callCtx(R (*p)(const lemon::NativeFunction::Context*), const NativeFunction::Context context) {
					pushStackGeneric(p(&context), context);
				}
			template<typename A1>
			static void callCtx(R (*p)(const lemon::NativeFunction::Context*, A1), A1 a1, const NativeFunction::Context context) {
				pushStackGeneric(p(&context, a1), context);
			}
			template<typename A1, typename A2>
			static void callCtx(R (*p)(const lemon::NativeFunction::Context*, A1, A2), A1 a1, A2 a2, const NativeFunction::Context context) {
				pushStackGeneric(p(&context, a1, a2), context);
			}
		};

		template<>
		struct CallWrapper<void> {
			static void call(void (*p)(), const NativeFunction::Context context) {
				p();
			}
			template<typename A1>
			static void call(void (*p)(A1), A1 a1, const NativeFunction::Context context) {
				p(a1);
			}
			template<typename A1, typename A2>
			static void call(void (*p)(A1, A2), A1 a1, A2 a2, const NativeFunction::Context context) {
				p(a1, a2);
			}
			template<typename A1, typename A2, typename A3>
			static void call(void (*p)(A1, A2, A3), A1 a1, A2 a2, A3 a3, const NativeFunction::Context context) {
				p(a1, a2, a3);
			}
			template<typename A1, typename A2, typename A3, typename A4>
			static void call(void (*p)(A1, A2, A3, A4), A1 a1, A2 a2, A3 a3, A4 a4, const NativeFunction::Context context) {
				p(a1, a2, a3, a4);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5>
			static void call(void (*p)(A1, A2, A3, A4, A5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6, A7), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6, a7);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6, A7, A8), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6, a7, a8);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6, a7, a8, a9);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
			}
			template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
			static void call(void (*p)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, const NativeFunction::Context context) {
				p(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
			}
				static void callCtx(void (*p)(const lemon::NativeFunction::Context*), const NativeFunction::Context context) {
					p(&context);
				}
			template<typename A1>
			static void callCtx(void (*p)(const lemon::NativeFunction::Context*, A1), A1 a1, const NativeFunction::Context context) {
				p(&context, a1);
			}
			template<typename A1, typename A2>
			static void callCtx(void (*p)(const lemon::NativeFunction::Context*, A1, A2), A1 a1, A2 a2, const NativeFunction::Context context) {
				p(&context, a1, a2);
			}
		};

		// MethodWrapper0
		template<typename CLASS, typename R>
		class MethodWrapper0 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)();
		public:
			MethodWrapper0(CLASS& object, R(CLASS::*pointer)()) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				pushStackGeneric((mObject.*mPointer)(), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override { return std::vector<const DataTypeDefinition*>(); }
		};

		template<typename CLASS>
		class MethodWrapper0Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)();
		public:
			MethodWrapper0Void(CLASS& object, void(CLASS::*pointer)()) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				(mObject.*mPointer)();
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override { return std::vector<const DataTypeDefinition*>(); }
		};

		template<typename CLASS, typename R>
		class MethodWrapper0Const : public NativeFunction::FunctionWrapper {
			const CLASS& mObject;
			R(CLASS::*mPointer)() const;
		public:
			MethodWrapper0Const(const CLASS& object, R(CLASS::*pointer)() const) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				pushStackGeneric((mObject.*mPointer)(), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override { return std::vector<const DataTypeDefinition*>(); }
		};

		template<typename CLASS>
		class MethodWrapper0ConstVoid : public NativeFunction::FunctionWrapper {
			const CLASS& mObject;
			void(CLASS::*mPointer)() const;
		public:
			MethodWrapper0ConstVoid(const CLASS& object, void(CLASS::*pointer)() const) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				(mObject.*mPointer)();
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override { return std::vector<const DataTypeDefinition*>(); }
		};

		// MethodWrapper1
		template<typename CLASS, typename R, typename A1>
		class MethodWrapper1 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1);
		public:
			MethodWrapper1(CLASS& object, R(CLASS::*pointer)(A1)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		template<typename CLASS, typename A1>
		class MethodWrapper1Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1);
		public:
			MethodWrapper1Void(CLASS& object, void(CLASS::*pointer)(A1)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		template<typename CLASS, typename R, typename A1>
		class MethodWrapper1Const : public NativeFunction::FunctionWrapper {
			const CLASS& mObject;
			R(CLASS::*mPointer)(A1) const;
		public:
			MethodWrapper1Const(const CLASS& object, R(CLASS::*pointer)(A1) const) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		template<typename CLASS, typename A1>
		class MethodWrapper1ConstVoid : public NativeFunction::FunctionWrapper {
			const CLASS& mObject;
			void(CLASS::*mPointer)(A1) const;
		public:
			MethodWrapper1ConstVoid(const CLASS& object, void(CLASS::*pointer)(A1) const) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				return res;
			}
		};

		// MethodWrapper2
		template<typename CLASS, typename R, typename A1, typename A2>
		class MethodWrapper2 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1, A2);
		public:
			MethodWrapper2(CLASS& object, R(CLASS::*pointer)(A1, A2)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1, a2), context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				return res;
			}
		};

		template<typename CLASS, typename A1, typename A2>
		class MethodWrapper2Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1, A2);
		public:
			MethodWrapper2Void(CLASS& object, void(CLASS::*pointer)(A1, A2)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1, a2);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				return res;
			}
		};

		// MethodWrapper3
		template<typename CLASS, typename R, typename A1, typename A2, typename A3>
		class MethodWrapper3 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1, A2, A3);
		public:
			MethodWrapper3(CLASS& object, R(CLASS::*pointer)(A1, A2, A3)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1, a2, a3), context);
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

		template<typename CLASS, typename A1, typename A2, typename A3>
		class MethodWrapper3Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1, A2, A3);
		public:
			MethodWrapper3Void(CLASS& object, void(CLASS::*pointer)(A1, A2, A3)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1, a2, a3);
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

		// MethodWrapper4
		template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4>
		class MethodWrapper4 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1, A2, A3, A4);
		public:
			MethodWrapper4(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1, a2, a3, a4), context);
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

		template<typename CLASS, typename A1, typename A2, typename A3, typename A4>
		class MethodWrapper4Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1, A2, A3, A4);
		public:
			MethodWrapper4Void(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1, a2, a3, a4);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				std::vector<const DataTypeDefinition*> res;
				res.push_back(traits::getDataType<A1>());
				res.push_back(traits::getDataType<A2>());
				res.push_back(traits::getDataType<A3>());
				res.push_back(traits::getDataType<A4>());
				return res;
			}
		};

		// MethodWrapper5
		template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		class MethodWrapper5 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1, A2, A3, A4, A5);
		public:
			MethodWrapper5(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4, A5)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1, a2, a3, a4, a5), context);
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

		template<typename CLASS, typename A1, typename A2, typename A3, typename A4, typename A5>
		class MethodWrapper5Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1, A2, A3, A4, A5);
		public:
			MethodWrapper5Void(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4, A5)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1, a2, a3, a4, a5);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
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

		// MethodWrapper6
		template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		class MethodWrapper6 : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			R(CLASS::*mPointer)(A1, A2, A3, A4, A5, A6);
		public:
			MethodWrapper6(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4, A5, A6)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				pushStackGeneric((mObject.*mPointer)(a1, a2, a3, a4, a5, a6), context);
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

		template<typename CLASS, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		class MethodWrapper6Void : public NativeFunction::FunctionWrapper {
			CLASS& mObject;
			void(CLASS::*mPointer)(A1, A2, A3, A4, A5, A6);
		public:
			MethodWrapper6Void(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4, A5, A6)) : mObject(object), mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				(mObject.*mPointer)(a1, a2, a3, a4, a5, a6);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<void>(); }
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

	// 0 script args, no context
		template<typename R>
		class FunctionWrapper0 : public NativeFunction::FunctionWrapper {
			R(*mPointer)();
		public:
			explicit FunctionWrapper0(R(*pointer)()) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				CallWrapper<R>::call(mPointer, context);
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
				CallWrapper<R>::call(mPointer, a1, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, context);
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

		// 0 script args + context
		template<typename R>
		class FunctionWrapper0Context : public NativeFunction::FunctionWrapper {
			R(*mPointer)(const lemon::NativeFunction::Context*);
		public:
			explicit FunctionWrapper0Context(R(*pointer)(const lemon::NativeFunction::Context*)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				CallWrapper<R>::callCtx(mPointer, context);
			}
			virtual const DataTypeDefinition* getReturnType() const override { return traits::getDataType<R>(); }
			virtual std::vector<const DataTypeDefinition*> getParameterTypes() const override {
				return std::vector<const DataTypeDefinition*>();
			}
		};

		// 1 script arg + context
		template<typename R, typename A1>
		class FunctionWrapper1Context : public NativeFunction::FunctionWrapper {
			R(*mPointer)(const lemon::NativeFunction::Context*, A1);
		public:
			explicit FunctionWrapper1Context(R(*pointer)(const lemon::NativeFunction::Context*, A1)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A1 a1 = popStackGeneric<A1>(context);
				CallWrapper<R>::callCtx(mPointer, a1, context);
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
			R(*mPointer)(const lemon::NativeFunction::Context*, A1, A2);
		public:
			explicit FunctionWrapper2Context(R(*pointer)(const lemon::NativeFunction::Context*, A1, A2)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				CallWrapper<R>::callCtx(mPointer, a1, a2, context);
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
			void(*mPointer)(const lemon::NativeFunction::Context*, A1, A2, A3);
		public:
			explicit FunctionWrapper3ContextVoid(void(*pointer)(const lemon::NativeFunction::Context*, A1, A2, A3)) : mPointer(pointer) {}
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, a7, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, a7, a8, context);
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
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, a7, a8, a9, context);
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

		// 10 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
		class FunctionWrapper10 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
		public:
			explicit FunctionWrapper10(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A10 a10 = popStackGeneric<A10>(context);
				A9 a9 = popStackGeneric<A9>(context);
				A8 a8 = popStackGeneric<A8>(context);
				A7 a7 = popStackGeneric<A7>(context);
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, context);
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
				res.push_back(traits::getDataType<A10>());
				return res;
			}
		};

		// 11 script args with return, no context
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
		class FunctionWrapper11 : public NativeFunction::FunctionWrapper {
			R(*mPointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
		public:
			explicit FunctionWrapper11(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)) : mPointer(pointer) {}
			void execute(const NativeFunction::Context context) const override {
				A11 a11 = popStackGeneric<A11>(context);
				A10 a10 = popStackGeneric<A10>(context);
				A9 a9 = popStackGeneric<A9>(context);
				A8 a8 = popStackGeneric<A8>(context);
				A7 a7 = popStackGeneric<A7>(context);
				A6 a6 = popStackGeneric<A6>(context);
				A5 a5 = popStackGeneric<A5>(context);
				A4 a4 = popStackGeneric<A4>(context);
				A3 a3 = popStackGeneric<A3>(context);
				A2 a2 = popStackGeneric<A2>(context);
				A1 a1 = popStackGeneric<A1>(context);
				CallWrapper<R>::call(mPointer, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, context);
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
				res.push_back(traits::getDataType<A10>());
				res.push_back(traits::getDataType<A11>());
				return res;
			}
		};
	}

	// 0 script args, no context
	template<typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)()) {
		return *new internal::FunctionWrapper0<R>(pointer);
	}

	// 1 script arg, no context
	template<typename R, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1)) {
		return *new internal::FunctionWrapper1<R, A1>(pointer);
	}

	// 2 script args, no context
	template<typename R, typename A1, typename A2>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2)) {
		return *new internal::FunctionWrapper2<R, A1, A2>(pointer);
	}

	// 3 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3)) {
		return *new internal::FunctionWrapper3<R, A1, A2, A3>(pointer);
	}

	// 3 script args void, no context
	template<typename A1, typename A2, typename A3>
	static lemon::NativeFunction::FunctionWrapper& wrap(void(*pointer)(A1, A2, A3)) {
		return *new internal::FunctionWrapper3Void<A1, A2, A3>(pointer);
	}

	// 0 script args + context
	template<typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(const lemon::NativeFunction::Context*)) {
		return *new internal::FunctionWrapper0Context<R>(pointer);
	}

	// 1 script arg + context
	template<typename R, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(const lemon::NativeFunction::Context*, A1)) {
		return *new internal::FunctionWrapper1Context<R, A1>(pointer);
	}

	// 2 script args + context
	template<typename R, typename A1, typename A2>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(const lemon::NativeFunction::Context*, A1, A2)) {
		return *new internal::FunctionWrapper2Context<R, A1, A2>(pointer);
	}

	// 3 script args void + context
	template<typename A1, typename A2, typename A3>
	static lemon::NativeFunction::FunctionWrapper& wrap(void(*pointer)(const lemon::NativeFunction::Context*, A1, A2, A3)) {
		return *new internal::FunctionWrapper3ContextVoid<A1, A2, A3>(pointer);
	}

	// 4 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4)) {
		return *new internal::FunctionWrapper4<R, A1, A2, A3, A4>(pointer);
	}

	// 5 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5)) {
		return *new internal::FunctionWrapper5<R, A1, A2, A3, A4, A5>(pointer);
	}

	// 6 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6)) {
		return *new internal::FunctionWrapper6<R, A1, A2, A3, A4, A5, A6>(pointer);
	}

	// 7 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7)) {
		return *new internal::FunctionWrapper7<R, A1, A2, A3, A4, A5, A6, A7>(pointer);
	}

	// 8 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8)) {
		return *new internal::FunctionWrapper8<R, A1, A2, A3, A4, A5, A6, A7, A8>(pointer);
	}

	// 9 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9)) {
		return *new internal::FunctionWrapper9<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(pointer);
	}

	// 10 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)) {
		return *new internal::FunctionWrapper10<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(pointer);
	}

	// 11 script args with return, no context
	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	static lemon::NativeFunction::FunctionWrapper& wrap(R(*pointer)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)) {
		return *new internal::FunctionWrapper11<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>(pointer);
	}

	// Method wrap overloads
	template<typename CLASS, typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)()) {
		return *new internal::MethodWrapper0<CLASS, R>(object, pointer);
	}

	template<typename CLASS>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)()) {
		return *new internal::MethodWrapper0Void<CLASS>(object, pointer);
	}

	template<typename CLASS, typename R>
	static lemon::NativeFunction::FunctionWrapper& wrap(const CLASS& object, R(CLASS::*pointer)() const) {
		return *new internal::MethodWrapper0Const<CLASS, R>(object, pointer);
	}

	template<typename CLASS>
	static lemon::NativeFunction::FunctionWrapper& wrap(const CLASS& object, void(CLASS::*pointer)() const) {
		return *new internal::MethodWrapper0ConstVoid<CLASS>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1)) {
		return *new internal::MethodWrapper1<CLASS, R, A1>(object, pointer);
	}

	template<typename CLASS, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1)) {
		return *new internal::MethodWrapper1Void<CLASS, A1>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(const CLASS& object, R(CLASS::*pointer)(A1) const) {
		return *new internal::MethodWrapper1Const<CLASS, R, A1>(object, pointer);
	}

	template<typename CLASS, typename A1>
	static lemon::NativeFunction::FunctionWrapper& wrap(const CLASS& object, void(CLASS::*pointer)(A1) const) {
		return *new internal::MethodWrapper1ConstVoid<CLASS, A1>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1, typename A2>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1, A2)) {
		return *new internal::MethodWrapper2<CLASS, R, A1, A2>(object, pointer);
	}

	template<typename CLASS, typename A1, typename A2>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1, A2)) {
		return *new internal::MethodWrapper2Void<CLASS, A1, A2>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1, typename A2, typename A3>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1, A2, A3)) {
		return *new internal::MethodWrapper3<CLASS, R, A1, A2, A3>(object, pointer);
	}

	template<typename CLASS, typename A1, typename A2, typename A3>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1, A2, A3)) {
		return *new internal::MethodWrapper3Void<CLASS, A1, A2, A3>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4)) {
		return *new internal::MethodWrapper4<CLASS, R, A1, A2, A3, A4>(object, pointer);
	}

	template<typename CLASS, typename A1, typename A2, typename A3, typename A4>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4)) {
		return *new internal::MethodWrapper4Void<CLASS, A1, A2, A3, A4>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4, A5)) {
		return *new internal::MethodWrapper5<CLASS, R, A1, A2, A3, A4, A5>(object, pointer);
	}

	template<typename CLASS, typename A1, typename A2, typename A3, typename A4, typename A5>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4, A5)) {
		return *new internal::MethodWrapper5Void<CLASS, A1, A2, A3, A4, A5>(object, pointer);
	}

	template<typename CLASS, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, R(CLASS::*pointer)(A1, A2, A3, A4, A5, A6)) {
		return *new internal::MethodWrapper6<CLASS, R, A1, A2, A3, A4, A5, A6>(object, pointer);
	}

	template<typename CLASS, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static lemon::NativeFunction::FunctionWrapper& wrap(CLASS& object, void(CLASS::*pointer)(A1, A2, A3, A4, A5, A6)) {
		return *new internal::MethodWrapper6Void<CLASS, A1, A2, A3, A4, A5, A6>(object, pointer);
	}
#endif

}
