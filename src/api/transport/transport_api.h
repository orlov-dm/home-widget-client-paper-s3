#pragma once

#include <Arduino.h>
#include <vector>

#include "get_transport_request.h"
#include "get_transport_response.h"
#include "../api_client.h"

class TransportApi
{
public:
    static TransportApi &getInstance()
    {
        static TransportApi instance;
        return instance;
    }

    GetTransportResponse *getTransportData(GetTransportRequest *request)
    {
        return client.doRequest<GetTransportResponse>(request);
    }

private:
    TransportApi() {}
    TransportApi(const TransportApi &) = delete;
    TransportApi &operator=(const TransportApi &) = delete;

    ApiClient &client = ApiClient::getInstance();
};