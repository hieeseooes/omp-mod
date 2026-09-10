#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#include "vendor/enet/enet/enet.h"

class ENetServer {
public:
    static inline std::unordered_map<uint16_t, std::string> g_vehicleLightStates;

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

        g_serverHost = enet_host_create(&address, 128, 2, 0, 0);
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
                enet_peer_timeout(event.peer, 32, 10000, 30000);
                enet_peer_ping_interval(event.peer, 1000);

                // Sync all current active vehicle light states to newly connected client
                for (const auto& pair : g_vehicleLightStates) {
                    std::string payload = std::to_string(pair.first) + " " + pair.second;
                    size_t len = 1 + payload.length();
                    uint8_t* buf = new uint8_t[len];
                    buf[0] = 226;
                    memcpy(&buf[1], payload.c_str(), payload.length());
                    ENetPacket* initPacket = enet_packet_create(buf, len, ENET_PACKET_FLAG_RELIABLE);
                    delete[] buf;
                    enet_peer_send(event.peer, 0, initPacket);
                }
                enet_host_flush(g_serverHost);
                break;
            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.packet && event.packet->dataLength >= 1) {
                    uint8_t rpcId = event.packet->data[0];
                    if (rpcId == 200) {
                        // Keep-alive heartbeat ping, consume without relaying
                        enet_packet_destroy(event.packet);
                        break;
                    }

                    if (rpcId == 226 && event.packet->dataLength > 1) {
                        std::string msg((char*)&event.packet->data[1], event.packet->dataLength - 1);
                        std::stringstream ss(msg);
                        uint16_t vehId = 0;
                        int indState = 0;
                        int fogState = 0;
                        if (ss >> vehId >> indState >> fogState) {
                            if (indState == 3 && fogState == 0) {
                                g_vehicleLightStates.erase(vehId);
                            } else {
                                g_vehicleLightStates[vehId] = std::to_string(indState) + " " + std::to_string(fogState);
                            }
                        }
                    }

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
