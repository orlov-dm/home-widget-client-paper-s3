#pragma once

#include <Arduino.h>

#include "../../icons/index.h"
#include "component.h"

class Button : public Component
{
public:
    Button(const String &label, const Position &position, const Size &size, Icon icon = ICON_NONE) : Component(position, size), label(label), icon(icon) {};

    void doRender() override;

    void setPressed(bool isPressed)
    {
        if (this->isPressedState != isPressed)
        {
            this->isPressedState = isPressed;
            this->setNeedsRender();
        }
    }

    bool isPressed() const
    {
        return this->isPressedState;
    }

private:
    String label;
    bool isPressedState = false;
    Icon icon = ICON_NONE;
};
