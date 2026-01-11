#pragma once
#include <Arduino.h>
#include <functional>

namespace std
{
    template <>
    struct hash<String>
    {
        size_t operator()(const String &s) const noexcept
        {
            return std::hash<std::string>{}(s.c_str());
        }
    };
}