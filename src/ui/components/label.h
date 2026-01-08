#pragma once

#include <Arduino.h>

#include "component.h"

class Label : public Component
{
public:
    Label(const String &value, const Position &position, const Size &size, int8_t textSize)
        : Component(position, size), value(value), textSize(textSize) {};
    Label(const String &value, const Size &size, int8_t textSize)
        : Component(size), value(value), textSize(textSize) {};

    void doRender() override;

private:
    String value;
    int8_t textSize;
    bool dirty;
};
