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
    this->_connected = true;
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

void Jetpack::Client::connectToServer()
{
    Jetpack::ProtocolUtils::sendPacket(this->_socket, 0x01, {}); // login
    Jetpack::Utils::consoleLog("LOGIN_REQUEST sent.", Jetpack::LogInfo::INFO);

    Jetpack::Packet login = Jetpack::ProtocolUtils::receivePacket(this->_socket); // response
    if (login.type != 0x02 || login.payload.size() != 1)
        throw ClientError("Invalid LOGIN_RESPONSE");

    Jetpack::Packet map = Jetpack::ProtocolUtils::receivePacket(this->_socket); // map
    if (map.type != 0x03)
        throw ClientError("Expected MAP_TRANSFER");
    Jetpack::Utils::consoleLog("Map received (" + std::to_string(map.payload.size()) + " bytes)", Jetpack::LogInfo::INFO);

    Jetpack::Packet start = Jetpack::ProtocolUtils::receivePacket(this->_socket); // start
    if (start.type != 0x04)
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
        while (this->_connected) {
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
        this->_connected = false;
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
    this->_connected = false;
}
