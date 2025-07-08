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

void Jetpack::Client::handshakeWithServer()
{
    Jetpack::ProtocolUtils::sendPacket(this->_socket, LOGIN_REQUEST, {});
    Jetpack::Packet login = Jetpack::ProtocolUtils::receivePacket(this->_socket);
    Jetpack::Packet map;

    if (login.type != LOGIN_RESPONSE || login.payload.size() != 1)
        throw ClientError("Invalid LOGIN_RESPONSE");
    this->_playerId = login.payload[0];
    Jetpack::Utils::consoleLog("Login accepted by server, has ID " + std::to_string(this->_playerId), Jetpack::LogInfo::INFO);
    map = Jetpack::ProtocolUtils::receivePacket(this->_socket);
    if (map.type != MAP_TRANSFER)
        throw ClientError("Expected MAP_TRANSFER");
    this->parseMapPayload(map.payload);
    Jetpack::Utils::consoleLog("Map was successfully received.", Jetpack::LogInfo::INFO);
    this->_state = ClientState::Waiting;
    Jetpack::Utils::consoleLog("Waiting game start...", Jetpack::LogInfo::INFO);
}

void Jetpack::Client::waitForGameStart()
{
    while (1) {
        Jetpack::Packet start = Jetpack::ProtocolUtils::receivePacket(this->_socket);

        if (start.type == GAME_START)
            break;
        else if (start.type == WAITING_PLAYERS_COUNT)
            this->_sharedState->setNumberClients(this->_sharedState->getNumberClients() + 1);
    }
    this->_state = ClientState::Connected;
    Jetpack::Utils::consoleLog("All players are ready. Game is starting!", Jetpack::LogInfo::SUCCESS);
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

void Jetpack::Client::handleGameState(const Jetpack::Packet &paquet)
{
    uint8_t id;
    bool alive;

    for (size_t i = 0; i + 2 <= paquet.payload.size(); i += 2) {
        id = paquet.payload[i];
        alive = paquet.payload[i + 1] == 1;
        this->_sharedState->updatePlayerAliveStatus(id, alive);
    }
}

void Jetpack::Client::handlePositionUpdate(const Jetpack::Packet &paquet)
{
    if (paquet.payload.size() < 9)
        return;
    uint8_t playerId = paquet.payload[0];
    float x;
    float y;
    std::memcpy(&x, &paquet.payload[1], sizeof(x));
    std::memcpy(&y, &paquet.payload[5], sizeof(y));
    this->_sharedState->updatePlayerPosition(playerId, x, y);
}

void Jetpack::Client::handleCoinEvent(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t playerId = paquet.payload[0];
    Jetpack::PlayerState& playerState = this->_sharedState->getPlayerState(playerId);
    playerState.addCoin();
}

void Jetpack::Client::handlePlayerEliminated(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t eliminatedId = paquet.payload[0];
    this->_sharedState->updatePlayerAliveStatus(eliminatedId, false);
}

void Jetpack::Client::handleGameOver(const Jetpack::Packet &paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t winner = paquet.payload[0];

    if (winner == 255)
        Jetpack::Utils::consoleLog("Game over: It's a tie!", Jetpack::LogInfo::SUCCESS);
    else if (winner == this->_playerId)
        Jetpack::Utils::consoleLog("Game over: You win!", Jetpack::LogInfo::SUCCESS);
    else
        Jetpack::Utils::consoleLog("Game over: Player " + std::to_string(winner) + " wins", Jetpack::LogInfo::SUCCESS);
    this->_state = ClientState::Disconnected;
    this->_sharedState->setGameOver(true);
}

void Jetpack::Client::handleActionAck(const Jetpack::Packet& paquet)
{
    if (paquet.payload.size() == 1 && paquet.payload[0] == PLAYER_ACTION)
        this->_ACKPlayerAction = true;
}

void Jetpack::Client::sendJump()
{
    std::vector<uint8_t> payload = {static_cast<uint8_t>(Jetpack::PlayerActionType::JUMP)};
    this->_ACKPlayerAction = false;
    Jetpack::ProtocolUtils::sendPacket(this->_socket, PLAYER_ACTION, payload);
    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(2);

    while (!this->_ACKPlayerAction && std::chrono::steady_clock::now() - startTime < timeout);
    if (this->_ACKPlayerAction)
        this->_ACKPlayerAction = false;
    else
        sendJump();
}