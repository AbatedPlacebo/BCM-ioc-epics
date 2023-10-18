#include "PROTOHI.h"
#include "BCMDEV.h"
#include "PROTOBCM.h"

int debug_level = 2;
const char* arg0;

void Usage()
{
	const char* fmt = "Wrong usage\n";

	printf(fmt, arg0);
	exit(1);
}

#define KEY(key) strcmp(#key, argv[arg]) == 0

int main(){
	int err = -1;
  PROTOHI<BCMDEV, PROTOBCM> bcm;
	typedef struct CFG {

	} CFG;
	arg0 = argv[0];
	for(int arg = 0; arg < argc; ++arg) {
		D(4,("%i %s\n", arg, argv[arg]));
		if(arg == 0) {
			if(argc == 1)
				Usage();
		}
		else if(KEY(-D)) {
			int d;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &d) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			debug_level = d;
			++arg;
		}
		else if(KEY(-i)) {
			const char* host = argv[++arg];
			CHK(err = bcm.connect(host, 2195));
			D(0, ("connect = %s\n", err >= 0 ? "success" : "fail"));
			CHKTRUE(err =  bcm.is_connected());
			D(0, ("connected = %i\n", err));
		}
		else if(KEY(-init)) {
			CHK(err = bcm.config(CFG));
		}

  }
  return 0;
}



