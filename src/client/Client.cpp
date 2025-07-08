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

Jetpack::Client::~Client()
{
    try {
        Jetpack::IO::close(_socket);
    } catch (const Jetpack::IO::IOError &e) {
        Jetpack::Utils::consoleLog("Client close failed: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
    }
}

Jetpack::Client::Client(const Jetpack::Parser &args)
{
    this->_socket = Jetpack::Network::socket(AF_INET, SOCK_STREAM, 0);
    this->_connected = true;
    Jetpack::SocketAddress addr;
    auto *in = reinterpret_cast<sockaddr_in*>(addr.raw());
    in->sin_port = Jetpack::Network::htons(args.getPort());
    Jetpack::Network::inet_pton(AF_INET, args.getIp().c_str(), &in->sin_addr);
    Jetpack::Network::connect(this->_socket, addr.raw(), *addr.lenPtr());
    Jetpack::Utils::consoleLog("Connected to " + args.getIp(), Jetpack::LogInfo::SUCCESS);
}

/******************************************************************************/
/*                                                                            */
/*                              Comunication                                  */
/*                                                                            */
/******************************************************************************/


std::string Jetpack::Client::receiveMessage()
{
    char tmp[FTP_BUFFER_SIZE];
    int n = Jetpack::IO::read(this->_socket, tmp, sizeof(tmp));
    size_t pos;
    std::string line;

    if (n <= 0)
        return "";
    this->_buffer.append(tmp, n);
    pos = this->_buffer.find('\n');
    if (pos != std::string::npos) {
        line = this->_buffer.substr(0, pos);
        this->_buffer.erase(0, pos + 1);
        return line;
    }
    return "\0";
}

void Jetpack::Client::sendMessage(const std::string &message)
{
    try {
        Jetpack::IO::write(this->_socket, message.c_str(), message.size());
    } catch (const Jetpack::IO::IOError &e) {
        Jetpack::Utils::consoleLog("Failed to send to client: " + std::string(e.what()), Jetpack::LogInfo::ERROR);
        this->_connected = false;
    }
}

/******************************************************************************/
/*                                                                            */
/*                              temp                                          */
/*                                                                            */
/******************************************************************************/

void Jetpack::Client::run()
{
    std::string msg;
    this->sendMessage("WELCOME\n");
    while (true) { // temporaire
        msg = this->receiveMessage();
        if (msg.empty())
            break;
        if (msg == "READY")
            this->sendMessage("WAITING FOR PLAYER\n");
        else if (msg == "EXIT")
            break;
        else
            this->sendMessage(msg);
    }
}