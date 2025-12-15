#include "..\..\Request.h"

class GetTransportRequest : public BaseRequest {
public:
  GetTransportRequest() {
    method = "POST";
    url = "/buses/get";
  }
};
