/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** server
*/

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <poll.h>
#include <netinet/in.h>
#include "client/Client.hpp"
#include "Process/Process.hpp"
#include "Network/Network.hpp"
#include "IO/IO.hpp"
#include "Utils/Utils.hpp"
#include "RemoteClient/remoteClient.hpp"
#include "Error/Error.hpp"
#include <functional>
#include "SocketAddress/SocketAddress.hpp"
#define NUMBER_CLIENTS 2


/******************************************************************************/
/*                                                                            */
/*                               SERVER SIDE                                  */
/*                                                                            */
/******************************************************************************/

namespace Jetpack {
    class Server {
    public:
        Server(int port, std::string map);
        ~Server();

        class ServerError : public Jetpack::Error {
            public:
                ServerError(const std::string &message) : Jetpack::Error(message) {}
        };

        void parseMap(const std::string &map);
        void run();
        void setupServer();
        void acceptClient();
        void removeClient(int clientIndex);
        std::vector<struct pollfd> preparePollFds() const;
        bool waitForEvents(std::vector<struct pollfd> &pollFds) const;
        void handleNewClient(std::vector<struct pollfd> &pollFds);
        void handleClientActivity(std::vector<struct pollfd> &pollFds);
        int getPort() const {return this->_port;}
        int getSocket() const {return this->_serverSocket;}
        int findClientIndexByFd(int fd) const;
        void lunchStart();
        int countReadyClients() const;

    private:
        int _port;
        int _serverSocket;
        std::vector<std::vector<TileType>> _map;
        std::vector<std::unique_ptr<Jetpack::RemoteClient>> _clients;
        std::map<uint8_t, std::function<void(int, const Jetpack::Packet&)>> _packetHandlers = {
            {0x01, [this](int fd, const Jetpack::Packet& pkt) {return this->handleLogin(fd, pkt);}},
            {0x05, [this](int fd, const Jetpack::Packet& pkt) {return this->handlePlayerAction(fd, pkt);}},
            {0x09, [this](int fd, const Jetpack::Packet& pkt) {return this->handleGameOver(fd, pkt);}}
        };
        void handleLogin(int fd, const Jetpack::Packet& pkt);
        void handlePlayerAction(int fd, const Jetpack::Packet& pkt);
        void handleGameOver(int fd, const Jetpack::Packet& pkt);
    };
}