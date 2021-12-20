import sys
import socket
from threading import Thread
from Client import RegisteredClientsMap
import uuid
from Request import Request
from Response import Response
import Request as ReqHandler
from MessagesManagemant import ServerMessagesPool

def read_port():
    portFile = open("port.info", "r")
    port = int(portFile.read())
    return port

def handle_client(connection, client_address):

    print("Establish new connection with client", client_address)
    request_stream = connection.recv(1024)
    print ("Incoming request content:", request_stream)
    request = Request(request_stream)
    print("request.code = ", request.code)
    if request.code == 1000:
        # Get requset from type "Register"
        response = ReqHandler.register(request.payload, RegisteredClientsMap)
        print(RegisteredClientsMap)
        if response is not None:
            connection.send(response.stream)
    elif request.code == 1001:
        # Get requset from type "UserList"
        response = ReqHandler.get_user_list(request.client_id, RegisteredClientsMap)
        if response is not None:
            connection.send(response.stream)
    elif request.code == 1002:
            # Get requset from type "SendMessage"
            response = ReqHandler.get_public_key(request.payload, RegisteredClientsMap)
            if response is not None:
                connection.send(response.stream)
    elif request.code == 1003:
        # Get request from type "SendMessage"
        response = ReqHandler.get_sent_message(request.client_id, request.payload, ServerMessagesPool)
        print(ServerMessagesPool)
        if response is not None:
            connection.send(response.stream)
            print("Sent success")
    elif request.code == 1004:
        response = ReqHandler.get_awaiting_messages(request.client_id , ServerMessagesPool)
        if response is not None:
            connection.send(response.stream)
    connection.close()

def start_server(port):
    sock = socket.socket()              # Create scoket object
    host = socket.gethostname()         # Get local name

    sock.bind((host, port))             # Bind socket to port
    sock.listen(5)                      # Wait fo client connection

    while True:
        print("wait for client...")
        connection, client_address = sock.accept()  # Establish connection with client
        print(RegisteredClientsMap)
        handler =Thread(target=handle_client, args=(connection, client_address))
        handler.start()


def main():
    print("Server...")
    port = read_port()
    print("Server listen on port", port)
    start_server(port)

if __name__ == "__main__":
    main()




