#include "schedule_view.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <utils/datetime_utils.h>

#include "../components/label.h"

void ScheduleView::draw()
{
    if (!dirty)
    {
        return;
    }

    int32_t labelHeight = 80;
    int32_t routeLabelWidth = 80;
    int32_t inLabelWidth = 80;
    int32_t timeLabelWidth = 70;
    int32_t minLabelWidth = 80;
    int index = 0;
    time_t utcTime = getUtcTime();
    int32_t currentY = this->position.y + 10;
    int32_t currentX = this->position.x + 10;

    int8_t smallerTextSize = 3;
    int8_t largerTextSize = 5;
    for (const auto &entry : entries)
    {
        if (index >= MAX_SCHEDULE_LABELS)
        {
            Serial.println("Max schedule labels reached, skipping remaining entries");
            break;
        }
        currentY = this->position.y + labelHeight * index + 10 + 2 * index;
        String routeStr = String(entry.route) + ":";
        Serial.println("Route: " + routeStr + ", Expected Arrival: " + String(entry.expectedArriveTimestamp));

        Label routeLabel = {currentX, currentY, routeLabelWidth, labelHeight, routeStr, largerTextSize, true};

        Label inLabel = {currentX + routeLabelWidth + 40, currentY, inLabelWidth, labelHeight, "In ", smallerTextSize, true};

        auto expectedArriveTimestamp = entry.expectedArriveTimestamp;
        int secondsDiff = expectedArriveTimestamp - utcTime;
        String secondsDiffString = String(secondsDiff / 60);
        Label timeLabel = {currentX + routeLabelWidth + 40 + inLabelWidth, currentY, timeLabelWidth, labelHeight, secondsDiffString, largerTextSize, true};

        Label minLabel = {currentX + routeLabelWidth + 40 + inLabelWidth + timeLabelWidth, currentY, minLabelWidth, labelHeight, " min", smallerTextSize, true};

        drawLabel(routeLabel);
        drawLabel(inLabel);
        drawLabel(timeLabel);
        drawLabel(minLabel);

        M5.Display.fillRect(this->position.x + 20, currentY + labelHeight, this->size.w - 40, 2, TFT_DARKGRAY);
        index++;
    }

    if (currentY < this->position.y + this->size.h)
    {
        // Clear remaining area
        M5.Display.fillRect(this->position.x, currentY + labelHeight, this->size.w, (this->position.y + this->size.h) - (currentY + labelHeight), TFT_WHITE);
    }
    dirty = false;
}

void ScheduleView::reset()
{
    this->entries.clear();
    dirty = true;
}
