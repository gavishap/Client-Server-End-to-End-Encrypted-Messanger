#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <array>
#include <iostream>
#include <iomanip>

#include "Response.h" /* include Message inside */
class Request
{
	ClientID m_client_ID;
	char m_Version;
	short m_Code;


protected:
	static constexpr int HeaderPayloadSize = 23;
	int m_Payload_size;
	char* m_payload = nullptr;
	Request(short code) : m_client_ID({}), m_Version(0), m_Code(code), m_Payload_size(HeaderPayloadSize) {   }
	Request(short code, std::array<char, 16> client_ID) : m_client_ID(client_ID), m_Version(0), m_Code(code), m_Payload_size(HeaderPayloadSize) {  }
	//Request(std::array<char, 16> client_ID, char version, short code, int payload_size) : m_client_ID(client_ID), m_Version(version), m_Code(code), m_Payload_size(payload_size) {  }
	virtual ~Request() {delete m_payload; }



	void toBytes(int value,int offset, int sizeOfBytes)
	{
		for (int i = 0; i < sizeOfBytes; ++i)
		{
			m_payload[offset + i] = value >> i*8;
		}
	}

	virtual void serialize()
	{
		if (m_payload != nullptr)
		{
			std::copy(m_client_ID.begin(), m_client_ID.end(), m_payload);
			m_payload[16] = m_Version;
			toBytes(m_Code, 17, 2);
			toBytes(m_Payload_size, 19, 4);
		}
	}; /*  = 0; encode request and insert to payload, update payload_size */
public:

	char* getPayload() 
	{ 
		return m_payload;
	}

	int getPayloadSize()
	{
		return m_Payload_size;
	}
};


class RegisterRequest : public Request
{
	static constexpr short Code = 1000;
	static constexpr int NameMaxSize = 255;
	static constexpr int PublicKeyMaxSize = 160;

	char m_Name[NameMaxSize];
	ClientPublicKey m_Public_Key;

public:
	RegisterRequest(const std::string& name, const std::string& public_key) : Request(Code){
		std::strncpy(m_Name, name.c_str(), NameMaxSize);
		std::copy(public_key.begin() , public_key.begin() + PublicKeyMaxSize, m_Public_Key.begin());
		serialize();
	}

	void serialize()
	{
		m_Payload_size += NameMaxSize + PublicKeyMaxSize;	// add son fields size to the payload_size
		m_payload = new char[m_Payload_size];				// allocate payload memory
		Request::serialize();								// serialize father basize fields
		char* payloadPointerAfterHeader = m_payload + HeaderPayloadSize;	// set pointer to the section after the header
		// copy son fields to payload
		std::copy(m_Name, m_Name + NameMaxSize, payloadPointerAfterHeader);
		std::copy(m_Public_Key.begin(), m_Public_Key.begin() + PublicKeyMaxSize, payloadPointerAfterHeader + NameMaxSize);
	}
};

class UsersListRequest : public Request
{
	static constexpr int Code = 1001;
	
public:
	UsersListRequest(std::array<char, 16> client_ID) : Request(Code, client_ID) { serialize();  }

	void serialize()
	{
		m_payload = new char[m_Payload_size];				// allocate payload memory
		Request::serialize();								// serialize father basic fields
	}
};

class ClientPublicKeyRequest : public Request
{
	static constexpr int Code = 1002;
	ClientID m_Target_Client_ID;

public:
	ClientPublicKeyRequest(ClientID src_client_ID, ClientID target_client_ID) : Request(Code, src_client_ID), m_Target_Client_ID(target_client_ID) { serialize(); }

	void serialize()
	{
		m_Payload_size += 16;	// add son fields size to the payload_size
		m_payload = new char[m_Payload_size];				// allocate payload memory
		Request::serialize();								// serialize father basize fields
		char* payloadPointerAfterHeader = m_payload + HeaderPayloadSize;	// set pointer to the section after the header
		// copy son fields to payload
		std::copy(m_Target_Client_ID.begin(), m_Target_Client_ID.end(), payloadPointerAfterHeader);
	}
};

class SendMessageRequest : public Request
{
	static constexpr int Code = 1003;
	ClientID m_dstClientID;
	char m_MessageType;
	std::string m_Content;
	int m_ContentSize;

public:
	//Message of Type 1
	//SendMessageRequest(std::array<char, 16> src_client_ID, char messageType) :
	//	m_dstClientID(msg->GetClientID()), m_MessageType(1), m_ContentSize(0), Request(Code, src_client_ID)
	//{
	//	serialize();
	//}
	////Message of Type 2
	//SendMessageRequest(std::array<char, 16> src_client_ID, char messageType, int contentSize, char* messageContent) :
	//	m_dstClientID(msg->GetClientID()), m_MessageType(messageType), m_ContentSize(contentSize), m_MessageContent(messageContent), Request(Code, src_client_ID) {
	//	serialize();}
	//Message of Type 3
	SendMessageRequest(ClientID src_client_ID, std::shared_ptr<Message> msg, int contentsize) :
		m_dstClientID(msg->GetClientID()), m_MessageType(msg->GetType()), m_Content(msg->GetContent()), m_ContentSize(contentsize), Request(Code, src_client_ID)
	{
		serialize();
	}


	void serialize()
	{
		m_Payload_size += 21 + m_ContentSize;	// add son fields size to the payload_size
		m_payload = new char[m_Payload_size];				// allocate payload memory
		Request::serialize();								// serialize father basize fields
		char* payloadPointerAfterHeader = m_payload + HeaderPayloadSize;	// set pointer to the section after the header
		// copy son fields to payload
		std::copy(m_dstClientID.begin(), m_dstClientID.end(), payloadPointerAfterHeader);
		payloadPointerAfterHeader[16] = m_MessageType;
		toBytes(m_ContentSize, 17 + HeaderPayloadSize, 4);
		std::copy(m_Content.begin(), m_Content.end(), payloadPointerAfterHeader + 21);
	}

};

class PullMessagesRequest : public Request
{
	static constexpr int Code = 1004;

public:
	PullMessagesRequest(ClientID client_ID) : Request(Code, client_ID) { serialize(); }

	void serialize()
	{
		m_payload = new char[m_Payload_size];				// allocate payload memory
		Request::serialize();								// serialize father basic fields
	}
};