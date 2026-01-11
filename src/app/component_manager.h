#pragma once

#include <Arduino.h>
#include <unordered_map>

#include "../std/string_hash.h"

class Component;

class ComponentManager
{
public:
    static ComponentManager &getInstance()
    {
        static ComponentManager instance;
        return instance;
    }

    void registerComponent(const String &id, Component *component);
    void unregisterComponent(const String &id);

    template <typename T>
    T *findComponentById(const String &id)
    {
        Serial.printf("Finding component with ID: %s\n", id.c_str());
        auto it = this->components.find(id);
        if (it != this->components.end())
        {
            Serial.printf("Component with ID: %s found\n", id.c_str());
            return static_cast<T *>(it->second);
        }

        Serial.printf("Component with ID: %s not found\n", id.c_str());
        return nullptr;
    }

private:
    ComponentManager() : components() {}
    ComponentManager(const ComponentManager &) = delete;
    ComponentManager &operator=(const ComponentManager &) = delete;
    std::unordered_map<String, Component *> components;
};