#pragma once

#include <Arduino.h>

#include "component.h"

enum TextSize
{
    SMALL = 2,
    MEDIUM = 3,
    LARGE = 5,
    XLARGE = 7
};

enum TextAlignment
{
    TA_LEFT,
    TA_CENTER,
    TA_RIGHT
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
        if (this->value == newValue)
            return;
        this->value = newValue;
        this->setNeedsRender();
    }
    void setTextAlignment(TextAlignment align)
    {
        if (this->alignment == align)
            return;
        this->alignment = align;
        this->setNeedsRender();
    }

    void doRender() override;

protected:
    void init();

private:
    String value;
    TextSize textSize = TextSize::SMALL;
    TextAlignment alignment = TextAlignment::TA_LEFT;
};
