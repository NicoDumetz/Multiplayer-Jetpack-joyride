/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Error
*/

#pragma once
#include <exception>
#include <iostream>
#include <string>

namespace Jetpack
{
    class Error : public std::exception
    {
        public:
            Error(const std::string &message) : _message(message) {};
            ~Error() = default;
            const char *what() const throw() {return this->_message.c_str();};
        private:
            std::string _message;
    };
}
