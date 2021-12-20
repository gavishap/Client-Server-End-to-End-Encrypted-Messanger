#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <memory>
#include <assert.h>
#include <string>
#include "Client.h"
class Message
{
protected:
	ClientID m_ClientID;
	bool m_IsDstclient;
	char m_MessageType;
	std::string m_Text;
	Message() {};
	Message(bool isDstclient, ClientID clientID, char messageType, std::string text = "") : m_IsDstclient(isDstclient), m_ClientID(clientID), m_MessageType(messageType), m_Text(text)
	{
		
	}
protected:
	unsigned int fromBytesToVal(const unsigned char* payload, int offset, int sizeOfBytes);
public:
	
	ClientID GetClientID() { return m_ClientID;  }
	char GetType() { return m_MessageType; }
	std::string GetContent() { return m_Text; }
	virtual void encrypt_or_decrypt() = 0;
};

class RequestSymmetricKeyMessage : public Message
{
public:
	RequestSymmetricKeyMessage(ClientID clientID) : Message(true, clientID, 1) {}

	void encrypt_or_decrypt() {}
};

class SendSymmetricKeyMessage : public Message
{

public:
	SendSymmetricKeyMessage(ClientID clientID, std::string text) : Message(true, clientID, 2, text) {}

	//std::string GetSymmetricKey()
	//{
	//	std::string key(m_MessageContent, m_ContentSize);
	//	return key;

	//}

	void encrypt_or_decrypt() { /* implement public key encryption */}
};

class SendTextMessage : public Message
{

public:
	SendTextMessage(ClientID clientID, std::string text) : Message(true, clientID, 3, text) {}

	//std::string GetText()
	//{
	//	std::string key(m_MessageContent, m_ContentSize);
	//	return key;
	//}

	void encrypt_or_decrypt() {/* implement symmetric key encryption */}
};

class CreateMessageFromResponse : public Message
{
	int m_MessageID = -1;
public:
	CreateMessageFromResponse(const unsigned char* payload, int offset) : Message()
	{
		m_IsDstclient = false;
		//Add that the content will change according to what message it is
		std::copy(payload +offset, payload + 16 + offset, m_ClientID.begin());
		m_MessageID = fromBytesToVal(payload, 16 + offset, 4);
		m_MessageType = payload[offset + 20];
		int contentSize = fromBytesToVal(payload, 21 + offset, 4);
		m_Text.resize(contentSize);
		std::copy(payload + offset + 25, payload + offset + 25 + contentSize, m_Text.begin());
	}

	void encrypt_or_decrypt() { /* need to imlement */}
};
