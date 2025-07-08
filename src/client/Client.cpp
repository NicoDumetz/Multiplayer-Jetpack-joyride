/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** Client
*/
#include "client/Client.hpp"
#include "Utils/Utils.hpp"
#include <cstddef>


/******************************************************************************/
/*                                                                            */
/*                               Constructor                                  */
/*                                                                            */
/******************************************************************************/

Jetpack::Client::Client(const Jetpack::Parser &args, bool debug)
{
    Jetpack::SocketAddress addr;

    this->_debug = debug;
    this->_ACKPlayerAction = false;
    this->_socket = Jetpack::Network::socket(AF_INET, SOCK_STREAM, 0);
    this->_state = ClientState::Disconnected;
    auto *in = reinterpret_cast<sockaddr_in *>(addr.raw());
    in->sin_port = Jetpack::Network::htons(args.getPort());
    Jetpack::Network::inet_pton(AF_INET, args.getIp().c_str(), &in->sin_addr);
    Jetpack::Network::connect(this->_socket, addr.raw(), *addr.lenPtr());
    Jetpack::Utils::consoleLog("Connected to " + args.getIp(), Jetpack::LogInfo::SUCCESS);
    this->_sharedState = std::make_shared<SharedGameState>();
}

Jetpack::Client::~Client()
{
    try {
        Jetpack::IO::close(this->_socket);
    } catch (const Jetpack::IO::IOError &e) {
        Jetpack::Utils::consoleLog("Client close failed: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
    }
}

void Jetpack::Client::disconnect()
{
    try {
        Jetpack::IO::closeSocket(_socket);
    } catch (...) {}
    _state = ClientState::Disconnected;
}

/******************************************************************************/
/*                                                                            */
/*                          Connection                                        */
/*                                                                            */
/******************************************************************************/

void Jetpack::Client::handshakeWithServer()
{
    Jetpack::ProtocolUtils::sendPacket(this->_socket, LOGIN_REQUEST, {}, this->_debug);
    Jetpack::Packet login = Jetpack::ProtocolUtils::receivePacket(this->_socket, this->_debug);
    Jetpack::Packet map;

    if (_debug)
        Jetpack::Utils::consoleLog("Sent LOGIN_REQUEST to server", Jetpack::LogInfo::INFO);
    if (_debug)
        Jetpack::Utils::consoleLog("Received LOGIN_RESPONSE from server", Jetpack::LogInfo::INFO);
    if (login.type != LOGIN_RESPONSE || login.payload.size() < 3 || login.payload[1] != 0)
        throw ClientError("Invalid LOGIN_RESPONSE");
    this->_playerId = login.payload[0];
    this->_numberClients = login.payload[2];
    this->_sharedState->initPlayers(this->_numberClients);
    Jetpack::Utils::consoleLog("Login accepted by server, has ID " + std::to_string(this->_playerId), Jetpack::LogInfo::INFO);
    Jetpack::Utils::consoleLog("Waiting for " + std::to_string(this->_numberClients) + " players to be ready.", Jetpack::LogInfo::INFO);
    map = Jetpack::ProtocolUtils::receivePacket(this->_socket, this->_debug);
    if (_debug)
        Jetpack::Utils::consoleLog("Received MAP_TRANSFER from server", Jetpack::LogInfo::INFO);
    if (map.type != MAP_TRANSFER)
        throw ClientError("Expected MAP_TRANSFER");
    this->handleMap(map);
    this->_state = ClientState::Waiting;
}

void Jetpack::Client::waitForGameStart()
{
    try {
        while (_state == ClientState::Waiting) {
            Jetpack::Packet start = Jetpack::ProtocolUtils::receivePacket(_socket, this->_debug);
            if (_debug)
                Jetpack::Utils::consoleLog("Received packet type " + std::to_string(start.type), Jetpack::LogInfo::INFO);
            if (start.type == GAME_START) {
                this->_state = ClientState::Connected;
                Jetpack::Utils::consoleLog("All players are ready. Game is starting!", Jetpack::LogInfo::SUCCESS);
                break;
            } else if (start.type == WAITING_PLAYERS_COUNT) {
                _sharedState->setNumberClients(_sharedState->getNumberClients() + 1);
                if (_debug)
                    Jetpack::Utils::consoleLog("Updated waiting player count", Jetpack::LogInfo::INFO);
            } else if (start.type == MAP_TRANSFER) {
                handleMap(start);
                if (_debug)
                    Jetpack::Utils::consoleLog("Handled MAP_TRANSFER during wait", Jetpack::LogInfo::INFO);
            }
        }
    } catch (...) {
        disconnect();
    }
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
            Jetpack::Packet paquet = Jetpack::ProtocolUtils::receivePacket(this->_socket, this->_debug);

            auto it = this->_packetHandlers.find(paquet.type);
            if (it != this->_packetHandlers.end())
                it->second(paquet);
            else
                Jetpack::Utils::consoleLog("Unknown packet: 0x" + Jetpack::Utils::toHex(paquet.type), Jetpack::LogInfo::ERROR);
        }
    } catch (const std::exception &e) {
        Jetpack::Utils::consoleLog("Disconnected : " + std::string(e.what()), Jetpack::LogInfo::ERROR);
        this->_state = ClientState::Disconnected;
    }
}

void Jetpack::Client::handleGameState(const Jetpack::Packet &paquet)
{
    uint8_t id;
    bool alive;

    for (size_t i = 0; i + 2 <= paquet.payload.size(); i += 2) {
        id = paquet.payload[i];
        alive = paquet.payload[i + 1] == 1;
        this->_sharedState->updatePlayerAliveStatus(id, alive);
        if (_debug)
            Jetpack::Utils::consoleLog("Updated alive state for player " + std::to_string(id) + ": " + (alive ? "alive" : "dead"), Jetpack::LogInfo::INFO);
    }
}

void Jetpack::Client::handlePositionUpdate(const Jetpack::Packet &paquet)
{
    float x;
    float y;

    if (paquet.payload.size() < 9)
        return;
    uint8_t playerId = paquet.payload[0];
    std::memcpy(&x, &paquet.payload[1], sizeof(x));
    std::memcpy(&y, &paquet.payload[5], sizeof(y));
    this->_sharedState->updatePlayerPosition(playerId, x, y);
    if (_debug)
        Jetpack::Utils::consoleLog("Updated position for player " + std::to_string(playerId) +
        " -> x: " + std::to_string(x) + ", y: " + std::to_string(y), Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handleCoinEvent(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t playerId = paquet.payload[0];
    int coinX;
    int coinY;

    std::memcpy(&coinX, &paquet.payload[1], sizeof(coinX));
    std::memcpy(&coinY, &paquet.payload[1 + sizeof(coinX)], sizeof(coinY));
    Jetpack::PlayerState &playerState = this->_sharedState->getPlayerState(playerId);
    playerState.addCoin();
    playerState.addCoinCollected(coinX, coinY);
    if (_debug)
        Jetpack::Utils::consoleLog("Player " + std::to_string(playerId) + " collected coin at (" + std::to_string(coinX) + ", " + std::to_string(coinY) + ")", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handlePlayerEliminated(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t eliminatedId = paquet.payload[0];

    this->_sharedState->updatePlayerAliveStatus(eliminatedId, false);
    if (_debug)
        Jetpack::Utils::consoleLog("Player " + std::to_string(eliminatedId) + " was eliminated", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handleGameOver(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t winnerId = paquet.payload[0];
    this->_state = Jetpack::ClientState::GameOver;
    this->_gameOverWinnerId = winnerId;
    if (_debug)
        Jetpack::Utils::consoleLog("Received GAME_OVER. Winner: " + std::to_string(winnerId), Jetpack::LogInfo::INFO);
}

void Jetpack::Client::handleActionAck(const Jetpack::Packet& paquet)
{
    if (paquet.payload.size() == 1 && paquet.payload[0] == PLAYER_ACTION) {
        this->_ACKPlayerAction = true;
        if (_debug)
            Jetpack::Utils::consoleLog("Received ACTION_ACK from server", Jetpack::LogInfo::INFO);
    }
}

void Jetpack::Client::sendJump()
{
    std::vector<uint8_t> payload = {static_cast<uint8_t>(Jetpack::PlayerActionType::JUMP)};
    this->_ACKPlayerAction = false;
    if (_debug)
        Jetpack::Utils::consoleLog("Sending PLAYER_ACTION (JUMP) to server", Jetpack::LogInfo::INFO);
    Jetpack::ProtocolUtils::sendPacket(this->_socket, PLAYER_ACTION, payload, this->_debug);
    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(2);
    while (!this->_ACKPlayerAction && std::chrono::steady_clock::now() - startTime < timeout);
    if (this->_ACKPlayerAction) {
        if (_debug)
            Jetpack::Utils::consoleLog("Action acknowledged by server", Jetpack::LogInfo::INFO);
        this->_ACKPlayerAction = false;
    } else {
        if (_debug)
            Jetpack::Utils::consoleLog("No ACK received, retrying jump", Jetpack::LogInfo::INFO);
        sendJump();
    }
}

void Jetpack::Client::handleMap(const Jetpack::Packet &paquet)
{
    uint8_t playerId = paquet.payload[0];
    std::vector<std::vector<TileType>> playerMap;
    std::vector<TileType> row;

    for (size_t i = 1; i < paquet.payload.size(); ++i) {
        if (paquet.payload[i] == '\n') {
            playerMap.push_back(row);
            row.clear();
        } else
            row.push_back(static_cast<TileType>(paquet.payload[i]));
    }
    if (!row.empty())
        playerMap.push_back(row);
    if (this->_sharedState->getPlayerState(playerId).map != playerMap)
        this->_sharedState->getPlayerState(playerId).map = playerMap;
    if (playerId == this->_playerId && this->_map != playerMap)
        this->_map = playerMap;
    if (_debug)
        Jetpack::Utils::consoleLog("Map received and assigned for player ID " + std::to_string(playerId), Jetpack::LogInfo::INFO);
}
