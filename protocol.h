#pragma once

#include <stddef.h>
#include <stdint.h>
#include "common.h"

#pragma pack(push, 1)
//-----------------------------------------------------
typedef enum : uint8_t {
	MST_NA = 0,
	MST_REQ_SYM_KEY = 1,
	MST_SEND_SYM_KEY = 2,
	MST_SEND_TEXT = 3,
	MST_SEND_FILE = 4,
}msg_type_e;
//-----------------------------------------------------
typedef enum : uint16_t {
	CCMD_REGISTER = 1000, //10
	CCMD_REQ_FOR_CLIENTS_LIST = 1001, //20
	CCMD_REQ_FOR_PUBLIC_KEY = 1002, //30
	CCMD_REQ_FOR_WAITING_MSGS = 1004, //40
	CCMD_SEND_TEXT_MSG = 1003, //50
	CCMD_REQ_FOR_OTHER_SYMMETRIC_KEY = 1003, //51
	CCMD_SEND_MY_SYMMETRIC_KEY = 1003, //52
	CCMD_SEND_FILE = 1003, //53
	MAX_CCMD = 0xffff,// force it to be 16 bit
}client_cmd_code_e;
//-----------------------------------------------------
typedef enum : uint16_t {
	SCMD_REGISTER = 2000, // resp for 10
	SCMD_REQ_FOR_CLIENTS_LIST = 2001, // resp for 20
	SCMD_REQ_FOR_PUBLIC_KEY = 2002, // resp for 30
	SCMD_REQ_FOR_WAITING_MSGS = 2004, // resp for 40
	SCMD_SEND_TEXT_MSG = 2003, // resp for 50
	SCMD_ERR_FROM_SERVER = 9000, // resp for any problem that the server found
	//-------------------------------------------------
	MAX_SCMD = 0xffff// force it to be 16 bit
}server_cmd_code_e;
//-----------------------------------------------------
typedef struct {
	char		name[MAX_NAME_LEN];
	char		public_key[MAX_PUBLIC_KEY_LEN];
}client_reg_payload_s;
//-----------------------------------------------------
typedef struct
{
	uint8_t				client_ID[MAX_UU_ID_LEN];
	uint32_t			msg_ID;
	msg_type_e			msg_type;
	uint32_t			size;
	uint8_t				content[1];
}msg_from_other_client_s;

//-----------------------------------
typedef struct {
	uint8_t				client_ID[MAX_UU_ID_LEN];
	uint8_t				client_version;
	client_cmd_code_e   ccmd; // delete because enum size is 4 instead of 2
	uint32_t			size;
	uint8_t				payload[1];
}tx_msg_s;
//-----------------------------------
typedef struct {
	uint8_t				client_version;
	server_cmd_code_e   scmd; // delete because enum size is 4 instead of 2
	uint32_t			size;
	uint8_t				payload[1];
}rx_msg_s;
//-----------------------------------
typedef struct {
	uint8_t				client_version;
	server_cmd_code_e   scmd;
	uint32_t			size;
}server_header_s;
#pragma pack(pop)
