/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** main
*/

#include "Parser/Parser.hpp"
#include "Utils/Utils.hpp"
#include "client/Client.hpp"

int main(int ac, char **av)
{
    try {
        Jetpack::Parser args(ac, av, Jetpack::Mode::CLIENT);
        Jetpack::Client client(args);

        Jetpack::Utils::consoleLog("Connected to server, starting session.", Jetpack::LogInfo::SUCCESS);
        client.sendMessage("READY\n");
        while (true) { // temporaire
            std::string msg = client.receiveMessage();
            if (!msg.empty())
                Jetpack::Utils::consoleLog("Server: " + msg, Jetpack::LogInfo::INFO);
        }
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