#pragma once

#include <Arduino.h>

#include "../../icons/index.h"
#include "component.h"

class Button : public Component
{
public:
    Button(const String &label, const Size &size, const String &id, Icon icon = ICON_NONE) : Component(size, id), label(label), icon(icon) {};
    Button(const String &label, const String &id, Icon icon = ICON_NONE) : Component(id), label(label), icon(icon) {};
    Button(const String &label, const String &id) : Component(id), label(label) {};
    Button(const String &label) : Component(), label(label) {};
    virtual ~Button() {};

    void doRender() override;

    void setPressed(bool isPressed);
    bool isPressed() const { return this->isPressedState; }

private:
    String label;
    bool isPressedState = false;
    Icon icon = ICON_NONE;
};
