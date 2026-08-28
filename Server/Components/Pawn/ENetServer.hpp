#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#include "vendor/enet/enet/enet.h"

class ENetServer {
public:
    static bool Init(uint16_t port = 7782) {
        static bool initialized = false;
        if (initialized) return true;

        if (enet_initialize() != 0) {
            std::cout << "[ENetServer] Failed to initialize ENet!" << std::endl;
            return false;
        }

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;

        g_serverHost = enet_host_create(&address, 1000, 2, 0, 0);
        if (!g_serverHost) {
            std::cout << "[ENetServer] Failed to create ENet Server Host on port " << port << "!" << std::endl;
            enet_deinitialize();
            return false;
        }

        std::cout << "[ENetServer] SUCCESS! ENet Secondary UDP Server listening on port " << port << std::endl;
        initialized = true;
        return true;
    }

    static void Update() {
        if (!g_serverHost) return;

        ENetEvent event;
        while (enet_host_service(g_serverHost, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "[ENetServer] Client connected to ENet Port 7782!" << std::endl;
                break;
            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.packet && event.packet->dataLength >= 1) {
                    uint8_t rpcId = event.packet->data[0];
                    std::cout << "[ENetServer] Received Client RPC " << (int)rpcId << " (" << event.packet->dataLength << " bytes), relaying to peers..." << std::endl;
                    for (size_t i = 0; i < g_serverHost->peerCount; ++i) {
                        ENetPeer* peer = &g_serverHost->peers[i];
                        if (peer->state == ENET_PEER_STATE_CONNECTED && peer != event.peer) {
                            ENetPacket* fwdPacket = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(peer, 0, fwdPacket);
                        }
                    }
                    enet_host_flush(g_serverHost);
                }
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "[ENetServer] Client disconnected from ENet Port 7782." << std::endl;
                break;
            default:
                break;
            }
        }
    }

    static bool BroadcastRPC(uint8_t rpcId, const std::string& message) {
        if (!g_serverHost) Init();
        if (!g_serverHost) return false;

        size_t len = 1 + message.length();
        std::vector<uint8_t> buffer(len);
        buffer[0] = rpcId;
        if (!message.empty()) {
            memcpy(&buffer[1], message.c_str(), message.length());
        }

        ENetPacket* packet = enet_packet_create(buffer.data(), len, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(g_serverHost, 0, packet);
        enet_host_flush(g_serverHost);

        std::cout << "[ENetServer] Broadcasted RPC " << (int)rpcId << " over ENet Channel 0! Message: " << message << std::endl;
        return true;
    }

private:
    inline static ENetHost* g_serverHost = nullptr;
};
