#ifndef SCHEDULE_VIEW_H
#define SCHEDULE_VIEW_H

#include <Arduino.h>
#include <vector>

#include "../components/types.h"

const uint8_t MAX_SCHEDULE_LABELS = 10;

struct ScheduleEntry
{
    int8_t route;
    uint32_t expectedArriveTimestamp;
};

class ScheduleView
{
public:
    ScheduleView(Position pos, Size size) : position(pos), size(size) {};
    ~ScheduleView() {};

    void draw();
    void reset();

    void setScheduleData(const std::vector<ScheduleEntry> &entries)
    {
        this->entries = entries;
        dirty = true;
    };

private:
    std::vector<ScheduleEntry> entries;
    Position position;
    Size size;
    bool dirty = true;
};

#endif // SCHEDULE_VIEW_H