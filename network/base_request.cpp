#include "base_request.h"

JsonDocument BaseRequest::serialize()
{
    JsonDocument doc;
    doc.toJson("{}");
    return doc;
}
