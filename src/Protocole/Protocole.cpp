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
    size_t totalSent = 0;
    size_t fragmentMax = FRAGMENT_SIZE;
    size_t fragmentSize;
    std::vector<uint8_t> buffer;
    uint16_t fragmentLength;

    if (payload.empty()) {
        buffer.push_back(type);
        buffer.push_back(0);
        buffer.push_back(0);
        Jetpack::IO::write(socket, buffer.data(), buffer.size());
        return;
    }
    while (totalSent < payload.size()) {
        fragmentSize = std::min(fragmentMax, payload.size() - totalSent);
        buffer.push_back(type);
        fragmentLength = Jetpack::Network::htons(fragmentSize);
        buffer.push_back(fragmentLength >> 8);
        buffer.push_back(fragmentLength & 0xFF);
        if (fragmentSize > 0)
            buffer.insert(buffer.end(), payload.begin() + totalSent, payload.begin() + totalSent + fragmentSize);
        Jetpack::IO::write(socket, buffer.data(), buffer.size());
        totalSent += fragmentSize;
    }
}

Jetpack::Packet Jetpack::ProtocolUtils::receivePacket(int socket)
{
    Jetpack::Packet pkt;
    uint8_t header[3];
    uint16_t length;
    size_t totalReceived = 0;
    size_t fragmentMax = FRAGMENT_SIZE;
    ssize_t bytesRead;
    size_t fragmentSize;

    if (Jetpack::IO::read(socket, header, 3) != 3)
        throw Jetpack::Error("Failed to read packet header");
    pkt.type = header[0];
    length = Jetpack::Network::ntohs((header[1] << 8) + header[2]);
    if (length > fragmentMax) {
        while (totalReceived < length) {
            fragmentSize = std::min(fragmentMax, length - totalReceived);
            std::vector<uint8_t> fragment(fragmentSize);
            bytesRead = Jetpack::IO::read(socket, fragment.data(), fragment.size());
            if (static_cast<size_t>(bytesRead) != fragment.size())
                throw Jetpack::Error("Failed to read full payload fragment");
            pkt.payload.insert(pkt.payload.end(), fragment.begin(), fragment.end());
            totalReceived += bytesRead;
        }
    } else {
        pkt.payload.resize(length);
        bytesRead = Jetpack::IO::read(socket, pkt.payload.data(), length);
        if (static_cast<size_t>(bytesRead) != pkt.payload.size())
            throw Jetpack::Error("Failed to read full payload");
    }
    return pkt;
}