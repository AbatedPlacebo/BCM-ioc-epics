// Communication
#ifndef BCM_COMMUNICATION_H
#define BCM_COMMUNICATION_H 

#include "BCMVariables.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// Initiating connection between the host and BCM
int initiate_connection(connection_credentials* cred);

// Closing connection
int close_connection(connection_credentials* cred);

// Executing command
int command_execution(commandlist* commands, connection_credentials* con);

#endif
