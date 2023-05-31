#include "BCMCommunication.h"

int createmessage(byte* array, int size, commandlist* commands){
	if (size < SEND_MESSAGE_SIZE)
		return 1;
	enum commands command = commands->number;
	array[0] = commands->input_number;
	int i;
	for (i = 1; i < size; i++)
		array[i] = 0x00;
	if (command == WRITE_REGISTER || command == READ_REGISTER){
		array[1] = commands->args[0];
	}
	if (command == READ_BUFFER){
		int firstpage = MIN_PAGE;
		int secondpage = MAX_PAGE;
		if (commands->args != NULL) {
			firstpage = commands->args[0];
			secondpage = commands->args[1];
		}
		array[2] = firstpage >> 8;
		array[3] = firstpage & 0xFF; 
		array[4] = secondpage >> 8;
		array[5] = secondpage & 0xFF; 
	}
	if (command == WRITE_REGISTER){
		int data = commands->args[1];
		array[2] = data >> 8;
		array[3] = data & 0xFF; 
	}
	return 0;
}

int read_packet(byte* buf, int length, connection_credentials* con){
	if (length == 0)
		return 1;
	int n = recvfrom(con->sockfd, buf, length, 0, NULL, NULL); 
	if (n < 0) {
		return 2;
	}
	return 0;
}

int read_register(byte* buf, commandlist* commands, connection_credentials* con){
	static int result;
	read_packet(buf, commands->message_size, con);
	result = (buf[2] << 8) | (buf[3] & 0xFF);
	commands->result = &result;
	commands->result_size = 1;
	return 0;
}

int readADC(byte* buf, commandlist* commands, connection_credentials* con){
	static int result[MAX_POINTS];
	int begin = commands->args[0];
	int end = commands->args[1];
	int count = end - begin + 1;
	int i;
	int j = 0;
	for (i = 0; i < count * PAGE_POINTS_SIZE; i++){
		result[i] = 0;
	}
	int k;
	int error = 0;
	for (k = 0; k < count; k++){
		int status = read_packet(buf, commands->message_size, con);
		if (status > 0){
			error++;
			if (error == 3)
				return NULL;
			continue;
		}
		for (i = PAGE_POINTS_START; i < commands->message_size; j++, i += 2){
			result[j] = ((buf[i] << 8) | (buf[i+1] & 0xFF)) - MAX_POINT_VALUE;
		}
	}	
	commands->result = result;
	commands->result_size = count * PAGE_POINTS_SIZE;
	return 0;
}

int command_execution(commandlist* commands, connection_credentials* con){
	byte buf[MAX_RECV_MESSAGE_SIZE];
	byte sendmessage[SEND_MESSAGE_SIZE];
	createmessage(sendmessage, SEND_MESSAGE_SIZE, commands);
	int n = sendto(con->sockfd, sendmessage, SEND_MESSAGE_SIZE, 0, &(con->serveraddr), sizeof(con->serveraddr));
	if (n < 0) {
		TRACE(("%d\n", n));
		return 1;
	}
	read_packet(buf, ACK_MESSAGE_SIZE, con);
	if (commands->number == READ_BUFFER){
		readADC(buf, commands, con);
	}
	if (commands->number == READ_REGISTER){
		read_register(buf, commands, con);
	}
	else {
		read_packet(buf, commands->message_size, con);
	}
	return 0;
}

int close_connection(connection_credentials* cred){
	close(cred->sockfd);
	return 0;
}

int initiate_connection(connection_credentials* cred){
	ssize_t n;
	int i;
	int serverlen;
	struct sockaddr_in* serveraddr;
	struct hostent *server;
	
	// Sockaddr pointing 
	serveraddr = &(cred->serveraddr); 

	// Creating a socket
	cred->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(cred->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	if (cred->sockfd < 0) 
		// Error while opening a socket
		return 1;
	server = gethostbyname(cred->hostname);
	if (server == NULL) 
		// No such host as hostname
		return 2;
	bzero((char *)serveraddr, sizeof(*serveraddr));
	serveraddr->sin_family = AF_INET;
	bcopy((char *)server->h_addr_list[0],  
			(char *)&(serveraddr->sin_addr.s_addr), server->h_length);
	serveraddr->sin_port = htons(cred->portno);
	return 0;
}
