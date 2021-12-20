#include "Message.h"

//std::shared_ptr<Message> Message::CreateMessage(const char* messagePayload)
//{
//    char type;
//    unsigned char* offset;
//    int contentSize;
//    offset = const_cast<unsigned char*>(messagePayload);
//    std::copy(offset, offset + sizeof(type), &type);
//    offset += sizeof(type);
//    std::copy(offset, offset + sizeof(contentSize), &contentSize);
//    offset += sizeof(contentSize);
//    if (type == 1) return std::make_shared<RequestSymmetricKeyMessage>();
//    else if (type == 2) return std::make_shared<SendSymmetricKeyMessage>(contentSize, reinterpret_cast<char*>(offset));
//    else if (type == 3) return std::make_shared<RequestSymmetricKeyMessage>();
//
//    return std::shared_ptr<Message>();
//}


unsigned int Message::fromBytesToVal(const unsigned char* payload, int offset, int sizeOfBytes)
{
	unsigned int value = 0;
	for (int i = 0; i < sizeOfBytes; ++i)
	{
		value += payload[i + offset] << i * 8;
	}
	return value;
}
