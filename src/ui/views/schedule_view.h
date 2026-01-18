#pragma once

#include <Arduino.h>
#include <vector>

#include "../types.h"
#include "view.h"
#include "../components/button.h"

const uint8_t MAX_SCHEDULE_LABELS = 10;

struct ScheduleEntry
{
    int8_t route;
    uint32_t expectedArriveTimestamp;
};

class ScheduleViewRow : public ViewBase<ScheduleViewRow>
{
public:
    ScheduleViewRow(const Size &size) : ViewBase<ScheduleViewRow>(size, LayoutDirection::Horizontal) {};
};

class ScheduleView : public ViewBase<ScheduleView>
{
public:
    ScheduleView(const String &id);

    void setScheduleData(const std::vector<ScheduleEntry> &entries);

private:
    std::vector<ScheduleEntry> entries;

    View *container = nullptr;
    Button *buttonNextPage = nullptr;
    Button *buttonPrevPage = nullptr;
};
