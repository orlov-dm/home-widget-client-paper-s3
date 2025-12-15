#include "base_response.h"

BaseResponse::deserialize(String json)
{
    JsonDocument doc;
    doc.fromJson(json);
    return doc;
}