
#pragma once

#include <Arduino.h>

class Button;

class EventManager
{
public:
    static EventManager &getInstance()
    {
        static EventManager instance;
        return instance;
    }

    void handleTouch(uint64_t *wakeupTime);
    bool detectAnyTouch();
    bool needsRender() const
    {
        return shouldRender;
    }
    void setNeedsRender(bool needsRender)
    {
        shouldRender = needsRender;
    }

private:
    bool shouldRender = false;
    Button *lastPressedButton = nullptr;
};