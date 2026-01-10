#pragma once

#include <Arduino.h>

#include "../types.h"
#include "view.h"
#include "../components/label.h"

class StatusBar : public ViewBase<StatusBar>
{
public:
    StatusBar(const Position &pos, const Size &size, String initialValue = "")
        : ViewBase<StatusBar>(pos, size, LayoutDirection::Horizontal),
          label(nullptr),
          batteryLabel(nullptr)
    {
        this->setBackgroundColor(TFT_LIGHTGREY);

        auto labelPtr = std::make_unique<Label>(initialValue, Size{0, 0}, TextSize::SMALL);
        this->label = labelPtr.get();
        this->addChild(std::move(labelPtr));

        auto batteryLabelPtr = std::make_unique<Label>("", Size{60, 0}, TextSize::SMALL);
        this->batteryLabel = batteryLabelPtr.get();
        this->addChild(std::move(batteryLabelPtr));

        this->refreshBatteryLevel();

        this->setPadding(5);
    }

    void setValue(String newValue);

private:
    void refreshBatteryLevel();

    Label *label;
    Label *batteryLabel;
};
