/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** File
*/

#pragma once
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "Error/Error.hpp"

namespace Jetpack {
    class File {
        public:
            class FileError : public Jetpack::Error {
            public:
                FileError(const std::string &msg) : Jetpack::Error(msg) {}
            };

            static void stat(const std::string &filePath, struct stat *fileStat)
            {
                if (::stat(filePath.c_str(), fileStat) == -1)
                    throw FileError("stat failed: " + filePath);
            }

            static void access(const std::string &filePath, int mode)
            {
                if (::access(filePath.c_str(), mode) == -1)
                    throw FileError("access failed: " + filePath);
            }
    };
}
