#include "PROTOHI.h"
#include "BCMDEV.h"
#include "PROTOBCM.h"

int debug_level;
int current_argument;

void Usage()
{
	const char* fmt =
		"Usage: %s -i ip [-init] [-dds] [-pll] [-pr] [-r R] [-w R,V]\n"
		" -D debug_level : debug level 0-9\n"
		" -i ip: ip address A.B.C.D \n"
		" -init: init\n"
		" -dds: init dds\n"
		" -pll: init pll\n"
		" -start: internal start command\n"
		" -pr: print registers\n"
		" -r R: read register R\n"
		" -w R,V: write register R with V\n"
		" -mode 0|1|cont|pulse : calibr mode 0 - pulse, 1 - continuously \n"
		" -ddssrc|-ds 0|1|ext|int : dds source 0 - internal, 1 - external  \n"
		" -startsrc|-ss 0|1|ext|int : start source 0 - external, 1 - internal \n"
		" -gain NN: gain \n"
		" -delsw NNNN: delay sw \n"
		" -sw NNNN: length sw \n"
		" -set IP: write ip A.B.C.D to regs \n"
		" -commit: commit ip address \n"
		" -sleep ms: sleep ms \n"
		"\n"
		;

	printf(fmt, arg0);
	exit(1);
}

const int KEY(key){
  strcmp(key, argv[current_argument]);
}

int main(){

	typedef struct CFG {
	} CFG;

  debug_level = 2;
  PROTOHI<BCMDEV, PROTOBCM> Connection;
  Connection.connect("192.168.147.9", 2195);
  Connection.set_start_mode(true);
  Connection.set_K_gain(2);
  Connection.disconnect();
  return 0;
}



