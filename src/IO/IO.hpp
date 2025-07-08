/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** IO
*/

#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include "Error/Error.hpp"

namespace Jetpack {
    class IO {
    public:
        class IOError : public Jetpack::Error {
            public:
                IOError(const std::string &msg) : Jetpack::Error(msg) {}
        };

        static ssize_t write(int fd, const void *msg, size_t len)
        {
            ssize_t ret = ::write(fd, msg, len);
            if (ret == -1)
                throw IOError("write operation failed");
            return ret;
        }

        static ssize_t read(int fd, void *buffer, size_t len)
        {
            if (len == 0)
                return 0;
            ssize_t ret = ::read(fd, buffer, len);
            if (ret == 0)
                throw IOError("connection closed by peer");
            if (ret < 0)
                throw IOError("read operation failed");
            return ret;
        }

        static void close(int fd)
        {
            if (::close(fd) == -1)
                throw IOError("close operation failed");
        }

        static void closeSocket(int fd)
        {
            ::shutdown(fd, SHUT_RDWR);
            if (::close(fd) == -1)
                throw IOError("socket close operation failed");
        }
    };
}
