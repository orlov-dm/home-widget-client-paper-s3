#include "schedule_view.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <memory>
#include <utils/datetime_utils.h>

#include "../components/label.h"
#include "../components/button.h"

ScheduleView::ScheduleView(const String &id) : ViewBase<ScheduleView>(id)
{
    auto buttonLayout = std::make_unique<View>(
        Size{0, 40}, LayoutDirection::Horizontal);
    buttonLayout->setSpacing(10);
    auto buttonPrevPage = std::make_unique<Button>("<");
    this->buttonPrevPage = buttonPrevPage.get();
    auto buttonNextPage = std::make_unique<Button>(">");
    this->buttonNextPage = buttonNextPage.get();
    buttonPrevPage->onTouch([this]()
                            { 
                                if (this->currentPage > 0)
                                {
                                    this->currentPage--;
                                    if (this->currentPage == 0)
                                    {
                                        this->buttonPrevPage->setDisabled(true);
                                    }
                                    if (this->buttonNextPage->isDisabled())
                                    {
                                        this->buttonNextPage->setDisabled(false);
                                    }
                                    this->renderPage();
                                } });
    buttonNextPage->onTouch([this]()
                            { 
                                if ((this->currentPage + 1) * PAGE_COUNT < this->entries.size())
                                {
                                    this->currentPage++;
                                    if ((this->currentPage + 1) * PAGE_COUNT >= this->entries.size())
                                    {
                                        this->buttonNextPage->setDisabled(true);
                                    }
                                    if (this->buttonPrevPage->isDisabled())
                                    {
                                        this->buttonPrevPage->setDisabled(false);
                                    }
                                    this->renderPage();
                                } });

    auto container = std::make_unique<View>(
        Size{0, 0}, LayoutDirection::Vertical);
    container->setSeparatorSize({0, 2});
    this->container = container.get();

    buttonLayout->addChild(std::move(buttonPrevPage));
    buttonLayout->addChild(std::move(buttonNextPage));
    this->addChild(std::move(container));
    this->addChild(std::move(buttonLayout));
}

void ScheduleView::setScheduleData(const std::vector<ScheduleEntry> &entries)
{
    this->entries = entries;

    this->resetPagination();
    this->renderPage();
}

void ScheduleView::renderPage()
{
    this->container->resetChildren();

    if (this->entries.empty())
    {
        auto noDataLabel = std::make_unique<Label>("No schedule data available", Size{0, 50}, TextSize::LARGE);
        this->container->addChild(std::move(noDataLabel));
        this->setNeedsRender();
        return;
    }

    auto viewSize = this->container->getSize();
    Serial.printf("ScheduleView size: w=%d, h=%d\n", viewSize.w, viewSize.h);
    int32_t rowHeight = 70;
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
        row->setName("ScheduleRow_" + String(index));
        row->setPadding(10);
        String routeStr = String(entry.route) + ":";
        Serial.println("Route: " + routeStr + ", Expected Arrival: " + String(entry.expectedArriveTimestamp));
        auto routeLabel = std::make_unique<Label>(routeStr, Size{routeLabelWidth, 0}, TextSize::LARGE);
        routeLabel->setName("RouteLabel_" + String(index));
        row->addChild(std::move(routeLabel));

        auto inLabel = std::make_unique<Label>("In", Size{inLabelWidth, 0}, TextSize::MEDIUM);
        inLabel->setName("InLabel_" + String(index));
        row->addChild(std::move(inLabel));

        auto expectedArriveTimestamp = entry.expectedArriveTimestamp;
        int secondsDiff = expectedArriveTimestamp - utcTime;
        String secondsDiffString = String(secondsDiff / 60);
        auto secondsDiffLabel = std::make_unique<Label>(secondsDiffString, Size{timeLabelWidth, 0}, TextSize::LARGE);
        secondsDiffLabel->setName("SecondsDiffLabel_" + String(index));
        row->addChild(std::move(secondsDiffLabel));

        auto minLabel = std::make_unique<Label>("min", Size{minLabelWidth, 0}, TextSize::MEDIUM);
        minLabel->setName("MinLabel_" + String(index));
        row->addChild(std::move(minLabel));

        auto expectedTimeLabel = std::make_unique<Label>(
            formatTimestampToLocalTimeString(expectedArriveTimestamp, "%H:%M"),
            Size{viewSize.w - (routeLabelWidth + 30 + inLabelWidth + timeLabelWidth + minLabelWidth + 20), 0},
            TextSize::LARGE);
        expectedTimeLabel->setName("ExpectedTimeLabel_" + String(index));
        row->addChild(std::move(expectedTimeLabel));

        this->container->addChild(std::move(row));

        ++index;
    }
    this->container->setNeedsRender();
}

void ScheduleView::resetPagination()
{
    this->currentPage = 0;
    this->buttonNextPage->setNeedsRender();
    this->buttonPrevPage->setNeedsRender();
    if (this->buttonPrevPage)
    {
        this->buttonPrevPage->setDisabled(true);
    }
    if (this->buttonNextPage)
    {
        if (this->entries.size() > PAGE_COUNT)
        {
            this->buttonNextPage->setDisabled(false);
        }
        else
        {
            this->buttonNextPage->setDisabled(true);
        }
    }
}
