#include "event_manager.h"

#include <M5Unified.h>

#include "../components/button.h"
#include "../screens/screen_manager.h"

void EventManager::handleTouch(uint64_t *wakeupTime)
{
    auto t = M5.Touch.getDetail();

    if (t.wasPressed())
    {
        *wakeupTime = millis(); // Reset wakeup timer

        Button *btn = ComponentManager::getInstance()
                          .findTouchableComponentAtPosition<Button>(t.x, t.y);

        if (btn)
        {
            lastPressedButton = btn;
            btn->touch();
            setNeedsRender(true);
        }
    }

    if (t.wasReleased())
    {
        if (lastPressedButton)
        {
            lastPressedButton->release();
            lastPressedButton = nullptr;
            setNeedsRender(true);
        }
    }
}