/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** main
*/

#include "server/Server.hpp"
#include "Parser/Parser.hpp"
#include "Utils/Utils.hpp"


int main(int ac, char **av)
{
    try {
        Jetpack::Parser args(ac, av, Jetpack::Mode::SERVER);
        Jetpack::Server serv(args.getPort());

        serv.run();
    } catch (const Jetpack::Parser::ParserError &e) {
        Jetpack::Utils::printUsageServer();
        Jetpack::Utils::consoleLog(e.what(), Jetpack::LogInfo::ERROR);
        return 84;
    } catch (const Jetpack::Error &e) {
        Jetpack::Utils::consoleLog(e.what(), Jetpack::LogInfo::ERROR);
        return 84;
    }
    return 0;
}

