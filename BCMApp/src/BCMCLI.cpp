#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <chrono>

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

int main(int argc, char** argv){
  int err = -1;
  PROTOHI<BCMDEV, PROTOBCM> bcm;
  struct CFG {
		int start_mode = 1; // 0 - external, 1 - internal
		int k_gain = 24; 
    int ndel0 = 1;
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
		else if(KEY(-sleep)) {
			int ms;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &ms) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			++arg;
		}
    else if(KEY(-init)) {
      CHK(err = bcm.config(CFG));
    }
		else if(KEY(-start)) {
			CHK(err = bcm.start_measurement());
		}
		else if(KEY(-r)) {
			int r;
			uint32_t v;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &r) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			//CHK(err = bcm.rd_reg(r, &v)); ??
			printf("reg %i = 0x%04x(%i)\n", r, v, v);
			++arg;
		}
		else {
			printf("Error at arg %i = %s\n", arg, argv[arg]);
			Usage();
		}
  return 0;
CHK_ERR:
	return err < 0 ? err : -1;
  }
  return 0;
}
