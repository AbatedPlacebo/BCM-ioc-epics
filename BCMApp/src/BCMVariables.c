#include "BCMVariables.h"

// 0 - disabled
// 1 - enabled
int debug_mode = 0;

enum inputcommands_enum inputcommands[COMM_LIST_COUNT] = {
	WRITE_REGISTER_COMM,
	START_CYCLE_COMM,
	READ_REGISTER_COMM,
	STOP_CYCLE_COMM,
	READ_BUFFER_COMM,
	START_GENERATOR_COMM,
	COUNT_RESET_COMM 
};

enum packetlengths_enum packetlengths[COMM_LIST_COUNT] = {
	WRITE_REGISTER_RECVLEN,
	START_CYCLE_RECVLEN,
	READ_REGISTER_RECVLEN,
	STOP_CYCLE_RECVLEN,
	READ_BUFFER_RECVLEN,
	START_GENERATOR_RECVLEN,
	COUNT_RESET_RECVLEN
}; 

enum command_args_num_enum command_args_num[COMM_LIST_COUNT] = {
	WRITE_REGISTER_ARGS,
	START_CYCLE_ARGS,
	READ_REGISTER_ARGS,
	STOP_CYCLE_ARGS,
	READ_BUFFER_ARGS,
	START_GENERATOR_ARGS,
	COUNT_RESET_ARGS 
};

char* args_message = "example: \"ip-address command [args] ...\"\n";

char* string_commands[] = {
	"writeregs",
	"start",
	"readregs",
	"stop",
	"readbuffer",
	"startgen",
	"countreset"
};

char* additional_commands[] = {
	"-S",
	"-debug_mode",
	"-file"
};

commandlist* init_commandlist(){
	commandlist* list = (commandlist*)malloc(sizeof(commandlist));
	list->next = NULL;
	return list;
}

commandlist* create_next_command_node(commandlist** curlist, int _commandnumber,  ...){
	commandlist* seekelem = *curlist;
	commandlist* list = init_commandlist(); 
	list->number = _commandnumber;
	list->input_number = inputcommands[_commandnumber];
	list->message_size = packetlengths[_commandnumber];
	list->args_count = command_args_num[_commandnumber];
	list->result = NULL;
	list->result_size = 0;
	list->args = NULL;
	if (list->args_count != 0)
		list->args = (int*)malloc(sizeof(int) * command_args_num[_commandnumber]);
	va_list args;
	va_start(args, _commandnumber);
	int i;
	for (i = 0; i < list->args_count; i++){
		int arg = va_arg(args, int);
		list->args[i] = arg;
	}
    va_end(args);

	if (seekelem == NULL){
		*curlist = list;
	}
	else {
		while (seekelem->next != NULL)
			seekelem = seekelem->next;
		seekelem->next = list;
	}
	return list;
}

int free_list(commandlist** curlist){
	commandlist* seekelem = *curlist;
	commandlist* prevelem = NULL;
	if (seekelem == NULL){
		return 0;
	}
	while (seekelem != NULL){
		seekelem->result = NULL;
		if (seekelem->args != NULL) free(seekelem->args);
		prevelem = seekelem;
		seekelem = seekelem->next;
		free(prevelem);
	}
	*curlist = NULL;
	return 0;
}

void my_strcpy(char** dest, const char* source){
	*dest = (char*)malloc(strlen(source) * sizeof(char) + 1);
	strcpy(*dest, source);
}

void dbg_printf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
