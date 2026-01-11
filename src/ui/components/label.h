#pragma once

#include <Arduino.h>

#include "component.h"

enum TextSize
{
    SMALL = 2,
    MEDIUM = 3,
    LARGE = 5
};

class Label : public Component
{
public:
    Label(const String &value, const Position &position, const Size &size, TextSize textSize = TextSize::SMALL)
        : Component(position, size), value(value), textSize(textSize) {};
    Label(const String &value, const Size &size, TextSize textSize = TextSize::SMALL)
        : Component(size), value(value), textSize(textSize) {};
    Label(const String &id) : Component(id) {};
    virtual ~Label() {};

    void setValue(const String &newValue)
    {
        if (this->value != newValue)
        {
            this->value = newValue;
            this->setNeedsRender();
        }
    }
    void doRender() override;

private:
    String value;
    TextSize textSize = TextSize::SMALL;
};
