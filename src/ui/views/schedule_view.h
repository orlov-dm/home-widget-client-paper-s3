#pragma once

#include <Arduino.h>
#include <vector>

#include "../components/types.h"
#include "view.h"

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
    ScheduleView(const Position &pos, const Size &size) : ViewBase<ScheduleView>(pos, size) {};

    void setScheduleData(const std::vector<ScheduleEntry> &entries);

private:
    std::vector<ScheduleEntry> entries;
};
