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
#include "SocketAddress/SocketAddress.hpp"

namespace Jetpack {

    class Client {
        public:
            Client(int socket) : _socket(socket), _connected(true) {};
            Client(const Jetpack::Parser &args);
            ~Client();

            int getSocket() const {return this->_socket;}
            std::string receiveMessage();
            void sendMessage(const std::string &message);
            void run();

        private:
            int _socket;
            bool _connected;
            std::string _buffer;
    };
}
