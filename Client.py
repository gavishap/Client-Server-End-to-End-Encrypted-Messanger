class Client:
    def __init__(self, uuid, name, public_key, last_seen):
        self.ID = uuid          # UUid (128bit)
        self.Name = name
        self.PublicKey = public_key
        self.LastSeen = last_seen

    def __str__(self):
        return "id = " + str(self.ID) + " name = " + str(self.Name) + " public key = " + str(self.PublicKey)

    def __repr__(self):
        return "id = " + str(self.ID) + " name = " + str(self.Name) + " public key = " + str(self.PublicKey)


class ClientsMap:
    def __init__(self):
        self.clients = dict()

    def __repr__(self):
        res = ""
        for uuid in self.clients:
            res += "Client name: " + self.clients[uuid].Name + "\nClient Details: " + str(self.clients[uuid]) + "\n"
        return res

    def add_client(self, client):
        self.clients[client.ID] = client

    def add(self, uuid, name, public_key, last_seen):
        self.add_client(Client(uuid, name, public_key, last_seen))


RegisteredClientsMap = ClientsMap()
