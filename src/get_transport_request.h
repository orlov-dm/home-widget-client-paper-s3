#ifndef GET_TRANSPORT_REQUEST_H
#define GET_TRANSPORT_REQUEST_H

#include "api_const.h"
#include "base_request.h"

class GetTransportRequest : public BaseRequest {
public:
  GetTransportRequest() {
    this->setMethod("POST");
    this->setUrl(URL_TRANSPORT_GET);
    // If you need to send a body, set it here:
    // this->setBody("{\"stopId\":\"123\"}");
  }
};

#endif
