/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "oxygen_netcore/network/RequestBase.h"


// Packets for communication between Oxygen server and client
namespace network
{

	class JoinChannelRequest : public highlevel::RequestBase
	{
		struct QueryData
		{
			uint32 mChannelHash = 0;
			std::string mChannelName;

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
				serializer.serialize(mChannelHash);
				serializer.serialize(mChannelName, 0xff);
			}
		};

		struct ResponseData
		{
			bool mSuccessful = true;

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
				serializer.serialize(mSuccessful);
			}
		};

#if defined(PLATFORM_PS3)
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY_PS3("JoinChannelRequest", 0xb06cc680, 0x26b03223)
#else
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY("JoinChannelRequest")
#endif
	};


	class LeaveChannelRequest : public highlevel::RequestBase
	{
		struct QueryData
		{
			uint32 mChannelHash = 0;

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
				serializer.serialize(mChannelHash);
			}
		};

		struct ResponseData
		{
			// No response data

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
			}
		};

#if defined(PLATFORM_PS3)
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY_PS3("LeaveChannelRequest", 0x75fb6df5, 0x8ea523bc)
#else
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY("LeaveChannelRequest")
#endif
	};


	class GetChannelContent : public highlevel::RequestBase
	{
		struct QueryData
		{
			uint32 mChannelHash = 0;

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
				serializer.serialize(mChannelHash);
			}
		};

		struct ResponseData
		{
			bool mSuccess = true;
			struct PlayerInfo
			{
				uint32 mPlayerID = 0;
				std::vector<uint8> mChannelReplicatedData;
			};
			std::vector<PlayerInfo> mPlayers;

			inline void serializeData(VectorBinarySerializer& serializer, uint8 protocolVersion)
			{
				serializer.serialize(mSuccess);
				if (mSuccess)
				{
					serializer.serializeArraySize(mPlayers, 16);
					for (PlayerInfo& player : mPlayers)
					{
						serializer.serialize(player.mPlayerID);
						serializer.serializeData(player.mChannelReplicatedData, 0x400);
					}
				}
			}
		};

#if defined(PLATFORM_PS3)
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY_PS3("GetChannelContent", 0x8805d70c, 0xb4dc5287)
#else
		HIGHLEVEL_REQUEST_DEFINE_FUNCTIONALITY("GetChannelContent")
#endif
	};


	// Sent from server to client
	struct ChannelErrorPacket : public highlevel::PacketBase
	{
#if defined(PLATFORM_PS3)
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE_PS3("ChannelErrorPacket", 0x176a74b3);
#else
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE("ChannelErrorPacket");
#endif

		enum class ErrorCode : uint8
		{
			UNKNOWN,
			UNKNOWN_CHANNEL,
			CHANNEL_NOT_JOINED
		};

		ErrorCode mErrorCode = ErrorCode::UNKNOWN;
		uint32 mParameter = 0;

		virtual void serializeContent(VectorBinarySerializer& serializer, uint8 protocolVersion) override
		{
			serializer.serializeAs<uint8>(mErrorCode);
			serializer.serialize(mParameter);
		}
	};


	// Sent from client to server
	struct BroadcastChannelMessagePacket : public highlevel::PacketBase
	{
#if defined(PLATFORM_PS3)
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE_PS3("BroadcastChannelMessagePacket", 0xae313eb9);
#else
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE("BroadcastChannelMessagePacket");
#endif

		bool mIsReplicatedData = false;		// If true, this is replicated data that gets cached on the server; otherwise it's just a message to broadcast
		uint32 mChannelHash = 0;
		uint32 mMessageType = 0;
		uint8 mMessageVersion = 0;
		std::vector<uint8> mMessage;

		virtual void serializeContent(VectorBinarySerializer& serializer, uint8 protocolVersion) override
		{
			serializer.serialize(mIsReplicatedData);
			serializer.serialize(mChannelHash);
			serializer.serialize(mMessageType);
			serializer.serialize(mMessageVersion);
			serializer.serializeData(mMessage, 0x400);
		}
	};


	// Sent from server to multiple clients
	struct ChannelMessagePacket : public BroadcastChannelMessagePacket
	{
#if defined(PLATFORM_PS3)
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE_PS3("ChannelMessagePacket", 0x3b8b81be);
#else
		HIGHLEVEL_PACKET_DEFINE_PACKET_TYPE("ChannelMessagePacket");
#endif

		// All properties from "BroadcastChannelMessagePacket" are shared here
		uint32 mSendingPlayerID = 0;

		virtual void serializeContent(VectorBinarySerializer& serializer, uint8 protocolVersion) override
		{
			BroadcastChannelMessagePacket::serializeContent(serializer, protocolVersion);
			serializer.serialize(mSendingPlayerID);
		}
	};

}
