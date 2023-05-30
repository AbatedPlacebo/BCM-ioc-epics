/* stabMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"

extern "C" int wait_exit();

char* argv0;
char* argv1;

enum ctl {ctlInit, ctlRun, ctlPause, ctlExit};
extern volatile enum ctl  castcp_ctl;

extern "C" int check_castcp_ctl()
{
	if(castcp_ctl != ctlRun) {
		printf("ERROR: castcp_ctl=%i\n", castcp_ctl);
		epicsExit(0);
	}
	return 0;
}

int main(int argc,char *argv[])
{
	argv0 = argv[0];
	if(argc>=2) {
		argv1 = argv[1];
		iocsh(argv[1]);
		epicsThreadSleep(.2);
	}
	check_castcp_ctl();
	iocsh(NULL);
	wait_exit();
	epicsExit(0);
	return(0);
}
