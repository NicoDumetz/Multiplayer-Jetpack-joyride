/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myJetpack-nicolas.dumetz
** File description:
** Client
*/

#pragma once

#include <string>
#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include "Utils/Utils.hpp"
#include "Parser/Parser.hpp"
#include "Network/Network.hpp"
#include <functional>
#include "SocketAddress/SocketAddress.hpp"
#include "Protocole/Protocole.hpp"


/******************************************************************************/
/*                                                                            */
/*                               CLIENT SIDE                                  */
/*                                                                            */
/******************************************************************************/

namespace Jetpack {
    enum class ClientState {
        Disconnected,
        Waiting,
        Connected
    };
    class Client {
        public:
            class ClientError : public Jetpack::Error {
                public:
                    ClientError(const std::string &message) : Jetpack::Error(message) {}
                    ~ClientError() = default;
            };
        public:
            Client(const Jetpack::Parser &args);
            ~Client();

            void run();
            int getSocket() const {return this->_socket;}
            void connectToServer();
            void parseMapPayload(const std::vector<uint8_t> &payload);

            void handleGameState(const Jetpack::Packet &paquet);
            void handleCoinEvent(const Jetpack::Packet &paquet);
            void handlePlayerEliminated(const Jetpack::Packet &paquet);
            void handleGameOver(const Jetpack::Packet &paquet);

        private:
            int _socket;
            ClientState _state;
            std::vector<std::vector<TileType>> _map;
            std::map<uint8_t, std::function<void(const Packet&)>> _packetHandlers = {
                {0x06, [this](const Packet& paquet) {this->handleGameState(paquet);}},
                {0x07, [this](const Packet& paquet) {this->handleCoinEvent(paquet);}},
                {0x08, [this](const Packet& paquet) {this->handlePlayerEliminated(paquet);}},
                {0x09, [this](const Packet& paquet) {this->handleGameOver(paquet);}}
            };
    };
}
