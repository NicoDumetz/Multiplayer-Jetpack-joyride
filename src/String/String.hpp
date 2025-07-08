/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** String
*/

#pragma once
#include <cstring>
#include "Error/Error.hpp"

namespace Jetpack {
    class String {
        public:
            class StringError : public Jetpack::Error {
                public:
                    StringError(const std::string &msg) : Jetpack::Error(msg) {}
            };

            static int strcmp(const char *s1, const char *s2)
            {
                if (!s1 || !s2)
                    throw StringError("strcmp received nullptr");
                return ::strcmp(s1, s2);
            }
    };
}
