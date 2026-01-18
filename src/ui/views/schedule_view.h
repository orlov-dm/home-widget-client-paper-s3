#pragma once

#include <Arduino.h>
#include <vector>

#include "../types.h"
#include "view.h"
#include "../components/button.h"

const uint8_t PAGE_COUNT = 10;

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

protected:
    void renderPage();

private:
    std::vector<ScheduleEntry> entries;

    View *container = nullptr;
    Button *buttonNextPage = nullptr;
    Button *buttonPrevPage = nullptr;

    int8_t currentPage = 0;
};
