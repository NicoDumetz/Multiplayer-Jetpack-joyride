/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** Utils
*/

#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <filesystem>
#include <string>
#include <map>
#include <sys/stat.h>
#include <sstream>
#include "IO/IO.hpp"
#include "File/File.hpp"
#define FTP_BUFFER_SIZE 1024
#define FTP_SMALL_BUFFER 256
#define FTP_PATH_BUFFER 512

// Client -> Serveur
#define LOGIN_REQUEST 0x01
#define PLAYER_ACTION 0x05

// Serveur -> Client
#define LOGIN_RESPONSE 0x02
#define MAP_TRANSFER 0x03
#define GAME_START 0x04
#define GAME_STATE 0x06
#define POSITION_UPDATE 0x07
#define COIN_EVENT 0x08
#define PLAYER_ELIMINATED 0x09
#define GAME_OVER 0x10

#define ACTION_ACK 0x0A

// constants server
#define NUMBER_CLIENTS 2
#define TICK_RATE 60.0f // 60 ticks/seconds
#define SCROLL_SPEED 2.0f // speed x (tiles/sec)
#define GRAVITY_SPEED 6.0f // speed gravity
#define JUMP_DELTA_Y 1.0f // delta Y jump
#define INVALID_TILE_ID 255
#define INVALID_ID 255
#define JETPACK_JUMP_SPEED 6.0f // tiles/second
#define TICK_INTERVAL (1.0f / 60.0f) // seconds


namespace Jetpack {
    enum class TileType {
        EMPTY,
        COIN,
        ZAPPER
    };

    struct Packet {
        uint8_t type;
        std::vector<uint8_t> payload;
    };

    enum class LogInfo {
        INFO,
        ERROR,
        SUCCESS,
        DISCONNECT
    };

    static const std::map<LogInfo, std::string> LogColorMap = {
        {LogInfo::INFO, "\033[1;34m"},
        {LogInfo::ERROR, "\033[1;31m"},
        {LogInfo::SUCCESS, "\033[1;32m"},
        {LogInfo::DISCONNECT, "\033[1;34m"}
    };

    class Utils {
    public:

        static void consoleLog(const std::string &msg, LogInfo color);

        static std::vector<std::string> str_to_vector(const std::string line)
        {
            std::vector<std::string> tokens;
            std::string token;

            for (std::stringstream ss(line); ss >> token;)
                tokens.push_back(token);
            return tokens;
        }

        static std::vector<std::string> split(const std::string line, char delimiter)
        {
            std::vector<std::string> tokens;
            std::string token;

            for (std::stringstream ss(line); std::getline(ss, token, delimiter); tokens.push_back(std::move(token)));
            return tokens;
        }

        static Jetpack::LogInfo writeAndLog(int socket, const std::string &msg, const std::string &logMsg, Jetpack::LogInfo logLevel)
        {
            Jetpack::IO::write(socket, msg.c_str(), msg.size());
            consoleLog(logMsg, logLevel);
            return logLevel;
        }

        static bool isNumber(const std::string &str)
        {
            if (str.empty())
                return false;
            for (char c : str)
                if (!std::isdigit(c))
                    return false;
            return true;
        }

        static void printUsageServer()
        {
            std::cerr << "Usage: ./jetpack_server -p <port> -m <map> [-d]" << std::endl;
        }

        static void printUsageClient()
        {
            std::cerr << "Usage: ./jetpack_client -h <ip> -p <port> [-d]" << std::endl;
        }

        static std::string toHex(uint8_t value)
        {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(value);
            return oss.str();
        }

    };
    std::ostream& operator<<(std::ostream& os, const LogInfo& color);

    inline std::ostream& operator<<(std::ostream& os, TileType tile)
    {
        switch (tile) {
            case TileType::EMPTY:
                return os << '_';
            case TileType::COIN:
                return os << 'c';
            case TileType::ZAPPER:
                return os << 'e';
            default:
                return os << '?';
        }
    }
}

