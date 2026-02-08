#pragma once

#include <Arduino.h>
#include <unordered_map>

#include "../std/string_hash.h"
#include "../ui/components/component.h"
#include "../utils/debug.h"

class ComponentManager
{
public:
    static ComponentManager &getInstance()
    {
        static ComponentManager instance;
        return instance;
    }

    ~ComponentManager()
    {
        unregisterAllComponents();
    }

    void registerComponent(const String &id, Component *component);
    void unregisterComponent(const String &id);
    void registerTouchableComponent(Component *component);
    void unregisterTouchableComponent(Component *component);
    void unregisterAllComponents();

    template <typename T>
    T *findComponentById(const String &id);
    template <typename T>
    T *findTouchableComponentAtPosition(int32_t x, int32_t y);

private:
    ComponentManager() : components() {}
    ComponentManager(const ComponentManager &) = delete;
    ComponentManager &operator=(const ComponentManager &) = delete;
    std::unordered_map<String, Component *> components;
    std::vector<Component *> touchableComponentList;
};

template <typename T>
T *ComponentManager::findComponentById(const String &id)
{
    DEBUG_PRINTF("Finding component with ID: %s\n", id.c_str());
    auto it = this->components.find(id);
    if (it != this->components.end())
    {
        DEBUG_PRINTF("Component with ID: %s found\n", id.c_str());
        return static_cast<T *>(it->second);
    }

    DEBUG_PRINTF("Component with ID: %s not found\n", id.c_str());
    return nullptr;
}

template <typename T>
T *ComponentManager::findTouchableComponentAtPosition(int32_t x, int32_t y)
{
    // Iterate in REVERSE - last rendered is on top
    for (auto it = touchableComponentList.rbegin();
         it != touchableComponentList.rend(); ++it)
    {
        auto component = *it;
        if (component->isVisible() && component->isTouched(x, y))
        {
            return static_cast<T *>(component);
        }
    }
    return nullptr;
}