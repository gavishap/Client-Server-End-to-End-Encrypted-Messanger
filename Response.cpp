#include "Response.h"


unsigned int Response::fromBytes(const unsigned char* payload, int offset, int sizeOfBytes)
{
    unsigned int value = 0;
    for (int i = 0; i < sizeOfBytes; ++i)
    {
        value += payload[i + offset] << i * 8;
    }
    return value;
}

std::shared_ptr<Response> Response::CreateResponse(const unsigned char* header, const unsigned char* payload)
{
    char version;
    short code;
    int size;
    version = fromBytes(header, 0, 1);
    code = fromBytes(header, 1, 2);
    size = fromBytes(header, 3, 4) - 7;

    if (code == 2000)
    {
        return std::make_shared<RegisterSuccessResponse>(version, code, payload, size);
    }
    else if (code == 2001)
    {
        return std::make_shared<UsersListResponse>(version, code, payload, size);
    }
    else if (code == 2002)
    {
        return std::make_shared<ClientPublicKeyResponse>(version, code, payload, size);
    }
    else if (code == 2003)
    {
        return std::make_shared<SentMessageResponse>(version, code, payload, size);
    }
    else if (code == 2004)
    {
        return std::make_shared<PullMessagesResponse>(version, code, payload, size);
    }
    else if (code == 9000)
    {
        return std::make_shared<ErrorResponse>(version, code);
    }
    return {};
}
