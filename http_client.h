#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "network/base_request.h"
#include "network/base_response.h"
#include <HTTPClient.h>

BaseResponse *doRequest(BaseRequest *request);

#endif
