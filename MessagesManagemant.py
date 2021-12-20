
class Message:
    def __init__(self, index, dst_client, src_client, msg_type, content):
        self.index = index          # Index (32bit)
        self.dst_client = dst_client
        self.src_client = src_client
        self.msg_type = msg_type
        self.content = content

    def __str__(self):
        return "index = " + str(self.index) + " dest client = " + str(self.dst_client) + " src client = " + \
            str(self.src_client) + " msg_type = " + str(self.msg_type) + " Content: " + str(self.content)


class MessagesPool:
    def __init__(self):
        self.pool = dict()
        self.size = 0

    def add_message(self, message):
        if message.dst_client not in self.pool.keys():
            self.pool[message.dst_client] = []
        self.pool[message.dst_client].append(message)
        self.size += 1

    def add(self, dst_client, src_client, msg_type, content):
        next_index = self.size
        self.add_message(Message(next_index, dst_client, src_client, msg_type, content))
        return next_index

    def __str__(self):
        res = ""
        for dst_client_id in self.pool:
            for msg in self.pool[dst_client_id]:
                res += str(msg) + "\n"
        return res


ServerMessagesPool = MessagesPool()
