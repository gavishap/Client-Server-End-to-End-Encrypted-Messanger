#pragma once
#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include "Message.h" /* include Client inside*/


class Response
{
	char m_Version;
	short m_Code;
	int m_Payload_size;
	const unsigned char* m_payload = nullptr;

protected:
	Response(char Version, short Code, const unsigned char* payload, int payloadSize) : m_Version(Version), m_Code(Code), m_payload(payload), m_Payload_size(payloadSize){}
	~Response() {}

public:
	static unsigned int fromBytes(const unsigned char* payload, int offset, int sizeOfBytes);
	static std::shared_ptr<Response> CreateResponse(const unsigned char* header, const unsigned char* payload);
	short GetCode() { 
		if (this == nullptr)
			return 0;
		return m_Code; 
	}
};

class ErrorResponse : public Response
{
public:
	ErrorResponse(char Version, short Code) : Response(Version, Code, nullptr, 0) {}
};

class RegisterSuccessResponse : public Response
{
	ClientID m_ClientID;
public:
	RegisterSuccessResponse(char Version, short Code, const unsigned char* payload, int payloadSize) : Response(Version, Code, payload, payloadSize)
	{
		std::copy(payload, payload + 16, m_ClientID.begin());
	}


	std::string ClientIdAsString()
	{
		std::stringstream ss;
		for (int i = 0; i < 16; ++i)
		{
			ss << std::hex << (((unsigned char)m_ClientID[i] & 0xF0) >> 4);
			ss << std::hex << (((unsigned char)m_ClientID[i] & 0x0F) >> 0);
		}
		return  ss.str();
	}
};

class UsersListResponse : public Response
{

	std::vector<Client> m_UsersList;
public:
	UsersListResponse(char Version, short Code, const unsigned char* payload, int payloadSize) : Response(Version, Code, payload, payloadSize)
	{
		int clientsSize = payloadSize / (16 + 255);
		unsigned char* offset = const_cast<unsigned char*>(payload);
		for (int i = 0; i < clientsSize; i++, offset+= 16 + 255)
		{
			Client client;
			std::copy(offset, offset + 16, client.first.begin());
			std::copy(offset + 16, offset + 16 + 255, client.second.begin());
			m_UsersList.push_back(client);
		}
	}

	std::vector<Client> GetUsersList() { return m_UsersList; }
};
 
class ClientPublicKeyResponse : public Response
{
	std::array<char, 16> m_ClientID;
	ClientPublicKey m_PublicKey;
public:
	ClientPublicKeyResponse(char Version, short Code, const unsigned char* payload, int payloadSize) : Response(Version, Code, payload, payloadSize)
	{
		std::copy(payload, payload + 16, m_ClientID.begin());
		std::copy(payload + 16, payload + 16 + 160, m_PublicKey.begin());
	}

	ClientPublicKey GetPublicKey()
	{
		return m_PublicKey;
	}
};

class SentMessageResponse : public Response
{
	std::array<char, 16> m_ClientID;
	int m_MessageID;

public:
	SentMessageResponse(char Version, short Code, const unsigned char* payload, int payloadSize) : Response(Version, Code, payload, payloadSize)
	{
		std::copy(payload, payload + 16, m_ClientID.begin());
		m_MessageID = fromBytes(payload, 16, 4);
	}
};


class PullMessagesResponse : public Response
{
	
	std::vector<std::shared_ptr<CreateMessageFromResponse>> m_MessagesList;

public:
	PullMessagesResponse(char Version, short Code, const unsigned char* payload, int payloadSize) : Response(Version, Code, payload, payloadSize)
	{
		int loc = 0;
		while (loc < payloadSize)
		{
			std::shared_ptr<CreateMessageFromResponse> msg = std::make_shared<CreateMessageFromResponse>(payload, loc);
			m_MessagesList.push_back(msg);
			if(msg->GetType()==2)
				loc += 25 + 128;
			else if (msg->GetType() == 1)
				loc += 25;
			else if (msg->GetType() == 3)
				loc += 25 + 16;
		}
	}

	std::vector<std::shared_ptr<CreateMessageFromResponse>> GetMessagesList() { return m_MessagesList; }

};
