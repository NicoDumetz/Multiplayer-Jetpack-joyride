/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Parser
*/

#pragma once
#include <string>
#include <stdexcept>
#include <cstring>
#include "Error/Error.hpp"
#include "Utils/Utils.hpp"

namespace Jetpack {
    enum class Mode {
        CLIENT,
        SERVER
    };

    class Parser {
        public:
            class ParserError : public Jetpack::Error {
                public:
                    ParserError(const std::string &message) : Jetpack::Error(message) {}
            };
        public:
            Parser(int ac, char **av, Mode mode);
            ~Parser() = default;

            inline void setPort(int port) {this->_port = port;}
            inline void setMapPath(const std::string &mapPath) {this->_mapPath = mapPath;}
            inline void setIp(const std::string &ip) {this->_ip = ip;}

            inline int getPort() const {return this->_port;}
            inline const std::string &getMapPath() const {return this->_mapPath;}
            inline const std::string &getIp() const {return this->_ip;}

        private:
            Mode _mode;
            int _port;
            std::string _mapPath;
            std::string _ip;
            bool _isClient;
    };
}