/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Protocole
*/

#include "Protocole/Protocole.hpp"
#include "Network/Network.hpp"

void Jetpack::ProtocolUtils::sendPacket(int socket, uint8_t type, const std::vector<uint8_t>& payload)
{
    uint16_t length = Jetpack::Network::htons(payload.size());
    std::vector<uint8_t> buffer;

    buffer.push_back(type);
    buffer.push_back(length >> 8);
    buffer.push_back(length & 0xFF);
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    Jetpack::IO::write(socket, buffer.data(), buffer.size());
}

Jetpack::Packet Jetpack::ProtocolUtils::receivePacket(int socket)
{
    Jetpack::Packet pkt;
    uint8_t header[3];
    uint16_t length;

    if (Jetpack::IO::read(socket, header, 3) != 3)
        throw Jetpack::Error("Failed to read packet header");
    pkt.type = header[0];
    length = Jetpack::Network::ntohs((header[1] << 8) + header[2]);
    if (length > 0) {
        pkt.payload.resize(length);
        if (Jetpack::IO::read(socket, pkt.payload.data(), length) != length)
            throw Jetpack::Error("Failed to read full payload");
    }
    return pkt;
}