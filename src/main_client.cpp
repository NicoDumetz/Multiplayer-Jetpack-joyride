/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** main_client
*/

#include "Parser/Parser.hpp"
#include "Utils/Utils.hpp"
#include "client/Client.hpp"
#include "Game/Game.hpp"
#include <thread>
#include <exception>

int main(int ac, char **av, char **env)
{
    try {
        Jetpack::Parser args(ac, av, Jetpack::Mode::CLIENT);
        Jetpack::Utils::checkDisplay(env);
        auto client = std::make_shared<Jetpack::Client>(args, args.isDebug());
        client->handshakeWithServer();
        Jetpack::Game game(client);
        std::thread waitThread([&client]() {
            client->waitForGameStart();
        });
        game.waitingRoom();
        waitThread.join();
        if (client->getState() == Jetpack::ClientState::Disconnected) {
            Jetpack::Utils::consoleLog("Client disconnected.", Jetpack::LogInfo::INFO);
            return 84;
        }
        std::thread runThread([&client]() {
            client->run();
        });
        game.run();
        runThread.join();
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
