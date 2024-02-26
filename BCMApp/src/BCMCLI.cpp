#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <chrono>


#include "BCMDEV.h"
#include "PROTOBCM.h"
#include "PROTOHI.h"
#include "chk.h"

const char* arg0;
int debug_level = 4;

void Usage()
{
  const char* fmt =
    "Usage: %s -i ip [-init] [-pr] [-r R] [-w R,V] [-adc]\n"
    " -D debug_level : debug level 0-9\n"
    " -i ip: ip address A.B.C.D \n"
    " -init: init\n"
    " -start: internal start command\n"
    " -pr: print registers\n"
    " -r R: read register R\n"
    " -w R,V: write register R with V\n"
    " -startsrc|-ss 0|1|ext|int : start source 0 - external, 1 - internal \n"
    " -gain NN: gain \n"
    " -delsw NNNN: delay sw \n"
    " -adc NNNNN: prints first N bytes of buf \n"
    " -sw NNNN: length sw \n"
    " -set IP: write ip A.B.C.D to regs \n"
    " -commit: commit ip address \n"
    " -sleep ms: sleep ms \n"
    "\n"
    ;
  printf(fmt, arg0);
  exit(1);
}

#define KEY(key) strcmp(#key, argv[arg]) == 0

struct TCFG {
  int remote_start = 1; // 0 - external, 1 - internal
  int k_gain = 24;
  int ndel0 = 1;
} CFG;


using Device = PROTOHI<BCMDEV, PROTOBCM, TCFG>;

int main(int argc, char** argv){
  int err = -1;
  Device bcm;
  arg0 = argv[0];
  D(4,("count = %d\n", argc));
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
		else if(KEY(-adc)) {
      int size = 0;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &size) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
      D(0, ("size = %d\n", size));
      double arr[size];
			CHK(err = bcm.get_ADC_buffer(arr, size));
      ++arg;
		}
		else if(KEY(-r)) {
			int r;
			uint32_t v;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &r) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			CHK(err = bcm.read_register(r, &v));
			printf("reg %i = 0x%04x(%i)\n", r, v, v);
			++arg;
		}
		else if(KEY(-set)) {
			int a,b,c,d;
			char buf[80];
			char rot[] = "-/|\\";
			CHKTRUEMESG(sscanf(argv[arg+1], "%i.%i.%i.%i", &a, &b, &c, &d) == 4, ("Error: %s %s", argv[arg], argv[arg+1]));
			sprintf(buf, "%i.%i.%i.%i", a,b,c,d);
			printf("set ip %s\n", buf);
			CHK(err = bcm.write_register(Device::INFO::REG_IPWR, 1));
			CHK(err = bcm.write_register(Device::INFO::REG_WRIPHI, (a<<8)|b));
			CHK(err = bcm.write_register(Device::INFO::REG_WRIPLO, (c<<8)|d));
			CHK(err = bcm.execute_command(Device::INFO::CMD_FLASHWR));
			for(a = 9; a > 0; --a) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				printf("\r   %c  ", rot[a % (sizeof(rot)-1)]);
				fflush(stdout);
			}
			printf("\r       \r");
			CHK(err = bcm.execute_command(Device::INFO::CMD_FLASHRD));
			++arg;
    }
    else if(KEY(-commit)) {
      uint32_t hi, lo;
      char host[80];
      CHK(err = bcm.read_register(Device::INFO::REG_BUFIPHI, &hi));
      CHK(err = bcm.read_register(Device::INFO::REG_BUFIPLO, &lo));
      printf("commit ip %i.%i.%i.%i\n", hi >> 8, hi & 0xff, lo >> 8, lo & 0xff);
      CHK(err = bcm.execute_command(Device::INFO::CMD_IPREWR));
      sprintf(host, "%i.%i.%i.%i", hi >> 8, hi & 0xff, lo >> 8, lo & 0xff);
      CHK(err = bcm.connect(host, 2195));
      D(0, ("connect = %s\n", err >= 0 ? "success" : "fail"));
      CHKTRUE(err = bcm.is_connected());
      D(0, ("connected = %i\n", err));
      CHK(err = bcm.write_register(Device::INFO::REG_IPWR, 0));
    }
    else {
      printf("Error at arg %i = %s\n", arg, argv[arg]);
      Usage();
    }
  }
  return 0;
  if(0){
  }
  return 0;
CHK_ERR:
  return err < 0 ? err : -1;
}
