#pragma once

#include <bitstream.hpp>
#include <network.hpp>
#include <packet.hpp>
#include <types.hpp>

namespace NetCode
{
namespace RPC
{
	struct RegisterCustomVehicle : NetworkPacketBase<186, NetworkPacketType::RPC, OrderingChannel_SyncRPC>
	{
		int32_t customModelId;
		int32_t baseModelId;
		String zipPath;
		String txdName;

		RegisterCustomVehicle() = default;

		RegisterCustomVehicle(int32_t cId, int32_t bId, StringView zip, StringView txd)
			: customModelId(cId)
			, baseModelId(bId)
			, zipPath(zip)
			, txdName(txd)
		{
		}

		void write(NetworkBitStream& bs) const
		{
			bs.writeINT32(customModelId);
			bs.writeINT32(baseModelId);
			bs.writeDynStr8(zipPath);
			bs.writeDynStr8(txdName);
		}
	};

	struct RegisterCustomPed : NetworkPacketBase<187, NetworkPacketType::RPC, OrderingChannel_SyncRPC>
	{
		int32_t customModelId;
		int32_t baseModelId;
		String zipPath;
		String txdName;

		RegisterCustomPed() = default;

		RegisterCustomPed(int32_t cId, int32_t bId, StringView zip, StringView txd)
			: customModelId(cId)
			, baseModelId(bId)
			, zipPath(zip)
			, txdName(txd)
		{
		}

		void write(NetworkBitStream& bs) const
		{
			bs.writeINT32(customModelId);
			bs.writeINT32(baseModelId);
			bs.writeDynStr8(zipPath);
			bs.writeDynStr8(txdName);
		}
	};

	struct RegisterCustomObject : NetworkPacketBase<188, NetworkPacketType::RPC, OrderingChannel_SyncRPC>
	{
		int32_t customModelId;
		int32_t baseModelId;
		String zipPath;
		String txdName;

		RegisterCustomObject() = default;

		RegisterCustomObject(int32_t cId, int32_t bId, StringView zip, StringView txd)
			: customModelId(cId)
			, baseModelId(bId)
			, zipPath(zip)
			, txdName(txd)
		{
		}

		void write(NetworkBitStream& bs) const
		{
			bs.writeINT32(customModelId);
			bs.writeINT32(baseModelId);
			bs.writeDynStr8(zipPath);
			bs.writeDynStr8(txdName);
		}
	};
}
}
