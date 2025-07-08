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
namespace Jetpack {
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

    };
    std::ostream& operator<<(std::ostream& os, const LogInfo& color);
}

