/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Protocole
*/

#include "Protocole/Protocole.hpp"
#include "Network/Network.hpp"


void Jetpack::ProtocolUtils::sendPacket(int socket, uint8_t type, const std::vector<uint8_t>& payload, bool debug)
{
    size_t totalSent = 0;
    size_t fragmentMax = FRAGMENT_SIZE;
    size_t fragmentSize;
    std::vector<uint8_t> firstFragment;
    uint16_t payloadLen;

    if (debug)
        Jetpack::Utils::consoleLog("Sending packet type 0x" + Jetpack::Utils::toHex(type) + " with payload size: " + std::to_string(payload.size()), Jetpack::LogInfo::INFO);
    if (payload.empty()) {
        std::vector<uint8_t> header = { type, 0, 0 };
        Jetpack::IO::write(socket, header.data(), header.size());
        return;
    }
    firstFragment.push_back(type);
    payloadLen = Jetpack::Network::htons(static_cast<uint16_t>(payload.size()));
    firstFragment.push_back(payloadLen >> 8);
    firstFragment.push_back(payloadLen & 0xFF);
    fragmentSize = std::min(fragmentMax, payload.size());
    firstFragment.insert(firstFragment.end(), payload.begin(), payload.begin() + fragmentSize);
    Jetpack::IO::write(socket, firstFragment.data(), firstFragment.size());
    totalSent += fragmentSize;
    while (totalSent < payload.size()) {
        fragmentSize = std::min(fragmentMax, payload.size() - totalSent);
        std::vector<uint8_t> fragment(payload.begin() + totalSent, payload.begin() + totalSent + fragmentSize);
        Jetpack::IO::write(socket, fragment.data(), fragment.size());
        totalSent += fragmentSize;
    }
    if (debug)
        Jetpack::Utils::consoleLog("Packet sent in " + std::to_string((payload.size() / fragmentMax) + 1) + " fragment(s)", Jetpack::LogInfo::INFO);
}


Jetpack::Packet Jetpack::ProtocolUtils::receivePacket(int socket, bool debug)
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
    if (debug)
        Jetpack::Utils::consoleLog("Receiving packet type 0x" + Jetpack::Utils::toHex(pkt.type) + " with length: " + std::to_string(length), Jetpack::LogInfo::INFO);
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
    if (debug)
        Jetpack::Utils::consoleLog("Packet successfully received with " + std::to_string(pkt.payload.size()) + " bytes", Jetpack::LogInfo::INFO);
    return pkt;
}
