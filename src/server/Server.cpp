/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** server
*/

#include "server/Server.hpp"

/******************************************************************************/
/*                                                                            */
/*                               CONSTRUCTORS                                 */
/*                                                                            */
/******************************************************************************/

Jetpack::Server::Server(int port, std::string map)
    : _port(port), _serverSocket(-1)
{
    try {
        this->setupServer();
        this->parseMap(map);
    } catch (const Jetpack::Error &e) {
        throw ServerError("Server setup failed: " + std::string(e.what()));
    }
}

Jetpack::Server::~Server()
{
    try {
        if (this->_serverSocket != -1)
            Jetpack::IO::close(this->_serverSocket);
    } catch (const Jetpack::IO::IOError &e) {
        Jetpack::Utils::consoleLog("Failed to close server socket: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
    }
}


/******************************************************************************/
/*                                                                            */
/*                            SERVER INITIALIZATION                           */
/*                                                                            */
/******************************************************************************/

void Jetpack::Server::setupServer()
{
    Jetpack::SocketAddress addr;

    auto *in = reinterpret_cast<sockaddr_in*>(addr.raw());
    in->sin_port = Jetpack::Network::htons(this->_port);
    in->sin_addr.s_addr = INADDR_ANY;
    this->_serverSocket = Jetpack::Network::socket(AF_INET, SOCK_STREAM, 0);
    Jetpack::Network::bind(this->_serverSocket, addr.raw(), *addr.lenPtr());
    Jetpack::Network::listen(this->_serverSocket, 2);
    Jetpack::Utils::consoleLog("Server listening on port " + std::to_string(_port), Jetpack::LogInfo::INFO);
}

void Jetpack::Server::parseMap(const std::string &mapStr)
{
    std::vector<TileType> row;

    for (char c : mapStr) {
        switch (c) {
            case '\n':
                if (!row.empty()) {
                    this->_map.push_back(row);
                    row.clear();
                }
                break;
            case '_':
                row.push_back(TileType::EMPTY);
                break;
            case 'c':
                row.push_back(TileType::COIN);
                break;
            case 'e':
                row.push_back(TileType::ZAPPER);
                break;
            default:
                break;
        }
    }
    if (!row.empty())
        this->_map.push_back(row);
}


/******************************************************************************/
/*                                                                            */
/*                             CLIENT MANAGEMENT                              */
/*                                                                            */
/******************************************************************************/


void Jetpack::Server::acceptClient()
{
    Jetpack::SocketAddress clientAddr;
    int socket = Jetpack::Network::accept(this->_serverSocket, clientAddr.raw(), clientAddr.lenPtr());

    this->_clients.push_back(std::make_unique<Jetpack::RemoteClient>(socket));
}


void Jetpack::Server::removeClient(int index)
{
    Jetpack::Utils::consoleLog("Client disconnected.", Jetpack::LogInfo::INFO);
    this->_clients.erase(this->_clients.begin() + index);
}

/******************************************************************************/
/*                                                                            */
/*                            POLLING SYSTEM                                  */
/*                                                                            */
/******************************************************************************/

bool Jetpack::Server::waitForEvents(std::vector<struct pollfd> &pollFds) const
{
    try {
        Jetpack::Network::poll(pollFds.data(), pollFds.size(), -1);
    } catch (const Jetpack::Network::NetworkError &e) {
        Jetpack::Utils::consoleLog("Poll error: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
        return false;
    }
    return true;
}

bool Jetpack::Server::waitForEvents(std::vector<struct pollfd> &pollFds, int timeoutMs)
{
    try {
        Jetpack::Network::poll(pollFds.data(), pollFds.size(), timeoutMs);
    } catch (const Jetpack::Network::NetworkError &e) {
        Jetpack::Utils::consoleLog("Poll error: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
        return false;
    }
    return true;
}

void Jetpack::Server::handleNewClient(std::vector<struct pollfd> &pollFds)
{
    if (pollFds[0].revents & POLLIN) {
        try {
            this->acceptClient();
        } catch (const ServerError &e) {
            Jetpack::Utils::consoleLog(e.what(), Jetpack::LogInfo::ERROR);
        }
    }
}

void Jetpack::Server::handleClientActivity(std::vector<struct pollfd> &pollFds)
{
    size_t clientIndex;
    int client_fd;

    for (size_t i = 1; i < pollFds.size(); i++) {
        clientIndex = i - 1;
        if (clientIndex >= this->_clients.size() || !(pollFds[i].revents & POLLIN))
            continue;
        client_fd = this->_clients[clientIndex]->getSocket();
        try {
            Jetpack::Packet paquet = Jetpack::ProtocolUtils::receivePacket(client_fd);
            auto it = this->_packetHandlers.find(paquet.type);
            if (it != this->_packetHandlers.end()) {
                it->second(client_fd, paquet);
            } else {
                Jetpack::Utils::consoleLog("unknown Paquet", Jetpack::LogInfo::ERROR);
            }
        } catch (const std::exception &e) {
            Jetpack::Utils::consoleLog("Client disconnected (" + std::string(e.what()) + ")", Jetpack::LogInfo::ERROR);
            this->removeClient(clientIndex);
            break;
        }
    }
}

std::vector<struct pollfd> Jetpack::Server::preparePollFds() const
{
    std::vector<struct pollfd> fds;
    fds.push_back({this->_serverSocket, POLLIN, 0});
    for (const auto &client : this->_clients)
        fds.push_back({client->getSocket(), POLLIN, 0});
    return fds;
}

/******************************************************************************/
/*                                                                            */
/*                               SERVER LOOP                                  */
/*                                                                            */
/******************************************************************************/

void Jetpack::Server::run()
{
    while (true) {
        auto pollFds = this->preparePollFds();
        if (!this->waitForEvents(pollFds))
            continue;
        this->handleNewClient(pollFds);
        this->handleClientActivity(pollFds);
        if (this->countReadyClients() == NUMBER_CLIENTS) {
            this->startGameLoop();
            break;
        }
    }
    Jetpack::Utils::consoleLog("game is over", Jetpack::LogInfo::INFO);
}

/******************************************************************************/
/*                                                                            */
/*                               PROTOCOLE                                    */
/*                                                                            */
/******************************************************************************/

int Jetpack::Server::findClientIndexByFd(int fd) const
{
    for (size_t i = 0; i < this->_clients.size(); i++)
        if (this->_clients[i]->getSocket() == fd)
            return static_cast<int>(i);
    return -1;
}

void Jetpack::Server::lunchStart()
{
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), GAME_START, {});
    Jetpack::Utils::consoleLog("All Clients are Ready. GAME_START send.", Jetpack::LogInfo::SUCCESS);
    this->_playerStates.clear();
    for (const auto &client : this->_clients) {
        PlayerState player(client->getId(), client->getSocket());
        player.setX(0);
        player.setY(static_cast<int>(_map.size()) - 1);
        player.map = _map;
        this->_playerStates.push_back(player);
    }
}

int Jetpack::Server::countReadyClients() const
{
    int count = 0;

    for (auto &client : this->_clients)
        if (client->isReady())
            count++;
    return count;
}

void Jetpack::Server::handleLogin(int fd, const Jetpack::Packet&)
{
    int index = this->findClientIndexByFd(fd);
    uint8_t id;
    uint8_t waitingPlayers;

    if (index == -1)
        return;
    id = static_cast<uint8_t>(index);
    this->_clients[index]->setId(id);
    this->_clients[index]->setReady(true);
    Jetpack::ProtocolUtils::sendPacket(fd, LOGIN_RESPONSE, {id});
    Jetpack::Utils::consoleLog("New Client accepted, has ID " + std::to_string(id), Jetpack::LogInfo::SUCCESS);
    this->sendMap(id, this->_map);
    waitingPlayers = static_cast<uint8_t>(this->countReadyClients());
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), WAITING_PLAYERS_COUNT, {waitingPlayers});
    if (this->countReadyClients() == NUMBER_CLIENTS)
        this->lunchStart();
    else
        Jetpack::Utils::consoleLog("Client ID " + std::to_string(id) + " is waiting...", Jetpack::LogInfo::INFO);
}

void Jetpack::Server::handlePlayerAction(int fd, const Jetpack::Packet& paquet)
{
    if (paquet.payload.empty())
        return;
    uint8_t actionRaw = paquet.payload[0];
    Jetpack::PlayerActionType action = static_cast<Jetpack::PlayerActionType>(actionRaw);

    if (action != Jetpack::PlayerActionType::JUMP)
        return;
    for (auto &player : this->_playerStates) {
        if (player.getSocket() == fd && player.isAlive()) {
            if (player.getY() > 0.0f) {
                player.setY(player.getY() - JETPACK_JUMP_SPEED * TICK_INTERVAL);
                player.setHasJumped(true);
            }
            break;
        }
    }
    std::vector<uint8_t> ackPayload = {PLAYER_ACTION};
    Jetpack::ProtocolUtils::sendPacket(fd, ACTION_ACK, ackPayload);
}

void Jetpack::Server::sendCoinEvent(uint8_t playerId, int coinX, int coinY)
{
    std::vector<uint8_t> payload = {playerId};
    uint8_t xBytes[sizeof(coinX)];
    uint8_t yBytes[sizeof(coinY)];

    std::memcpy(xBytes, &coinX, sizeof(coinX));
    std::memcpy(yBytes, &coinY, sizeof(coinY));
    payload.insert(payload.end(), xBytes, xBytes + sizeof(coinX));
    payload.insert(payload.end(), yBytes, yBytes + sizeof(coinY));
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), COIN_EVENT, payload);
}


void Jetpack::Server::sendGameState()
{
    std::vector<uint8_t> payload;

    for (const auto &player : this->_playerStates) {
        payload.push_back(player.getId());
        payload.push_back(player.isAlive() ? 1 : 0);
    }
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), GAME_STATE, payload);
}

void Jetpack::Server::sendPositionUpdate(uint8_t playerId, float x, float y)
{
    std::vector<uint8_t> payload;
    uint8_t xBytes[sizeof(x)];
    uint8_t yBytes[sizeof(y)];

    if (playerId == INVALID_ID)
        return;
    payload.push_back(playerId);
    std::memcpy(xBytes, &x, sizeof(x));
    payload.insert(payload.end(), xBytes, xBytes + sizeof(x));
    std::memcpy(yBytes, &y, sizeof(y));
    payload.insert(payload.end(), yBytes, yBytes + sizeof(y));
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), POSITION_UPDATE, payload);
}

void Jetpack::Server::sendMap(int playerId, const std::vector<std::vector<TileType>> &map)
{
    std::vector<uint8_t> payload;
    payload.push_back(playerId);

    for (const auto &row : map) {
        for (TileType tile : row) {
            payload.push_back(static_cast<uint8_t>(tile));
        }
        payload.push_back('\n');
    }
    for (const auto &client : this->_clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), MAP_TRANSFER, payload);
}

/******************************************************************************/
/*                                                                            */
/*                               GAME                                         */
/*                                                                            */
/******************************************************************************/
void Jetpack::Server::startGameLoop()
{
    auto lastTime = std::chrono::steady_clock::now();
    float accumulator = 0.0f;
    const int mapHeight = static_cast<int>(_map.size());
    const int mapWidth = static_cast<int>(_map[0].size());
    bool gameRunning = true;

    while (gameRunning) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        accumulator += dt;
        this->processNetworkEvents();
        while (accumulator >= TICK_INTERVAL) {
            accumulator -= TICK_INTERVAL;
            this->processPlayers(mapHeight, mapWidth);
        }
        this->sendGameState();
        gameRunning = this->isGameStillRunning();
    }
    this->handleGameOver();
}

void Jetpack::Server::processNetworkEvents()
{
    auto pollFds = this->preparePollFds();

    if (this->waitForEvents(pollFds, 0))
        this->handleClientActivity(pollFds);
}

void Jetpack::Server::processPlayers(int mapHeight, int mapWidth)
{
    for (auto &player : this->_playerStates) {
        if (!player.isAlive())
            continue;
        player.setX(player.getX() + SCROLL_SPEED * TICK_INTERVAL);
        if (!player.hasJumped() && player.getY() < mapHeight - 1)
            player.setY(player.getY() + GRAVITY_SPEED * TICK_INTERVAL);
        player.setHasJumped(false);
        if (player.getTileX() >= mapWidth) {
            player.setAlive(false);
            continue;
        }
        this->checkCollisions(player);
        this->sendPositionUpdate(player.getId(), player.getX(), player.getY());
    }
}

void Jetpack::Server::checkCollisions(PlayerState &player)
{
    TileType tile = player.map[player.getTileY()][player.getTileX()];

    if (tile == TileType::COIN) {
        player.map[player.getTileY()][player.getTileX()] = TileType::EMPTY;
        player.addCoin();
        sendCoinEvent(player.getId(), player.getCurrentX(), player.getCurrentY());
    } else if (tile == TileType::ZAPPER)
        player.setAlive(false);
}

bool Jetpack::Server::isGameStillRunning()
{
    return std::any_of(this->_playerStates.begin(), _playerStates.end(),[](const PlayerState &p) {return p.isAlive();});
}

void Jetpack::Server::handleGameOver()
{
    uint8_t winnerId = INVALID_ID;
    int bestScore = -1;
    bool tie = false;

    for (const auto &player : this->_playerStates) {
        if (player.getCoins() > bestScore) {
            bestScore = player.getCoins();
            winnerId = player.getId();
            tie = false;
        } else if (player.getCoins() == bestScore) {
            tie = true;
        }
    }
    if (tie)
        winnerId = 255;
    std::vector<uint8_t> payload = {winnerId};
    for (const auto &client : _clients)
        Jetpack::ProtocolUtils::sendPacket(client->getSocket(), GAME_OVER, payload);
    Jetpack::Utils::consoleLog("Game ended (all players dead or map finished)", Jetpack::LogInfo::INFO);

}
