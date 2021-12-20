#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <fstream>
#include "CommunicationHandler.h"
#include <iomanip>
#include <algorithm>
#include "AESWrapper.h"
#include <map>
#include <modes.h>
#include <aes.h>
#include <filters.h>
#include <stdexcept>
#include <immintrin.h>	
#include "RSAWrapper.h"
#include "Base64Wrapper.h"


void displayMenu()
{
	std::cout << "MessageU client at your service." << std::endl << std::endl;
	std::cout << "10)Register" << std::endl;
	std::cout << "20)Request for clients list" << std::endl;
	std::cout << "30)Request for public key" << std::endl;
	std::cout << "40)Request for waiting messages" << std::endl;
	std::cout << "50)Send a text Message" << std::endl;
	std::cout << "51)Send a request for symmetric key" << std::endl;
	std::cout << "52)Send your symmetric key" << std::endl;
	std::cout << "0)Exit client" << std::endl;
	//TODO: fill according to the picture.
}

int GetRequestIDFromClient()
{
	int requestID = -1;
	std::cin >> requestID;
	if (std::cin.good())
	{
		if (requestID != 0 && requestID != 10 && requestID != 20 && requestID != 30 && requestID != 40 && requestID != 50 && requestID != 51 && requestID != 52)
		{
			return -1;
		}
		return requestID;
	}
	else
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return -1;
	}
}
void print_map(std::map<ClientID, ClientPublicKey>& clientsMap)
{
	for (const auto& client : clientsMap)
	{
		std::cout << "Client ID = " << ClientIdAsString(client.first) << ", value = " << ClientPubKeyAsString(client.second) << std::endl;
	}
}
std::pair<std::string, int> readServerInfo()
{
	std::string info = "", path = "C:/Users/gavis/source/repos/mmn15_defensive/x64/Debug/server.info";
	std::ifstream file(path);
	if (file.is_open())
	{
		file >> info;
	}
	else
		std::cout << "cant find " << path << std::endl;

	auto foundDelimiter = info.find(':');
	std::string ip = info.substr(0, foundDelimiter);
	std::string port = info.substr(foundDelimiter + 1);

	return {ip, std::stoi(port)};
}

void writeMeInfo(std::string name, std::string uuid, std::string privateKey)
{
	std::string path = "C:/Users/gavis/source/repos/mmn15_defensive/x64/Debug/me.info";
	std::ofstream file(path);
	if (file.is_open())
	{
		file << name << std::endl;
		file << uuid << std::endl;
		file << privateKey << std::endl;
	}
	else
		std::cout << "cant find " << path << std::endl;
}
//function to turn a string into type ClientID
ClientID parseClientID(std::string ID)
{
	int len = ID.length();
	ClientID uuid;
	for (int i = 0, j = 0; i < len; j += 1, i += 2)
	{
		std::string byte = ID.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), nullptr, 16);
		uuid[j] = chr;
	}
	return uuid;
}

bool readMeInfo(std::string& name, ClientID& uuid, std::string& privateKey)
{
	std::string path = "C:/Users/gavis/source/repos/mmn15_defensive/x64/Debug/me.info";
	std::string firstLine, secondLine, thirdLine, line;
	std::ifstream file(path);
	if (file.is_open())
	{
		file >> firstLine;
		file >> secondLine;
		name = firstLine;
		uuid = parseClientID(secondLine);
		while(std::getline(file, line))
			privateKey += line;
		return true;
	}
	else
	{
		return false;
	}
}


int main()
{
	auto serverInfo = readServerInfo();
	std::cout << serverInfo.first << ":" << serverInfo.second << std::endl;
	//readMeInfo();

	CommunicationHandler communication(serverInfo.first, serverInfo.second);

	bool isExit = false;
	std::vector<std::string>nameList;
	std::vector<std::string> identities;
	std::map<ClientID, ClientPublicKey> client_public_info = {};
	std::map<ClientID, const unsigned char*> client_sym_info = {};
	do
	{
		std::shared_ptr<Request> request;
		std::shared_ptr<Response> response;
		displayMenu();
		int incomingRequestID = GetRequestIDFromClient();
		std::string clientName, publicKey, privateKey;
		ClientID clientID = {};
		if (incomingRequestID == -1)
		{
			// Get invalid request id
			std::cout << "server responded with an error" << std::endl;
			continue;
		}
		
		if (incomingRequestID == 0)
		{
			// Exit client

			//TODO: free all allocated memory
			isExit = true;
			break;

		}
		// start handle request

		if (incomingRequestID == 10)
		{
			// check me.info already exists
			bool isUserRegistered = readMeInfo(clientName, clientID, privateKey);
			if (isUserRegistered)
			{
				std::cout << "Client already exists, can't register again." << std::endl;
				continue;
			}

			std::cout << "Insert client name:" << std::endl;
			std::cin >> clientName;

			//Create an RSA decryptor. this is done here to generate a new private/public key pair
			RSAPrivateWrapper rsapriv;
			publicKey = rsapriv.getPublicKey();
			privateKey = Base64Wrapper::encode(rsapriv.getPrivateKey());
			std::cout << "Your Public Key:\n" << std::endl;
			char buffer[3024];
			for (int i = 0; i < publicKey.size(); ++i)
			{
				sprintf(&buffer[3 * i], "%02X%c", static_cast<uint8_t>(publicKey[i]), ' ');
			}
			std::cout << buffer << std::endl;
			std::cout << "============================================================" << std::endl;
			request = std::make_shared<RegisterRequest>(clientName, publicKey);
			uint8_t* tmp = reinterpret_cast<uint8_t*>(request->getPayload());
			
			for (int i = 0; i < request->getPayloadSize(); ++i)
				sprintf(&buffer[3 * i], "%02X%c", tmp[i], ' ');
			std::cout << buffer << std::endl;

			communication.sendAndReceiveMessage(request, response);

			if (response->GetCode() == 2000)
			{

				std::cout << "Client register succeeded" << std::endl;
				writeMeInfo(clientName, std::static_pointer_cast<RegisterSuccessResponse>(response)->ClientIdAsString(), privateKey);
			}

			continue;
		}

		/* Verify user registered */
		bool isUserRegistered = readMeInfo(clientName, clientID, privateKey);
		if (!isUserRegistered)
		{
			std::cout << "Unauthorized user request, Please register first" << std::endl;
			continue;
		}

		/* Handle incoming requests when User alredy registered */

		if (incomingRequestID == 20)
		{
			request = std::make_shared<UsersListRequest>(clientID);
			std::cout << "the ID of the user list request is:"<< ClientIdAsString(clientID) << std::endl;
			communication.sendAndReceiveMessage(request, response);
			if (response->GetCode() == 2001)
			{
				std::cout << "Heres the client List:" << std::endl;
				std::vector<Client> UsersList = std::static_pointer_cast<UsersListResponse>(response)->GetUsersList();
				for (int i = 0; i < UsersList.size(); i++)
				{
					identities.push_back(ClientIdAsString(UsersList.at(i).first));
					nameList.push_back(UsersList.at(i).second.data());
					std::cout << nameList[i] << "  " << identities[i] << std::endl;
				}
			}
		}
		else if (incomingRequestID == 50)
		{
			if (!isUserRegistered)
			{
				std::cout << "Unauthorized user request, Please register first" << std::endl;
				continue;
			}
			if (nameList.empty() && identities.empty())
				std::cout << "Please call for a user list before trying to send a message." << std::endl;
			std::string text;
			/* send text message to server */
			std::cout << "Insert target name :" << std::endl;
			std::cin >> clientName;
			if (std::find(std::begin(nameList), std::end(nameList), clientName) == std::end(nameList))
			{
				//we did not find the name in the registered user list
				std::cout << "User not found, Please type in a user that is registered." << std::endl;
				continue;
			}
			ClientID dstID;
			for (int i = 0; i < nameList.size(); i++)
			{
				if (clientName == nameList[i])
				{
					dstID = parseClientID(identities[i]);
					break;
				}
			}
			std::cout << "Insert text: " << std::endl;
			std::cin >> text;

			if (client_sym_info.find(dstID) == client_sym_info.end() && client_sym_info.find(clientID) == client_sym_info.end()) {
				//Symmetric key not found
				std::cout << "Need a symmetric key first" << std::endl;
				continue;
			}
			else {
				//Symmetric key found
				const unsigned char* sym_key = client_sym_info[dstID];
				AESWrapper aes(sym_key, AESWrapper::DEFAULT_KEYLENGTH);
				std::string ciphertext = aes.encrypt(text.c_str(), text.length());
				std::shared_ptr<Message> msg = std::make_shared<SendTextMessage>(dstID, ciphertext);

				/*unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
				AESWrapper aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH);
				std::string ciphertext = aes.encrypt(text.c_str(), text.length());*/


				//msg->encrypt_or_decrypt();
				request = std::make_shared<SendMessageRequest>(clientID, msg, ciphertext.size());
				communication.sendAndReceiveMessage(request, response);
				if (response->GetCode() == 2003)
				{
					std::cout << "Send text succeeded" << std::endl;
				}
			}
		}
		else if (incomingRequestID == 51)
		{
			if (!isUserRegistered)
			{
				std::cout << "Unauthorized user request, Please register first" << std::endl;
				continue;
			}
			if (nameList.empty() && identities.empty())
			{
				std::cout << "Please call for a user list before trying to send a message." << std::endl;
				continue;
			}
			std::string text;
			/* send text message to server */
			std::cout << "Insert target name :" << std::endl;
			std::cin >> clientName;
			if (std::find(std::begin(nameList), std::end(nameList), clientName) == std::end(nameList))
			{
				//we did not find the name in the registered user list
				std::cout << "User not found, Please type in a user that is registered." << std::endl;
				continue;
			}
			ClientID dstID;
			for (int i = 0; i < nameList.size(); i++)
			{
				if (clientName == nameList[i])
				{
					dstID = parseClientID(identities[i]);
					break;
				}
			}
			std::shared_ptr<Message> msg = std::make_shared<RequestSymmetricKeyMessage>(dstID);
			msg->encrypt_or_decrypt();
			request = std::make_shared<SendMessageRequest>(clientID, msg, 0);

			communication.sendAndReceiveMessage(request, response);
			if (response->GetCode() == 2003)
			{
				std::cout << "Send SymmetricKey succeeded" << std::endl;
			}
		}

		else if (incomingRequestID == 52)
		{
			if (!isUserRegistered)
			{
				std::cout << "Unauthorized user request, Please register first" << std::endl;
				continue;
			}
			if (nameList.empty() && identities.empty())
			{
				std::cout << "Please call for a user list before trying to send a message." << std::endl;
				continue;
			}
			
			/* TO DO: make symmetric key to send*/
			unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];			
			AESWrapper aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH);
			
	
			 

			std::cout << "Insert target name :" << std::endl;
			std::cin >> clientName;
			if (std::find(std::begin(nameList), std::end(nameList), clientName) == std::end(nameList))
			{
				//we did not find the name in the registered user list
				std::cout << "User not found, Please type in a user that is registered." << std::endl;
				continue;
			}
			ClientID dstID;
			for (int i = 0; i < nameList.size(); i++)
			{
				if (clientName == nameList[i])
				{
					dstID = parseClientID(identities[i]);
					break;
				}
			}

			/*encrypt symmetric key with destination clients public key*/
			std::string pubkey(std::begin(client_public_info[dstID]), std::end(client_public_info[dstID]));
			RSAPublicWrapper rsapub(pubkey);
			std::string sym_key_encrypted = rsapub.encrypt((const char*)aes.getKey(), AESWrapper::DEFAULT_KEYLENGTH); //rsapub is dst public key
			/*Client_IDandSYMKEY temp_client = std::make_pair( dstID, symmetric_key );
			client_sym_info.insert(clientName, temp_client);*/


			std::shared_ptr<Message> msg = std::make_shared<SendSymmetricKeyMessage>(dstID, sym_key_encrypted);
			/*msg->encryption();*/
			request = std::make_shared<SendMessageRequest>(clientID, msg, sym_key_encrypted.size());

			communication.sendAndReceiveMessage(request, response);
			if (response->GetCode() == 2003)
			{
				std::cout << "Send symmetric key succeeded" << std::endl;
			}
		}
		//Request for public key
		else if (incomingRequestID == 30)
		{
			if (!isUserRegistered)
			{
				std::cout << "Unauthorized user request, Please register first" << std::endl;
				continue;
			}

			if (nameList.empty() && identities.empty())
			{
				std::cout << "Please call for a user list before trying to send a message." << std::endl;
				continue;
			}

			std::cout << "Insert which user's public key you want :" << std::endl;
			std::cin >> clientName;
			if (std::find(std::begin(nameList), std::end(nameList), clientName) == std::end(nameList))
			{
				//we did not find the name in the registered user list
				std::cout << "User not found, Please type in a user that is registered." << std::endl;
				continue;
			}

			ClientID dstID;
			for (int i = 0; i < nameList.size(); i++)
			{
				if (clientName == nameList[i])
				{
					dstID = parseClientID(identities[i]);
					break;
				}
			}
			request = std::make_shared<ClientPublicKeyRequest>(clientID, dstID);
			communication.sendAndReceiveMessage(request, response);
			if (response->GetCode() == 2002)
			{
				std::cout << "Successfully recieved public key" << std::endl;
				client_public_info[dstID] = std::static_pointer_cast<ClientPublicKeyResponse>(response)->GetPublicKey();
				print_map(client_public_info);
			}
		}
		else if (incomingRequestID == 40)
		{
			if (!isUserRegistered)
			{
				std::cout << "Unauthorized user request, Please register first" << std::endl;
				continue;
			}
	
			if (nameList.empty() && identities.empty())
			{
				std::cout << "Please call for a user list before trying to send a message." << std::endl;
				continue;
			}
			request = std::make_shared<PullMessagesRequest>(clientID);
			communication.sendAndReceiveMessage(request, response);
			if (response->GetCode() == 2004)
			{
				std::vector<std::shared_ptr<CreateMessageFromResponse>> awaited_messages = std::static_pointer_cast<PullMessagesResponse>(response)->GetMessagesList();
				std::cout << std::endl<<awaited_messages.size()<<std::endl;
				for (const auto& element : awaited_messages)
				{
					ClientID srcID = element->GetClientID();
					std::string srcName;
					char msgType= element->GetType();
					std::string content = element->GetContent();
					for (int i = 0; i < nameList.size(); i++)
					{
						if (ClientIdAsString(srcID) == identities[i])
						{
							srcName = nameList[i];
							break;
						}
					}
					if (msgType == 1)
					{
						content = "Request for Symmetric Key";
					}
					else if (msgType == 2)
					{
						RSAPrivateWrapper rsapriv_other(Base64Wrapper::decode(privateKey));
						content = rsapriv_other.decrypt(content);
						client_sym_info[srcID] = (const unsigned char*)content.c_str();
						client_sym_info[clientID] = (const unsigned char*)content.c_str();
						content = "Symmetric key recieved";
					}
					else if (msgType == 3)
					{
						if (client_sym_info.find(clientID) == client_sym_info.end()) {
							//Symmetric key not found
							content = "Cant decrypt message without " +srcName +"'s symmetric key";
						}
						else {
							//Symmetric key found
							const unsigned char* sym_key = client_sym_info[clientID];
							AESWrapper aes( sym_key, AESWrapper::DEFAULT_KEYLENGTH);
							std::string decrypttext  = aes.decrypt(content.c_str(), content.length());
							content = decrypttext;
						}
					}
					
					//std::string decrypttext = aes.decrypt(ciphertext.c_str(), ciphertext.length());
					std::cout << "From:" << srcName << std::endl << "Content:" << std::endl << content << std::endl << "----<EOM>----" << std::endl;

				}
				std::cout << "Successfully recieved awaiting messages" << std::endl;
			}
		}


	} while (!isExit);


	return 0;
};

