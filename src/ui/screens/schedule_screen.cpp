#include "schedule_screen.h"

#include "../../api/transport/transport_api.h"
#include "../../utils/datetime_utils.h"

void ScheduleScreen::requestData()
{
    if (isRequestInProgress)
    {
        Serial.println("Request already in progress, skipping...");
        return;
    }

    auto &screenManager = ScreenManager::getInstance();
    auto &transportApi = TransportApi::getInstance();

    isRequestInProgress = true;
    GetTransportRequest request;
    auto response = transportApi.getTransportData(&request);

    if (response && response->isSuccess())
    {
        TransportTime *times = response->getTransportTimes();
        auto count = response->getTransportTimesCount();
        std::vector<ScheduleEntry> scheduleEntries;
        for (int i = 0; i < count; i++)
        {
            ScheduleEntry entry;
            entry.route = times[i].route;
            entry.expectedArriveTimestamp = times[i].expectedArriveTimestamp;
            scheduleEntries.push_back(entry);
        }

        this->scheduleView->setScheduleData(scheduleEntries);

        delete response;

        String timeStr = timestampToDatetime(getUtcTime());
        screenManager.setStatus("Refresh time: " + timeStr);
    }
    else
    {
        if (response)
        {
            Serial.print("API Request failed with status code: ");
            Serial.println(response->getStatusCode());
            Serial.print("Response body: ");
            Serial.println(response->getResponseBody());
            screenManager.setStatus("API Request Failed: " + response->getErrorDescription());
            delete response;
        }
        else
        {
            Serial.println("API Request failed: No response received");
            screenManager.setStatus("API Request Failed");
        }
    }
    isRequestInProgress = false;
}