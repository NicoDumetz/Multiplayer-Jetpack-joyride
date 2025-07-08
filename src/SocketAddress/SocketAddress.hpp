/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** SocketAddres
*/

#pragma once
#include <netinet/in.h>

namespace Jetpack {

    class SocketAddress {
        public:
            SocketAddress()
            {
                this->_addr.sin_family = AF_INET;
                this->_len = sizeof(sockaddr_in);
            }
            sockaddr *raw() {return reinterpret_cast<sockaddr *>(&this->_addr);}
            socklen_t *lenPtr() {return &this->_len;}

        private:
            sockaddr_in _addr;
            socklen_t _len;
    };

}

