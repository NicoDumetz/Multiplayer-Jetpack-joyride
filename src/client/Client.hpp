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
            uint8_t _playerId;
            std::vector<std::vector<TileType>> _map;
            std::map<uint8_t, std::function<void(const Packet&)>> _packetHandlers = {
                {GAME_STATE, [this](const Packet& paquet) {this->handleGameState(paquet);}},
                {COIN_EVENT, [this](const Packet& paquet) {this->handleCoinEvent(paquet);}},
                {PLAYER_ELIMINATED, [this](const Packet& paquet) {this->handlePlayerEliminated(paquet);}},
                {GAME_OVER, [this](const Packet& paquet) {this->handleGameOver(paquet);}}
            };
    };
}
