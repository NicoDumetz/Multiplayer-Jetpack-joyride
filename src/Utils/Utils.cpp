/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-myftp-nicolas.dumetz
** File description:
** Utils
*/

#include "Utils.hpp"


std::ostream& Jetpack::operator<<(std::ostream& os, const Jetpack::LogInfo& color)
{
    switch (color) {
        case Jetpack::LogInfo::INFO:
            os << "[Info]";
            break;
        case Jetpack::LogInfo::ERROR:
            os << "[Error]";
            break;
        case Jetpack::LogInfo::SUCCESS:
            os << "[Success]";
            break;
        case Jetpack::LogInfo::DISCONNECT:
            os << "[Disconnect]";
            break;
    }
    return os;
}

void Jetpack::Utils::consoleLog(const std::string &msg, LogInfo color)
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);

    std::cout << Jetpack::LogColorMap.at(color)
              << "["
              << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
              << std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
              << std::setfill('0') << std::setw(2) << localTime->tm_sec
              << "] "
              << color
              << " " << msg
              << "\033[0m" << std::endl;
}
