/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Network
*/

#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include "Error/Error.hpp"

namespace Jetpack {
    class Network {
        public:
            class NetworkError : public Jetpack::Error {
                public:
                    NetworkError(const std::string &msg) : Jetpack::Error(msg) {}
            };

            static int socket(int domain, int type, int protocol)
            {
                int fd = ::socket(domain, type, protocol);
                if (fd == -1)
                    throw NetworkError("socket creation failed");
                return fd;
            }

            static void bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
            {
                if (::bind(sockfd, addr, addrlen) == -1)
                    throw NetworkError("bind operation failed");
            }

            static void listen(int sockfd, int backlog)
            {
                if (::listen(sockfd, backlog) == -1)
                    throw NetworkError("listen operation failed");
            }

            static int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
            {
                int fd = ::accept(sockfd, addr, addrlen);
                if (fd == -1)
                    throw NetworkError("accept operation failed");
                return fd;
            }

            static void connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
            {
                if (::connect(sockfd, addr, addrlen) == -1)
                    throw NetworkError("connect operation failed");
            }

            static int poll(struct pollfd *fds, nfds_t nfds, int timeout)
            {
                int ret = ::poll(fds, nfds, timeout);
                if (ret == -1)
                    throw NetworkError("poll operation failed");
                return ret;
            }

            static void inet_pton(int af, const char *src, void *dst)
            {
                if (::inet_pton(af, src, dst) <= 0)
                    throw NetworkError("inet_pton conversion failed");
            }

            static void getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
            {
                if (::getsockname(sockfd, addr, addrlen) == -1)
                    throw NetworkError("getsockname operation failed");
            }

            static uint16_t htons(uint16_t value)
            {
                return ::htons(value);
            }

            static uint16_t ntohs(uint16_t value)
            {
                return ::ntohs(value);
            }
    };
}
