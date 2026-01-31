#pragma once

#include "../base_response.h"
#include <Arduino.h>
#include <ArduinoJson.h>

struct TransportTime
{
  int route;
  String transportType;
  uint32_t expectedArriveTimestamp;
  uint32_t scheduledArriveTimestamp;
};

class GetTransportResponse : public BaseResponse
{
public:
  GetTransportResponse(int statusCode, String responseBody)
      : BaseResponse(statusCode, responseBody)
  {
    this->transportTimesCount = 0;
    // Parse JSON - JsonDocument automatically manages memory in ArduinoJson 7
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, responseBody);

    if (error)
    {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      this->setErrorDescription(error.c_str());
      this->setIsValid(false);
      this->setIsSuccessValue(false);
      return;
    }

    bool success = doc["success"] | false;
    if (!success)
    {
      Serial.println("API indicates failure");
      this->setErrorDescription("API indicated failure");
      this->setIsSuccessValue(false);
      return;
    }

    this->setIsSuccessValue(true);
    // Check if data exists
    if (!doc["data"].is<JsonObject>())
    {
      Serial.println("JSON missing data object");
      this->setIsValid(false);
      this->setErrorDescription("Missing data object");
      return;
    }

    JsonObject dataObj = doc["data"].as<JsonObject>();

    if (success && dataObj["stopDepartures"].isNull())
    {
      Serial.println("No data, but API indicates success, this could be valid");
      this->setIsValid(true);
      return;
    }

    // Check if stopDepartures exists and is an object
    if (!dataObj["stopDepartures"].is<JsonArray>())
    {
      Serial.println("JSON missing stopDepartures array");
      this->setIsValid(false);
      this->setErrorDescription("Missing stopDepartures array");
      return;
    }

    this->setIsValid(true);

    JsonArray stopDepartures = dataObj["stopDepartures"].as<JsonArray>();

    // Parse each transport time in the array
    for (JsonObject stopDeparture : stopDepartures)
    {
      if (this->transportTimesCount >= MAX_TRANSPORT_TIMES)
      {
        Serial.println("Reached max transport times limit");
        return;
      }

      this->transportTimes[transportTimesCount].route = stopDeparture["route"] | -1;
      this->transportTimes[transportTimesCount].transportType = stopDeparture["transportType"] | "";
      this->transportTimes[transportTimesCount].expectedArriveTimestamp =
          stopDeparture["expectedArriveTimestamp"] | 0;
      this->transportTimes[transportTimesCount].scheduledArriveTimestamp =
          stopDeparture["scheduledArriveTimestamp"] | 0;
      this->transportTimesCount++;
    }
  };

  TransportTime *getTransportTimes() { return transportTimes; }
  int getTransportTimesCount() { return transportTimesCount; }

private:
  static const int MAX_TRANSPORT_TIMES = 50;
  TransportTime transportTimes[MAX_TRANSPORT_TIMES];
  int transportTimesCount = 0;
};