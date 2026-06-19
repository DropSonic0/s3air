/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>


namespace lemon
{

	struct AnyBaseValue
	{
	public:
		inline AnyBaseValue()  {}
		inline explicit AnyBaseValue(int8   value);
		inline explicit AnyBaseValue(uint8  value);
		inline explicit AnyBaseValue(int16  value);
		inline explicit AnyBaseValue(uint16 value);
		inline explicit AnyBaseValue(int32  value);
		inline explicit AnyBaseValue(uint32 value);
		inline explicit AnyBaseValue(int64  value);
		inline explicit AnyBaseValue(uint64 value);
		inline explicit AnyBaseValue(bool   value);
		inline explicit AnyBaseValue(float  value);
		inline explicit AnyBaseValue(double value);

		template<typename T> T get() const;
		template<typename T> void set(T value);

		inline void reset()  { mUint64 = 0; }

		template<typename S, typename T> void cast();

	private:
		union
		{
			uint64 mUint64 = 0;
			float  mFloat;
			double mDouble;
		};
	};


	// --- Specializations of get() ---

	template<> FORCE_INLINE int8   AnyBaseValue::get<int8>() const		{ return (int8)mUint64; }
	template<> FORCE_INLINE uint8  AnyBaseValue::get<uint8>() const		{ return (uint8)mUint64; }
	template<> FORCE_INLINE int16  AnyBaseValue::get<int16>() const		{ return (int16)mUint64; }
	template<> FORCE_INLINE uint16 AnyBaseValue::get<uint16>() const	{ return (uint16)mUint64; }
	template<> FORCE_INLINE int32  AnyBaseValue::get<int32>() const		{ return (int32)mUint64; }
	template<> FORCE_INLINE uint32 AnyBaseValue::get<uint32>() const	{ return (uint32)mUint64; }
	template<> FORCE_INLINE int64  AnyBaseValue::get<int64>() const		{ return (int64)mUint64; }
	template<> FORCE_INLINE uint64 AnyBaseValue::get<uint64>() const	{ return mUint64; }
	template<> FORCE_INLINE bool   AnyBaseValue::get<bool>() const		{ return (mUint64 != 0); }
	template<> FORCE_INLINE float  AnyBaseValue::get<float>() const
	{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		union { uint64 u; float f[2]; } val;
		val.u = mUint64;
		return val.f[1];
#else
		return mFloat;
#endif
	}
	template<> FORCE_INLINE double AnyBaseValue::get<double>() const		{ return mDouble; }
	template<> FORCE_INLINE AnyBaseValue AnyBaseValue::get<AnyBaseValue>() const  { return *this; }


	// --- Specializations of set() ---

	template<> FORCE_INLINE void AnyBaseValue::set<int8>(int8 value)			{ mUint64 = (uint64)(int64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<uint8>(uint8 value)			{ mUint64 = (uint64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<int16>(int16 value)			{ mUint64 = (uint64)(int64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<uint16>(uint16 value)		{ mUint64 = (uint64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<int32>(int32 value)			{ mUint64 = (uint64)(int64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<uint32>(uint32 value)		{ mUint64 = (uint64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<int64>(int64 value)			{ mUint64 = (uint64)value; }
	template<> FORCE_INLINE void AnyBaseValue::set<uint64>(uint64 value)		{ mUint64 = value; }
	template<> FORCE_INLINE void AnyBaseValue::set<bool>(bool value)			{ mUint64 = value ? 1 : 0; }
	template<> FORCE_INLINE void AnyBaseValue::set<float>(float value)
	{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		union { uint64 u; float f[2]; } val;
		val.u = 0;
		val.f[1] = value;
		mUint64 = val.u;
#else
		mUint64 = 0;
		mFloat = value;
#endif
	}
	template<> FORCE_INLINE void AnyBaseValue::set<double>(double value)		{ mDouble = value; }
	template<> FORCE_INLINE void AnyBaseValue::set<AnyBaseValue>(AnyBaseValue value)  { *this = value; }


	// --- Definitions of constructors and other member functions ---

	inline AnyBaseValue::AnyBaseValue(int8   value)  { set<int8>(value); }
	inline AnyBaseValue::AnyBaseValue(uint8  value)  { set<uint8>(value); }
	inline AnyBaseValue::AnyBaseValue(int16  value)  { set<int16>(value); }
	inline AnyBaseValue::AnyBaseValue(uint16 value)  { set<uint16>(value); }
	inline AnyBaseValue::AnyBaseValue(int32  value)  { set<int32>(value); }
	inline AnyBaseValue::AnyBaseValue(uint32 value)  { set<uint32>(value); }
	inline AnyBaseValue::AnyBaseValue(int64  value)  { set<int64>(value); }
	inline AnyBaseValue::AnyBaseValue(uint64 value)  { set<uint64>(value); }
	inline AnyBaseValue::AnyBaseValue(bool   value)  { set<bool>(value); }
	inline AnyBaseValue::AnyBaseValue(float  value)  { set<float>(value); }
	inline AnyBaseValue::AnyBaseValue(double value)  { set<double>(value); }

	template<typename S, typename T> void AnyBaseValue::cast() { set<T>(static_cast<T>(get<S>())); }


	struct BaseTypeConversion
	{
		template<typename S, typename T>
		static FORCE_INLINE T convert(S value)  { return AnyBaseValue(value).get<T>(); }
	};

	template<> FORCE_INLINE int8   BaseTypeConversion::convert(int8 value)		{ return value; }
	template<> FORCE_INLINE uint8  BaseTypeConversion::convert(uint8 value)		{ return value; }
	template<> FORCE_INLINE int16  BaseTypeConversion::convert(int16 value)		{ return value; }
	template<> FORCE_INLINE uint16 BaseTypeConversion::convert(uint16 value)	{ return value; }
	template<> FORCE_INLINE int32  BaseTypeConversion::convert(int32 value)		{ return value; }
	template<> FORCE_INLINE uint32 BaseTypeConversion::convert(uint32 value)	{ return value; }
	template<> FORCE_INLINE int64  BaseTypeConversion::convert(int64 value)		{ return value; }
	template<> FORCE_INLINE uint64 BaseTypeConversion::convert(uint64 value)	{ return value; }
	template<> FORCE_INLINE float  BaseTypeConversion::convert(float value)		{ return value; }
	template<> FORCE_INLINE double BaseTypeConversion::convert(double value)	{ return value; }
	template<> FORCE_INLINE AnyBaseValue BaseTypeConversion::convert(AnyBaseValue value)	{ return value; }

}
