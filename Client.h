#pragma once
#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include <string>

#include <sstream>
#include "AESWrapper.h"

using ClientID = std::array<char, 16>;
using ClientName = std::array<char, 255>;
using Client = std::pair<ClientID, ClientName>;
using ClientPublicKey = std::array<char, 160>;
using Client_IDandPUBKEY = std::pair<ClientID, ClientPublicKey>;
using Client_IDandSYMKEY = std::pair<ClientID, AESWrapper>;

//std::string hex_to_ascii(std::string ID)
//{
//    int len = ID.length();
//    std::string uuid;
//    for (int i = 0; i < len; i += 2)
//    {
//        std::string byte = ID.substr(i, 2);
//        char chr = (char)(int)strtol(byte.c_str(), nullptr, 16);
//        uuid.push_back(chr);
//    }
//    return uuid;
//}

static std::string ClientIdAsString(ClientID clientId)
{
	std::stringstream ss;
	for (int i = 0; i < 16; ++i)
	{
		ss << std::hex << (((int)clientId[i] & 0xF0) >> 4);
		ss << std::hex << (((int)clientId[i] & 0x0F) >> 0);
	}
	return  ss.str();
}

static std::string ClientPubKeyAsString(ClientPublicKey clientPubKey)
{
	std::stringstream ss;
	for (int i = 0; i < 160; ++i)
	{
		ss << std::hex << (((int)clientPubKey[i] & 0xF0) >> 4);
		ss << std::hex << (((int)clientPubKey[i] & 0x0F) >> 0);
	}
	return  ss.str();
}

//std::string convertToString(char* a, int size)
//{
//	int i;
//	std::string s = "";
//	for (i = 0; i < size; i++) {
//		s = s + a[i];
//	}
//	return s;
//}
//std::string ClientNameAsString(ClientName name)
//{
//	return {};
//}
//
//std::string ClientPublicKeyAsString(ClientPublicKey publicKey)
//{
//	return {};
//}




