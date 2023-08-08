// Variables
#ifndef BCM_VARIABLES_H
#define BCM_VARIABLES_H

#include "BCMConstants.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define COMM_LIST_COUNT 7
#define ADDCOM_LIST_COUNT 3

#define SEND_MESSAGE_SIZE 6

#define MAX_PAGE 127
#define MIN_PAGE 0
#define PAGE_POINTS_START 10

#define PAGE_POINTS_SIZE 512

#define MAX_POINT_VALUE 2048
#define MIN_POINT_VALUE -2048
#define MAX_POINTS (PAGE_POINTS_SIZE * (MAX_PAGE + 1))

#define ACK_MESSAGE_SIZE 4

#define MAX_RECV_MESSAGE_SIZE 1034

#define MAX_BYTE_BUFFER MAX_RECV_MESSAGE_SIZE * (MAX_PAGE + 1)

#define WAVEFORM_LENGTH_TIME (320.0 / (double)MAX_POINTS) 

#define TRACE(x) do { if (debug_mode) dbg_printf x; } while (0)

typedef unsigned char byte;
typedef byte page[1034];

typedef struct _connection_credentials {
	char* hostname;
	int portno, sockfd;
	struct sockaddr_in serveraddr;
} connection_credentials;

enum commands {
	WRITE_REGISTER = 0,
	START_CYCLE,
	READ_REGISTER,
	STOP_CYCLE,
	READ_BUFFER,
	START_GENERATOR,
	COUNT_RESET 
};

typedef struct _commandlist {
	enum commands number;
	enum inputcommands_enum input_number;
	int* args;
	enum command_args_num_enum args_count;
	enum packetlengths_enum message_size;
	struct _commandlist* next;
	int* result;
	int result_size;
} commandlist;

extern enum inputcommands_enum inputcommands[COMM_LIST_COUNT];

extern enum packetlengths_enum packetlengths[COMM_LIST_COUNT];

extern enum command_args_num_enum command_args_num[COMM_LIST_COUNT];

extern int debug_mode; 

extern char* args_message;

extern char* string_commands[];

extern char* additional_commands[];

commandlist* init_commandlist();

commandlist* create_next_command_node(commandlist** curlist, int _commandnumber, ...);

int free_list(commandlist** curlist);

void my_strcpy(char** dest, const char* source);

void dbg_printf(const char *fmt, ...);

#endif
