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
