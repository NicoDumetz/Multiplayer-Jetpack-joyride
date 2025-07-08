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

Jetpack::Server::Server(int port)
    : _port(port), _serverSocket(-1)
{
    try {
        this->setupServer();
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
    Jetpack::Utils::consoleLog("New Client (fd = " + std::to_string(socket) + ")", Jetpack::LogInfo::SUCCESS);
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

    for (size_t i = 1; i < pollFds.size(); ++i) {
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
    }
}

/******************************************************************************/
/*                                                                            */
/*                               IN WORKING                                   */
/*                                                                            */
/******************************************************************************/

int Jetpack::Server::findClientIndexByFd(int fd) const
{
    for (size_t i = 0; i < this->_clients.size(); i++)
        if (this->_clients[i]->getSocket() == fd)
            return static_cast<int>(i);
    return -1;
}


void Jetpack::Server::handleLogin(int fd, const Jetpack::Packet&)
{
    int index = this->findClientIndexByFd(fd);
    uint8_t id;

    if (index == -1)
        return;
    id = static_cast<uint8_t>(index);
    this->_clients[index]->setId(id);
    this->_clients[index]->setReady(true);
    Jetpack::ProtocolUtils::sendPacket(fd, 0x02, {id});
    Jetpack::Utils::consoleLog("Client " + std::to_string(fd) + " ID " + std::to_string(id), Jetpack::LogInfo::SUCCESS);
}

void Jetpack::Server::handlePlayerAction(int, const Jetpack::Packet&)
{
    return;
}

void Jetpack::Server::handleGameOver(int, const Jetpack::Packet&)
{
    return;
}