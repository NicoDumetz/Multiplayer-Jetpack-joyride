/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** remoteClient
*/

#pragma once
#include <string>
#include <netinet/in.h>
#include <vector>
#include <poll.h>

/******************************************************************************/
/*                                                                            */
/*                               SERVER SIDE                                  */
/*                                                                            */
/******************************************************************************/


namespace Jetpack {
    class RemoteClient {
    public:
        RemoteClient(int socket) : _socket(socket), _id(255), _ready(false) {}
        ~RemoteClient() = default;

        int getSocket() const {return this->_socket;}
        uint8_t getId() const {return this->_id;}
        bool isReady() const {return this->_ready;}

        void setReady(bool ready) {this->_ready = ready;}
        void setId(uint8_t id) {this->_id = id;}

    private:
        int _socket;
        uint8_t _id;
        bool _ready;
    };
}
