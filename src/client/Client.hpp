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
#include "GameShared/GameShared.hpp"
#include "Visuals/Coin/Coin.hpp"

/******************************************************************************/
/*                                                                            */
/*                               CLIENT SIDE                                  */
/*                                                                            */
/******************************************************************************/

namespace Jetpack {
    enum class ClientState {
        Disconnected,
        Waiting,
        Connected,
        GameOver
    };
    class Client {
        public:
            class ClientError : public Jetpack::Error {
                public:
                    ClientError(const std::string &message) : Jetpack::Error(message) {}
                    ~ClientError() = default;
            };
        public:
            Client(const Jetpack::Parser &args, bool debug);
            ~Client();

            void run();
            ClientState getState() const {return this->_state;}
            uint8_t getExpectedPlayerCount() const {return this->_numberClients;}
            int getSocket() const {return this->_socket;}
            uint8_t getPlayerId () const {return this->_playerId;}
            void handshakeWithServer();
            void waitForGameStart();

            void handleGameState(const Jetpack::Packet &paquet);
            void handlePositionUpdate(const Jetpack::Packet &paquet);

            void handleCoinEvent(const Jetpack::Packet &paquet);
            void handlePlayerEliminated(const Jetpack::Packet &paquet);
            void handleGameOver(const Jetpack::Packet &paquet);
            inline std::shared_ptr<SharedGameState> getSharedState() const {return this->_sharedState;}
            void sendJump();
            void handleActionAck(const Jetpack::Packet &paquet);
            inline const std::vector<std::vector<TileType>>& getMap() const {return this->_map;}
            void handleMap(const Jetpack::Packet &paquet);
            void disconnect();
            uint8_t getGameOverWinnerId() const { return _gameOverWinnerId; }

        private:
            int _socket;
            ClientState _state;
            uint8_t _playerId;
            bool _debug;
            uint8_t _numberClients = 0;
            uint8_t _gameOverWinnerId = 0;
            std::vector<std::vector<TileType>> _map;
            std::map<uint8_t, std::function<void(const Packet&)>> _packetHandlers = {
                {GAME_STATE, [this](const Packet &paquet) {this->handleGameState(paquet);}},
                {COIN_EVENT, [this](const Packet &paquet) {this->handleCoinEvent(paquet);}},
                {POSITION_UPDATE, [this](const Packet &paquet) {this->handlePositionUpdate(paquet);}},
                {PLAYER_ELIMINATED, [this](const Packet &paquet) {this->handlePlayerEliminated(paquet);}},
                {GAME_OVER, [this](const Packet &paquet) {this->handleGameOver(paquet);}},
                {ACTION_ACK, [this](const Packet &paquet) {this->handleActionAck(paquet);}},
                {MAP_TRANSFER, [this](const Packet &paquet) {this->handleMap(paquet);}}
            };
            std::shared_ptr<SharedGameState> _sharedState;
    };
}
