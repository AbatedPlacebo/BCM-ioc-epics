#include "BCMVariables.h"

// 0 - disabled
// 1 - enabled
int debug_mode = 0;

int inputcommands[] = {
	0x00, // write register
	0x03, // start cycle
	0x04, // read register
	0x05, // reset cycle
	0x08, // read ADC
	0x06, // initialize generator
	0x07, // reset counter
	0x09, // flash write
	0x0a, // ip rewrite
	0x0f  // flash read
};	

int packetlengths[] = {
	0,
	2,
	4,
	0,
	1034,
	2,
	0
};


int command_args_num[] = {
	2,
	0,
	1,
	0,
	2,
	0,
	0
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
	list->output = 0;
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
		free(seekelem->result);
		if (seekelem->args != NULL) free(seekelem->args);
		seekelem->file_name = NULL;
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
