/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Protocole
*/

#pragma once
#include "Utils/Utils.hpp"

namespace Jetpack {
    class ProtocolUtils {
        public:
            static void sendPacket(int socket, uint8_t type, const std::vector<uint8_t>& payload, bool debug);
            static Jetpack::Packet receivePacket(int socket, bool debug);
    };
}