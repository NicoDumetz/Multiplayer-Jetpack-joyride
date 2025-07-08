/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** Client
*/
#include "client/Client.hpp"

/******************************************************************************/
/*                                                                            */
/*                               Constructor                                  */
/*                                                                            */
/******************************************************************************/

Jetpack::Client::Client(const Jetpack::Parser &args)
{
    Jetpack::SocketAddress addr;

    this->_socket = Jetpack::Network::socket(AF_INET, SOCK_STREAM, 0);
    this->_state = ClientState::Disconnected;
    auto *in = reinterpret_cast<sockaddr_in *>(addr.raw());
    in->sin_port = Jetpack::Network::htons(args.getPort());
    Jetpack::Network::inet_pton(AF_INET, args.getIp().c_str(), &in->sin_addr);
    Jetpack::Network::connect(this->_socket, addr.raw(), *addr.lenPtr());
    Jetpack::Utils::consoleLog("Connected to " + args.getIp(), Jetpack::LogInfo::SUCCESS);
}

Jetpack::Client::~Client()
{
    try {
        Jetpack::IO::close(this->_socket);
    } catch (const Jetpack::IO::IOError &e) {
        Jetpack::Utils::consoleLog("Client close failed: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
    }
}

/******************************************************************************/
/*                                                                            */
/*                          Connection                                        */
/*                                                                            */
/******************************************************************************/

void Jetpack::Client::parseMapPayload(const std::vector<uint8_t> &payload)
{
    this->_map.clear();
    std::vector<TileType> row;

    for (uint8_t val : payload) {
        val == '\n' && !row.empty() ? (this->_map.push_back(row), row.clear()) : void();
        val <= static_cast<uint8_t>(TileType::ZAPPER) ? row.push_back(static_cast<TileType>(val)) : void();
    }
    if (!row.empty())
        this->_map.push_back(row);
}

void Jetpack::Client::connectToServer()
{
    Jetpack::Packet login;
    Jetpack::Packet map;

    Jetpack::ProtocolUtils::sendPacket(this->_socket, LOGIN_REQUEST, {});
    login = Jetpack::ProtocolUtils::receivePacket(this->_socket);
    if (login.type != LOGIN_RESPONSE || login.payload.size() != 1)
        throw ClientError("Invalid LOGIN_RESPONSE");
    Jetpack::Utils::consoleLog("Login accepted by server", Jetpack::LogInfo::INFO);
    this->_state = ClientState::Waiting;
    map = Jetpack::ProtocolUtils::receivePacket(this->_socket);
    if (map.type != MAP_TRANSFER)
        throw ClientError("Expected MAP_TRANSFER");
    parseMapPayload(map.payload);
    Jetpack::Utils::consoleLog("Map was successfully receive.", Jetpack::LogInfo::INFO);
    Jetpack::Utils::consoleLog("Waiting game start...", Jetpack::LogInfo::INFO);
    Jetpack::Packet start = Jetpack::ProtocolUtils::receivePacket(this->_socket); // start
    if (start.type != GAME_START)
        throw ClientError("Expected GAME_START");
    Jetpack::Utils::consoleLog("Game is starting!", Jetpack::LogInfo::SUCCESS);
}


/******************************************************************************/
/*                                                                            */
/*                                 Run                                        */
/*                                                                            */
/******************************************************************************/

void Jetpack::Client::run()
{
    try {
        while (this->_state == ClientState::Connected) {
            Jetpack::Packet paquet = Jetpack::ProtocolUtils::receivePacket(this->_socket);

            auto it = this->_packetHandlers.find(paquet.type);
            if (it != this->_packetHandlers.end()) {
                it->second(paquet);
            } else {
                Jetpack::Utils::consoleLog("Unknown packet: 0x" + Jetpack::Utils::toHex(paquet.type), Jetpack::LogInfo::ERROR);
            }
        }
    } catch (const std::exception &e) {
        Jetpack::Utils::consoleLog("Disconnected : " + std::string(e.what()), Jetpack::LogInfo::ERROR);
        this->_state = ClientState::Disconnected;
    }
}

void Jetpack::Client::handleGameState(const Jetpack::Packet &)
{
    Jetpack::Utils::consoleLog("GAME_STATE", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handleCoinEvent(const Jetpack::Packet &)
{
    Jetpack::Utils::consoleLog("COIN_EVENT", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handlePlayerEliminated(const Jetpack::Packet &)
{
    Jetpack::Utils::consoleLog("PLAYER_ELIMINATED", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handleGameOver(const Jetpack::Packet &)
{
    Jetpack::Utils::consoleLog("GAME_OVER", Jetpack::LogInfo::SUCCESS);
    this->_state = ClientState::Disconnected;
}
