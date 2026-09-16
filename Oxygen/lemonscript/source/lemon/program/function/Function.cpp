/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/program/function/Function.h"


namespace lemon
{
	namespace detail
	{
		uint32 getVoidSignatureHash()
		{
			uint32 value = PredefinedDataTypes::VOID.getDataTypeHash();
#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__) || defined(__PPU__) || defined(__ORDER_BIG_ENDIAN__)
			value = rmx::swapBytes(value);
#endif
			return rmx::getFNV1a_32((const uint8*)&value, sizeof(uint32));
		}
	}


	void Function::SignatureBuilder::clear(const DataTypeDefinition& returnType)
	{
		mData.clear();
		mData.push_back(returnType.getDataTypeHash());
	}

	void Function::SignatureBuilder::addParameterType(const DataTypeDefinition& dataType)
	{
		mData.push_back(dataType.getDataTypeHash());
	}

	uint32 Function::SignatureBuilder::getSignatureHash()
	{
#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__) || defined(__PPU__) || defined(__ORDER_BIG_ENDIAN__)
		std::vector<uint32> swappedData = mData;
		for (uint32& v : swappedData)
		{
			v = rmx::swapBytes(v);
		}
		uint32 hash = rmx::getFNV1a_32((const uint8*)&swappedData[0], swappedData.size() * sizeof(uint32));
		while (hash == 0)		// That should be a really rare case anyway
		{
			swappedData.push_back(rmx::swapBytes(0xcd000000u));		// Just add anything to get away from hash 0
			hash = rmx::getFNV1a_32((const uint8*)&swappedData[0], swappedData.size() * sizeof(uint32));
		}
		return hash;
#else
		uint32 hash = rmx::getFNV1a_32((const uint8*)&mData[0], mData.size() * sizeof(uint32));
		while (hash == 0)		// That should be a really rare case anyway
		{
			mData.push_back(0xcd000000);		// Just add anything to get away from hash 0
			hash = rmx::getFNV1a_32((const uint8*)&mData[0], mData.size() * sizeof(uint32));
		}
		return hash;
#endif
	}


	void Function::setParametersByTypes(const std::vector<const DataTypeDefinition*>& parameterTypes)
	{
		mParameters.clear();
		mParameters.resize(parameterTypes.size());
		for (size_t i = 0; i < parameterTypes.size(); ++i)
		{
			mParameters[i].mDataType = parameterTypes[i];
		}
		mSignatureHash = 0;
	}

	uint32 Function::getVoidSignatureHash()
	{
		static const uint32 signatureHash = detail::getVoidSignatureHash();
		return signatureHash;
	}

	uint32 Function::getSignatureHash() const
	{
		if (mSignatureHash == 0)
		{
			static SignatureBuilder builder;
			builder.clear(*mReturnType);
			for (const Parameter& parameter : mParameters)
				builder.addParameterType(*parameter.mDataType);
			mSignatureHash = builder.getSignatureHash();
		}
		return mSignatureHash;
	}

}
