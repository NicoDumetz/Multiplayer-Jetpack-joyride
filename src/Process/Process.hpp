/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Process
*/

#pragma once
#include <unistd.h>
#include <sys/types.h>
#include "Error/Error.hpp"

namespace Jetpack {
    class Process {
        public:
            class ProcessError : public Jetpack::Error {
                public:
                    ProcessError(const std::string &msg) : Jetpack::Error(msg) {}
            };

            static pid_t fork()
            {
                pid_t pid = ::fork();
                if (pid == -1)
                    throw ProcessError("fork operation failed");
                return pid;
            }
    };
}
