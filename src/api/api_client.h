// api_client.h
#ifndef API_CLIENT_H
#define API_CLIENT_H

#include "base_request.h"
#include "../secrets.h"
#include <HTTPClient.h>
#include "utils/wifi_utils.h"

class ApiClient
{
public:
  template <typename TResponse>
  TResponse *doRequest(BaseRequest *request)
  {
    if (!request)
    {
      Serial.println("Error: Request is null");
      return nullptr;
    }

    if (!isWifiConnected())
    {
      auto wifiStatus = wifiConnect();
      if (wifiStatus != WifiConnectionStatus::CONNECTED)
      {
        Serial.println("Error: Failed to reconnect WiFi");
        return nullptr;
      }
    }

    // Start connection
    Serial.println(request->getUrl() + " start");
    String fullUrl = SECRET_API_URL + request->getUrl();
    this->http.begin(fullUrl);
    this->http.addHeader("Content-Type", "application/json");
    this->http.addHeader("x-api-key", SECRET_API_KEY);
    this->http.addHeader("CF-Access-Client-Id", SECRET_CF_ACCESS_CLIENT_ID);
    this->http.addHeader("CF-Access-Client-Secret",
                         SECRET_CF_ACCESS_CLIENT_VALUE);

    // Send GET/POST Request
    String method = request->getMethod();
    int httpResponseCode = 0;
    if (method == "POST")
    {
      String jsonPayload = request->getBody();
      httpResponseCode = this->http.POST(jsonPayload);
    }
    else
    {
      httpResponseCode = this->http.GET();
    }

    TResponse *result = nullptr;
    try
    {
      // Handle Response
      if (httpResponseCode > 0)
      {
        String response = this->http.getString();
        result = new TResponse(httpResponseCode, response);
      }
      else
      {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
      }
      this->http.end();
      return result;
    }
    catch (...)
    {
      Serial.println("Exception occurred while processing response");
      // Close connection
      this->http.end();
      if (result)
      {
        delete result;
      }
      return nullptr;
    }
  }

private:
  HTTPClient http;
};

#endif