#include "component_manager.h"

void ComponentManager::registerComponent(const String &id, Component *component)
{
    Serial.printf("Registering component with ID: %s\n", id.c_str());
    this->components[id] = component;
}

void ComponentManager::unregisterComponent(const String &id)
{
    Serial.printf("Unregistering component with ID: %s\n", id.c_str());
    this->components.erase(id);
}

void ComponentManager::registerTouchableComponent(Component *component)
{
    this->touchableComponentList.push_back(component);
}

void ComponentManager::unregisterTouchableComponent(Component *component)
{
    this->touchableComponentList.erase(
        std::remove(this->touchableComponentList.begin(), this->touchableComponentList.end(), component),
        this->touchableComponentList.end());
}

void ComponentManager::unregisterAllComponents()
{
    this->components.clear();
    this->touchableComponentList.clear();
}
