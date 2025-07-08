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
    std::string filename;

    for (int i = 1; i < ac; i++) {
        if (!Jetpack::String::strcmp(av[i], "-p") && i + 1 < ac && Jetpack::Utils::isNumber(av[i + 1]))
            this->_port = std::stoi(av[++i]);
        else if (!Jetpack::String::strcmp(av[i], "-h") && i + 1 < ac)
            this->_ip = av[++i];
        else if (!Jetpack::String::strcmp(av[i], "-m") && i + 1 < ac)
            filename = av[++i];
        else if (!Jetpack::String::strcmp(av[i], "-n") && i + 1 < ac && Jetpack::Utils::isNumber(av[i + 1])) {
            int value = std::stoi(av[++i]);
            if (value < 2 || value > NUMBER_CLIENTS_MAX)
                throw ParserError("Player count must be between 2 and " + NUMBER_CLIENTS_MAX );
            this->_expectedPlayers = value;
        } else
            throw ParserError("Invalid or missing arguments");
    }
    if (_port == 0 || (this->_mode == Jetpack::Mode::CLIENT && this->_ip.empty()) || (this->_mode == Jetpack::Mode::SERVER && filename.empty()))
        throw ParserError(this->_mode == Jetpack::Mode::CLIENT ? "Missing required arguments (-h or -p)" : "Missing required arguments (-p or -m)");
    if (this->_mode == Jetpack::Mode::CLIENT && this->_expectedPlayers != 2)
        throw ParserError("Client cannot specify -n (player count)");
    if (this->_mode == Jetpack::Mode::SERVER)
        this->loadMapContent(filename);
}

void Jetpack::Parser::loadMapContent(std::string &filename)
{
    std::ifstream file(filename);
    std::ostringstream content;
    int lineCount = 0;

    if (!file.is_open())
        throw ParserError("Unable to open map file: " + filename);
    content << file.rdbuf();
    this->_map = content.str();
    for (char c : this->_map) {
        if (c != '_' && c != 'e' && c != 'c' && c != '\n')
            throw ParserError("Invalid character in map file: '" + std::string(1, c) + "'");
        if (c == '\n')
            lineCount++;
    }

    if (!this->_map.empty() && this->_map.back() != '\n')
        lineCount++;

    if (lineCount > MAX_LINES)
        throw ParserError("Map file exceeds maximum number of lines: " + std::to_string(MAX_LINES));
}