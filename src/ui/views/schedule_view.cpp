#include "schedule_view.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <memory>
#include <utils/datetime_utils.h>

#include "../components/label.h"
#include "../components/button.h"

ScheduleView::ScheduleView(const String &id) : ViewBase<ScheduleView>(id)
{
    auto buttonPrevPage = std::make_unique<Button>("Prev");
    buttonPrevPage->setHeight(20);
    this->buttonPrevPage = buttonPrevPage.get();
    buttonPrevPage->onTouch([this]()
                            { 
                                if (this->currentPage > 0)
                                {
                                    this->currentPage--;
                                    this->renderPage();
                                } });
    auto buttonNextPage = std::make_unique<Button>("Next");
    buttonNextPage->setHeight(20);
    buttonNextPage->onTouch([this]()
                            { 
                                if ((this->currentPage + 1) * PAGE_COUNT < this->entries.size())
                                {
                                    this->currentPage++;
                                    this->renderPage();
                                } });
    this->buttonNextPage = buttonNextPage.get();
    auto container = std::make_unique<View>(
        Size{0, 0}, LayoutDirection::Vertical);
    container->setSeparatorSize({0, 2});
    this->container = container.get();

    this->addChild(std::move(buttonPrevPage));
    this->addChild(std::move(container));
    this->addChild(std::move(buttonNextPage));
}

void ScheduleView::setScheduleData(const std::vector<ScheduleEntry> &entries)
{
    this->entries = entries;

    this->currentPage = 0;
    this->renderPage();
}

void ScheduleView::renderPage()
{
    this->container->resetChildren();
    auto viewSize = this->getSize();
    Serial.printf("ScheduleView size: w=%d, h=%d\n", viewSize.w, viewSize.h);
    int32_t rowHeight = 80;
    int32_t routeLabelWidth = 90;
    int32_t inLabelWidth = 60;
    int32_t timeLabelWidth = 70;
    int32_t minLabelWidth = 80;
    int index = 0;
    time_t utcTime = getUtcTime();

    size_t recordsStartIndex = this->currentPage * PAGE_COUNT;
    size_t recordsEndIndex = std::min(recordsStartIndex + PAGE_COUNT, this->entries.size());

    for (size_t i = recordsStartIndex; i < recordsEndIndex; ++i)
    {
        if (i >= this->entries.size())
        {
            break;
        }
        const auto &entry = this->entries[i];
        Serial.printf("Processing entry %d: route=%d, expectedArriveTimestamp=%u\n", i, entry.route, entry.expectedArriveTimestamp);
        auto row = std::make_unique<ScheduleViewRow>(Size{0, rowHeight});
        row->setPadding(10);
        String routeStr = String(entry.route) + ":";
        Serial.println("Route: " + routeStr + ", Expected Arrival: " + String(entry.expectedArriveTimestamp));
        row->addChild(std::make_unique<Label>(routeStr, Size{routeLabelWidth, 0}, TextSize::LARGE));

        row->addChild(std::make_unique<Label>("In", Size{inLabelWidth, 0}, TextSize::MEDIUM));

        auto expectedArriveTimestamp = entry.expectedArriveTimestamp;
        int secondsDiff = expectedArriveTimestamp - utcTime;
        String secondsDiffString = String(secondsDiff / 60);
        row->addChild(std::make_unique<Label>(secondsDiffString, Size{timeLabelWidth, 0}, TextSize::LARGE));

        row->addChild(std::make_unique<Label>("min", Size{minLabelWidth, 0}, TextSize::MEDIUM));

        row->addChild(std::make_unique<Label>(
            formatTimestampToLocalTimeString(expectedArriveTimestamp, "%H:%M"),
            Size{viewSize.w - (routeLabelWidth + 30 + inLabelWidth + timeLabelWidth + minLabelWidth + 20), 0},
            TextSize::LARGE));

        this->container->addChild(std::move(row));

        ++index;
    }
    this->setNeedsRender();
}
