#ifndef GET_TRANSPORT_RESPONSE_H
#define GET_TRANSPORT_RESPONSE_H

#include "base_response.h"
#include <Arduino.h>
#include <ArduinoJson.h>

struct TransportTime {
  int route;
  String transportType;
  uint32_t expectedArriveTimestamp;
  uint32_t scheduledArriveTimestamp;
};

class GetTransportResponse : public BaseResponse {
public:
  GetTransportResponse(int statusCode, String responseBody)
      : BaseResponse(statusCode, responseBody) {
    // Parse JSON - JsonDocument automatically manages memory in ArduinoJson 7
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, responseBody);

    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      isValid = false;
      success = false;
      return;
    }

    // Parse success field
    success = doc["success"] | false;

    // Check if data exists
    if (!doc["data"].is<JsonObject>()) {
      Serial.println("JSON missing data object");
      isValid = false;
      transportTimesCount = 0;
      return;
    }

    JsonObject dataObj = doc["data"].as<JsonObject>();

    // Check if stopDepartures exists and is an object
    if (!dataObj["stopDepartures"].is<JsonArray>()) {
      Serial.println("JSON missing stopDepartures array");
      isValid = false;
      transportTimesCount = 0;
      return;
    }

    isValid = true;
    transportTimesCount = 0;

    // Parse stopDepartures as a JSON object (e.g., {"bus_24": [...], "bus_83": [...]})
    JsonArray stopDepartures = dataObj["stopDepartures"].as<JsonArray>();

    // Iterate through all keys in stopDepartures (bus_24, bus_83, etc.)
        // Parse each transport time in the array
    for (JsonObject stopDeparture : stopDepartures) {
      if (transportTimesCount >= MAX_TRANSPORT_TIMES) {
        Serial.println("Reached max transport times limit");
        return;
      }

      this->transportTimes[transportTimesCount].route = stopDeparture["route"] | -1;
      this->transportTimes[transportTimesCount].transportType = stopDeparture["transportType"] | "";
      this->transportTimes[transportTimesCount].expectedArriveTimestamp =
          stopDeparture["expectedArriveTimestamp"] | 0;
      this->transportTimes[transportTimesCount].scheduledArriveTimestamp =
          stopDeparture["scheduledArriveTimestamp"] | 0;
      transportTimesCount++;
    }
  };

  bool isSuccess() {
    return isValid && success && getStatusCode() >= 200 && getStatusCode() < 300;
  }

  bool getSuccess() { return success; }

  TransportTime *getTransportTimes() { return transportTimes; }
  int getTransportTimesCount() { return transportTimesCount; }

private:
  static const int MAX_TRANSPORT_TIMES = 50; // Adjust based on your needs
  TransportTime transportTimes[MAX_TRANSPORT_TIMES];
  int transportTimesCount = 0;
  bool isValid = false;
  bool success = false;
};

#endif