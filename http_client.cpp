#include "http_client.h"
#include <ArduinoJson.h>

BaseResponse *doRequest(BaseRequest *request)
{
  HTTPClient http;
  statusText = "Sending...";
  textDirty = true;

  // Start connection
  http.begin(SECRET_API_URL + path);
  http.addHeader("Content-Type", "application/json");

  // 3. Create JSON Payload (Manual string creation for simplicity)
  // If complex, use ArduinoJson library

  JsonDocument doc;
  String jsonPayload = "{\"device\":\"M5Paper\",\"battery\":" + String(M5.Power.getBatteryLevel()) + "}";

  // 4. Send POST Request
  int httpResponseCode = http.POST(jsonPayload);

  // 5. Handle Response
  if (httpResponseCode > 0)
  {
    String response = http.getString(); // Get the response from server
    statusText = "Success: " + String(httpResponseCode);
    // Serial.println(response); // Debugging
  }
  else
  {
    statusText = "Error: " + String(httpResponseCode);
  }

  // 6. Close connection
  http.end();
}
