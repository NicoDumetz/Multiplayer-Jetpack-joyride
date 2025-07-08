/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** main_client
*/

#include "Parser/Parser.hpp"
#include "Utils/Utils.hpp"
#include "client/Client.hpp"

int main(int ac, char **av)
{
    try {
        Jetpack::Parser args(ac, av, Jetpack::Mode::CLIENT);
        Jetpack::Client client(args);
        client.connectToServer();
        client.run();

    } catch (const Jetpack::Parser::ParserError &e) {
        Jetpack::Utils::printUsageClient();
        Jetpack::Utils::consoleLog(e.what(), Jetpack::LogInfo::ERROR);
        return 84;

    } catch (const Jetpack::Error &e) {
        Jetpack::Utils::consoleLog(e.what(), Jetpack::LogInfo::ERROR);
        return 84;

    }
    return 0;
}