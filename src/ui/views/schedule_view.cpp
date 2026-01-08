#include "schedule_view.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <utils/datetime_utils.h>

#include "../components/label.h"

void ScheduleView::reset()
{
    this->entries.clear();
    this->resetChildren();
}

void ScheduleView::setScheduleData(const std::vector<ScheduleEntry> &entries)
{
    this->entries = entries;

    auto viewSize = this->getSize();
    int32_t rowHeight = 80;
    int32_t routeLabelWidth = 90;
    int32_t inLabelWidth = 60;
    int32_t timeLabelWidth = 70;
    int32_t minLabelWidth = 80;
    int index = 0;
    time_t utcTime = getUtcTime();

    int8_t smallerTextSize = 3;
    int8_t largerTextSize = 5;
    for (auto &entry : this->entries)
    {
        if (index >= MAX_SCHEDULE_LABELS)
        {
            Serial.println("Max schedule labels reached, skipping remaining entries");
            break;
        }

        ScheduleViewRow *row = (new ScheduleViewRow(Size{0, rowHeight}))->setPadding(10);
        String routeStr = String(entry.route) + ":";
        Serial.println("Route: " + routeStr + ", Expected Arrival: " + String(entry.expectedArriveTimestamp));
        row->addChild(new Label(routeStr, {routeLabelWidth, 0}, largerTextSize));

        row->addChild(new Label("In", {inLabelWidth, 0}, smallerTextSize));

        auto expectedArriveTimestamp = entry.expectedArriveTimestamp;
        int secondsDiff = expectedArriveTimestamp - utcTime;
        String secondsDiffString = String(secondsDiff / 60);
        row->addChild(new Label(secondsDiffString, {timeLabelWidth, 0}, largerTextSize));

        row->addChild(new Label("min", {minLabelWidth, 0}, smallerTextSize));

        row->addChild(new Label(
            formatTimestampToLocalTimeString(expectedArriveTimestamp, "%H:%M"),
            {viewSize.w - (routeLabelWidth + 30 + inLabelWidth + timeLabelWidth + minLabelWidth + 20), 0},
            largerTextSize));

        this->addChild(row);

        ++index;
    }
}
