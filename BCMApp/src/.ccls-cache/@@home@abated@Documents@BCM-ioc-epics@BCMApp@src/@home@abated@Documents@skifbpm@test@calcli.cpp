#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <thread>
#include <chrono>

#define CALLIB_CPP
#include"udpLib.h"
#include"calLib.h"
#include"cacheDev.h"

int debug_level = 2;

const char* arg0;

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

#define KEY(key) strcmp(#key, argv[arg]) == 0

//using CALUDPCACHE = CALProto<DEVCAL, CACHE_RW(TUDPLIB)>;
//typedef CALUDPCACHE CAL;

int main(int argc, char** argv)
{
	int err = -1;
	CAL cal;
	struct CFG{
		int calibr_mode = 0; // 0 -pulse 1 - continueos
		int start_source = 1; // 0 - external, 1 - internal
		int dds_source = 1; // 0 - external, 1 - dds
		int gain = 30;
		int delsw = 0;
		int sw = 0;
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
			CHK(err = cal.connect(host, 2195));
			D(0, ("connect = %s\n", err >= 0 ? "success" : "fail"));
			CHKTRUE(err =  cal.is_connected());
			D(0, ("connected = %i\n", err));
		}
		else if(KEY(-init)) {
			CHK(err = cal.config(CFG));
		}
		else if(KEY(-dds)) {
			CHK(err = cal.dds());
		}
		else if(KEY(-pll)) {
			CHK(err = cal.pll());
		}
		else if(KEY(-w)) {
			int r;
			uint32_t v;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i,%i", &r, &v) == 2, ("Error: %s %s", argv[arg], argv[arg+1]));
			CHK(err = cal.write_reg(r, v));
			++arg;
		}
		else if(KEY(-r)) {
			int r;
			uint32_t v;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &r) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			CHK(err = cal.read_reg(r, &v));
			printf("reg %i = 0x%04x(%i)\n", r, v, v);
			++arg;
		}
		else if(KEY(-pr)) {
			CHK(err = cal.print_reg());
		}
		else if(KEY(-start)) {
			CHK(err = cal.start());
		}
		else if(KEY(-set)) {
			int a,b,c,d;
			char buf[80];
			char rot[] = "-/|\\";
			CHKTRUEMESG(sscanf(argv[arg+1], "%i.%i.%i.%i", &a, &b, &c, &d) == 4, ("Error: %s %s", argv[arg], argv[arg+1]));
			sprintf(buf, "%i.%i.%i.%i", a,b,c,d);
			printf("set ip %s\n", buf);
			CHK(err = cal.write_reg(CAL::INFO::REG_IPWR, 1));
			CHK(err = cal.write_reg(CAL::INFO::REG_WRIPHI, (a<<8)|b));
			CHK(err = cal.write_reg(CAL::INFO::REG_WRIPLO, (c<<8)|d));
			CHK(err = cal.exec_com(CAL::INFO::CMD_WRFLASH));
			for(a = 9; a > 0; --a) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				printf("\r   %c  ", rot[a % (sizeof(rot)-1)]);
				fflush(stdout);
			}
			printf("\r       \r");
			CHK(err = cal.exec_com(CAL::INFO::CMD_RDFLASH));
			++arg;
		}
		else if(KEY(-commit)) {
			uint32_t hi, lo;
			char host[80];
			CHK(err = cal.read_reg(CAL::INFO::REG_BUFIPHI, &hi));
			CHK(err = cal.read_reg(CAL::INFO::REG_BUFIPLO, &lo));
			printf("commit ip %i.%i.%i.%i\n", hi >> 8, hi & 0xff, lo >> 8, lo & 0xff);
			CHK(err = cal.exec_com(CAL::INFO::CMD_IPUPDATE));
			sprintf(host, "%i.%i.%i.%i", hi >> 8, hi & 0xff, lo >> 8, lo & 0xff);
			CHK(err = cal.connect(host, 2195));
			D(0, ("connect = %s\n", err >= 0 ? "success" : "fail"));
			CHKTRUE(err =  cal.is_connected());
			D(0, ("connected = %i\n", err));
			CHK(err = cal.write_reg(CAL::INFO::REG_IPWR, 0));
		}
		else if(KEY(-sleep)) {
			int ms;
			CHKTRUEMESG(sscanf(argv[arg+1], "%i", &ms) == 1, ("Error: %s %s", argv[arg], argv[arg+1]));
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			++arg;
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
	return err < 0 ? err : -1;;
}
