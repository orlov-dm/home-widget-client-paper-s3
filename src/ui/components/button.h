#pragma once

#include <Arduino.h>

#include "../../icons/index.h"
#include "component.h"

class Button : public Component
{
public:
    Button(const String &label, const Size &size, const String &id, Icon icon = ICON_NONE) : Component(size, id), label(label), icon(icon) { this->init(); };
    Button(const String &label, const String &id, Icon icon = ICON_NONE) : Component(id), label(label), icon(icon) { this->init(); };
    Button(const String &label, const String &id) : Component(id), label(label) { this->init(); };
    Button(const String &label) : Component(), label(label) { this->init(); };
    virtual ~Button() { this->destroy(); };

    void doRender() override;

    void setPressed(bool isPressed);
    bool isPressed() const { return this->isPressedState; }

    void touch();
    void release();

    void onTouch(std::function<void()> callback)
    {
        this->touchCallback = callback;
    }
    void onRelease(std::function<void()> callback)
    {
        this->releaseCallback = callback;
    }

protected:
    void init();
    void destroy();

private:
    String label;
    bool isPressedState = false;
    Icon icon = ICON_NONE;

    std::function<void()> touchCallback;
    std::function<void()> releaseCallback;
};
