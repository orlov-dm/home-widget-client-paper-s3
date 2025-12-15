#ifndef GET_TRANSPORT_RESPONSE_H
#define GET_TRANSPORT_RESPONSE_H

#include "Response.h"
#include <ArduinoJson.h>

struct TransportTime {
    int route;
    string transportType;
    uint32_t expectedArriveTimestamp;
    uint32_t scheduledArriveTimestamp;
}

struct StopDepartures {
    TransportTime transportTimes[];
}


class GetTransportResponse : public BaseResponse {  
    public:
        GetTransportResponse(String json) {
            statusCode = -1;
            responseBody = "";
            data = JsonDocument(json);
        }

        GetTransportResponse deserialize(String json) override {
            JsonDocument doc;
            doc.fromJson(json);

            
            return GetTransportResponse(json);
        }
};

#endif