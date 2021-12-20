#include "CommunicationHandler.h"
#include <iostream>
#include <iomanip>

using namespace std;

void hexify(const unsigned char* buffer, unsigned int length)
{
    std::ios::fmtflags f(std::cout.flags());
    std::cout << std::hex;
    for (size_t i = 0; i < length; i++)
        std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
    std::cout << std::endl;
    std::cout.flags(f);
}

bool CommunicationHandler::recvHeader(tcp::socket& socket, unsigned char* header)
{
    boost::asio::streambuf headerBuffer;
    boost::system::error_code error;
    boost::asio::read(socket, headerBuffer, boost::asio::transfer_exactly(7), error);
    if (error && error != boost::asio::error::eof) 
    {
        cout << "receive header failed: " << error.message() << endl;
        return false;
    }
    auto buffer = const_cast<unsigned char*>(boost::asio::buffer_cast<const unsigned char*>(headerBuffer.data()));
    std::copy(buffer, buffer + 7, header);
    return true;
}

unsigned int CommunicationHandler::extractPayloadSizeFromHeader(const unsigned char* header)
{
    auto payloadSize = Response::fromBytes(header, 3, 4);
    return payloadSize;
}

bool CommunicationHandler::recvPayload(tcp::socket& socket, unsigned int payload_size, unsigned char* payload)
{
    boost::asio::streambuf payloadBuffer;
    boost::system::error_code error;
    boost::asio::read(socket, payloadBuffer, boost::asio::transfer_exactly(payload_size), error);
    if (error && error != boost::asio::error::eof) 
    {
        cout << "receive header failed: " << error.message() << endl;
        return false;
    }
    auto buffer = const_cast<unsigned char*>(boost::asio::buffer_cast<const unsigned char*>(payloadBuffer.data()));
    std::copy(buffer, buffer + payload_size, payload);
    return true;
}

CommunicationHandler::~CommunicationHandler() = default;


bool CommunicationHandler::sendAndReceiveMessage(std::shared_ptr<Request>& request, std::shared_ptr<Response>& response)
{
    boost::asio::io_context io_context;

    //socket creation
    tcp::socket socket(io_context);

    //connection
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(m_Address), m_Port));

    // sending request/message to server
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(request->getPayload(), request->getPayloadSize()), error);
    if (error) 
    {
        cout << "send failed: " << error.message() << endl;
        return 0;
    }
    std::cout << "write success" << std::endl;
    // received response from server
    unsigned char header[7];
    if (!recvHeader(socket, header)) return false;
        

    auto payload_size = extractPayloadSizeFromHeader(header);
    unsigned char* payload = new unsigned char[payload_size];
    if (!recvPayload(socket, payload_size - 7, payload)) return false;
    std::cout << "read success" << std::endl;
    response = Response::CreateResponse(header, payload);
    delete[]payload;
    return true;
}
