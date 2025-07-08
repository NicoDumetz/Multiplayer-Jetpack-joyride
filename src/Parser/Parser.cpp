/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Parser
*/
#include "Parser/Parser.hpp"

Jetpack::Parser::Parser(int ac, char **av, Mode mode)
    : _mode(mode), _port(0)
{
    for (int i = 1; i < ac; i++) {
        if (!strcmp(av[i], "-p") && i + 1 < ac && Jetpack::Utils::isNumber(av[i + 1]))
            this->_port = std::stoi(av[++i]);
        else if (!strcmp(av[i], "-h") && i + 1 < ac)
            this->_ip = av[++i];
        else if (!strcmp(av[i], "-m") && i + 1 < ac)
            this->_mapPath = av[++i];
        else
            throw ParserError("Invalid or missing arguments");
    }
    if (_port == 0 || (this->_mode == Jetpack::Mode::CLIENT && this->_ip.empty()) || (this->_mode == Jetpack::Mode::SERVER && this-> _mapPath.empty()))
        throw ParserError(this->_mode == Jetpack::Mode::CLIENT ? "Missing required arguments (-h or -p)" : "Missing required arguments (-p or -m)");
}
