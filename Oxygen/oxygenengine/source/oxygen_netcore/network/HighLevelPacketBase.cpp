/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen_netcore/pch.h"
#include "oxygen_netcore/network/HighLevelPacketBase.h"

#if defined(PLATFORM_PS3)
#include "oxygen_netcore/serverclient/ChannelBroadcastPackets.h"
#include "oxygen_netcore/serverclient/FileTransferPackets.h"
#include "oxygen_netcore/serverclient/Packets.h"

std::unordered_map<uint32, std::string> highlevel::PacketBase::mPacketTypeRegistry;

#define HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(_class_, _name_) \
	const std::string _class_::PACKET_NAME = _name_; \
	highlevel::PacketTypeRegistration _class_::mPacketTypeRegistration { _class_::PACKET_TYPE, _class_::PACKET_NAME };

// network::ChannelErrorPacket
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::ChannelErrorPacket, "ChannelErrorPacket")

// network::BroadcastChannelMessagePacket
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::BroadcastChannelMessagePacket, "BroadcastChannelMessagePacket")

// network::ChannelMessagePacket
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::ChannelMessagePacket, "ChannelMessagePacket")

// network::JoinChannelRequest
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::JoinChannelRequest::Query, "JoinChannelRequest::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::JoinChannelRequest::Response, "JoinChannelRequest::Response")

// network::LeaveChannelRequest
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::LeaveChannelRequest::Query, "LeaveChannelRequest::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::LeaveChannelRequest::Response, "LeaveChannelRequest::Response")

// network::GetChannelContent
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::GetChannelContent::Query, "GetChannelContent::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::GetChannelContent::Response, "GetChannelContent::Response")

// network::GetServerFeaturesRequest
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::GetServerFeaturesRequest::Query, "GetServerFeaturesRequest::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::GetServerFeaturesRequest::Response, "GetServerFeaturesRequest::Response")

// network::AppUpdateCheckRequest
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::AppUpdateCheckRequest::Query, "AppUpdateCheck::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::AppUpdateCheckRequest::Response, "AppUpdateCheck::Response")

// network::FileTransferRequestPiecesPacket
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::FileTransferRequestPiecesPacket, "FileTransferRequestPiecesPacket")

// network::FileTransferPiecePacket
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::FileTransferPiecePacket, "FileTransferPiecePacket")

// network::FileDownloadRequest
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::FileDownloadRequest::Query, "FileDownloadRequest::Query")
HIGHLEVEL_PACKET_IMPLEMENT_PACKET_TYPE(network::FileDownloadRequest::Response, "FileDownloadRequest::Response")

#endif
