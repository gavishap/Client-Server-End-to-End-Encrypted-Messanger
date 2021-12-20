#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
#include "Request.h" /*include Response inside */

class CommunicationHandler
{
private:
	std::string m_Address;
	int			m_Port;

	bool recvHeader(tcp::socket& socket, unsigned char* header);
	unsigned int extractPayloadSizeFromHeader(const unsigned char* header);
	bool recvPayload(tcp::socket& socket, unsigned int payload_size, unsigned char* payload);

public:
	CommunicationHandler(std::string address, int port) : m_Address(address), m_Port(port) {}
	CommunicationHandler();
	virtual ~CommunicationHandler();
	bool sendAndReceiveMessage(std::shared_ptr<Request>& request, std::shared_ptr<Response>& response);
};