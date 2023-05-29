// Variables
#ifndef BCM_VARIABLES_H
#define BCM_VARIABLES_H

#include <netinet/in.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define COMM_LIST_COUNT 7
#define ADDCOM_LIST_COUNT 3

typedef unsigned char byte;
typedef byte page[1034];

typedef struct _connection_credentials {
	char* hostname;
	int portno, sockfd;
	struct sockaddr_in serveraddr;
} connection_credentials;

typedef struct _commandlist {
	int number;
	int input_number;
	int* args;
	size_t args_count;
	size_t message_size;
	struct _commandlist* next;
	int output;
	char* file_name;
	int* result;
	int result_size;
} commandlist;

enum commands {
	WRITE_REGISTER = 0,
	START_CYCLE,
	READ_REGISTER,
	STOP_CYCLE,
	READ_BUFFER,
	START_GENERATOR,
	COUNT_RESET 
};

extern int debug_mode; 

extern int inputcommands[]; 

extern int packetlengths[]; 

extern char* args_message;

extern char* string_commands[];

extern char* additional_commands[];

extern int command_args_num[];

commandlist* init_commandlist();

commandlist* create_next_command_node(commandlist** curlist, int _commandnumber, ...);

int free_list(commandlist** curlist);

void my_strcpy(char** dest, const char* source);

void debug_printf(void* ptr, int n);

#endif
