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
#include "client/Client.hpp"
#include "Error/Error.hpp"
#include "SocketAddress/SocketAddress.hpp"
#define NUMBER_CLIENTS 2


namespace Jetpack {
    class Client;
    class Server {
    public:
        Server(int port);
        ~Server();

        class ServerError : public Jetpack::Error {
            public:
                ServerError(const std::string &message) : Jetpack::Error(message) {}
        };

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

    private:
        int _port;
        int _serverSocket;
        std::vector<std::unique_ptr<Jetpack::Client>> _clients;
    };
}