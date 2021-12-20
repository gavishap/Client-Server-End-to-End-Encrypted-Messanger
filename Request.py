import struct
import uuid
from datetime import datetime
from Response import Response


class Request:
    def __init__(self, stream):
        self.client_id = stream[0:16]
        self.version = int.from_bytes(stream[16:17], "little")
        self.code = struct.unpack_from("H", stream, 17)[0]
        self.payload_size = int.from_bytes(stream[19:23], "little")
        self.payload = stream[23:]

    def __repr__(self):
        return "Version " + str(self.version) + " Code: " + str(self.code) + " PayloadSize: " + str(self.payload_size)


def register(payload, registered_clients):
    now = datetime.now()
    current_time_format = now.strftime("%d/%m/%Y %H:%M:%S")
    name = payload[0:255].decode('utf-8')
    public_key = payload[255:415]

    if name in registered_clients.clients.keys():
        print("Error: user", name, "already exists")
        return None
    else:
        uu = uuid.uuid4()
        print("Insert new client")
        registered_clients.add(uu, name, public_key, current_time_format)
        return Response(1, 2000, 23, uu.bytes)


def get_user_list(client_id, registered_clients):
    payload = bytearray()
    payload_size = 7  # header size
    for client in registered_clients.clients.values():
        if client_id != client.ID:
            payload += client.ID.bytes
            payload += client.Name.encode('utf-8')
            payload += b"0" * (255 - len(client.Name))
            payload_size += 16 + 255

    return Response(1, 2001, payload_size, payload)


def get_sent_message(src_client_id, request_payload, messages_pool):
    dst_client_id = request_payload[0:16]
    msg_type = request_payload[16]
    msg_content_size = int.from_bytes(request_payload[17:21], "little")

    msg_content = request_payload[21:21+msg_content_size]
    print("Message content size:", msg_content_size , "Content:", msg_content)
    new_msg_id = messages_pool.add(dst_client_id, src_client_id, msg_type, msg_content)
    response_payload = bytearray()
    response_payload += dst_client_id
    response_payload += int.to_bytes(new_msg_id, 4, "little")
    return Response(1, 2003, 27, response_payload)




def get_public_key(request_payload , registered_clients):
    dst_client_id_in_bytes = request_payload[0:16]
    dst_client_id = uuid.UUID(bytes=dst_client_id_in_bytes)

    dst_client = None
    if dst_client_id in registered_clients.clients.keys():
        dst_client = registered_clients.clients[dst_client_id]
    if dst_client is None:
        print("Error, cannot found this client id")  # Return Error Response
        return None

    public_key = dst_client.PublicKey
    print(public_key)
    response_payload = bytearray()
    response_payload += dst_client_id_in_bytes
    response_payload += public_key
    return Response(1, 2002, 183, response_payload)


def get_awaiting_messages(client_id, messages_pool):
    target_msgs = messages_pool.pool[client_id]
    payload_size = 7  # header size
    response_payload = bytearray()
    for msg in target_msgs:
        response_payload += msg.src_client                              # Client ID
        response_payload += int.to_bytes(msg.index, 4, "little")        # Message ID
        response_payload += int.to_bytes(msg.msg_type, 1, "little")     # Message Type
        response_payload += int.to_bytes(len(msg.content), 4, "little") # Message Size
        response_payload += msg.content                                 # Message Content
        print("Message content:", msg.content)
        print(len(msg.content))
        payload_size += 25 + len(msg.content)

    return Response(1, 2004, payload_size, response_payload)


