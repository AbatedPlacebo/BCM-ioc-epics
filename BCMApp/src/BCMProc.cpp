#if !(defined(GEN_DB) || defined(GEN_DBD) || defined(GEN_WIKI) || defined(GEN_PY))
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <execinfo.h>

//#include <errno.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/inotify.h>
//#include <sys/stat.h>


#include <dbDefs.h>
#include <registryFunction.h>
#include <subRecord.h>
#include <aSubRecord.h>
#include <epicsExport.h>
#include <epicsAssert.h>
#include <epicsEvent.h>
#include <epicsThread.h>
#include <epicsExit.h>
#include <dbScan.h>
#include <iocsh.h>
#include <errlog.h>
#include <iocLog.h>
#endif

#include"BCM.h"
#include"chk_dt.h"
#include"chk.h"

// Driver library
#include "PROTOHI.h"
#include "BCMDEV.h"
#include "PROTOBCM.h"

//#define debug_level debug_level_ioc
extern int debug_level_ioc;

int debug_level = 2;

#define ALIAS(name) name

#define DATA_EVENT 2
#define K_EVENT 3
#define K2_EVENT 4
#define CONNECT_EVENT 5
#define CONNECTED_EVENT 6


#ifndef EPICS_VERSION_INT
#define VERSION_INT(v,r,m,p) (((v)<<24) | ((r) << 16) | ((m) << 8) | (p))
#define EPICS_VERSION_INT VERSION_INT(EPICS_VERSION, EPICS_REVISION, EPICS_MODIFICATION, EPICS_PATCH_LEVEL)
#endif

#if EPICS_VERSION_INT >= VERSION_INT(3,15,0,0)
#define STR(s) #s
#define post_event(str) postEvent(eventNameToHandle(STR(str)))
#endif

#if defined(GEN_DB)
/****************************************************************************
 *                                                                          *
 *  GEN_DB  GEN_DB  GEN_DB  GEN_DB  GEN_DB  GEN_DB  GEN_DB                  *
 *                                                                          *
 ****************************************************************************/
#define TPRO_VAR 0
#define TPRO_BIN 0
#define TPRO_MBB 0
#define EXTRA_PHAS_var_ro field(PHAS,2)
#define EXTRA_PHAS_bin_ro field(PHAS,3)
#define EXTRA_PHAS_mbb_ro field(PHAS,5)
#define EXTRA_PHAS_arr_ro field(PHAS,4)

/* PLACE_EXTRA_INSERT */

#define EXTRA_timeQ field(PREC,3) field(EGU,"us")
#define EXTRA_Q field(PREC,3) field(EGU,"nQ")
#define EXTRA_minmax field(ZNAM,"MIN") field(ONAM,"MAX")

#include"gen_db.h"
#elif defined(GEN_DBD)
/****************************************************************************
 *                                                                          *
 *  GEN_DBD   GEN_DBD   GEN_DBD   GEN_DBD   GEN_DBD   GEN_DBD               *
 *                                                                          *
 ****************************************************************************/
#include"gen_dbd.h"
#elif defined(GEN_WIKI)
/****************************************************************************
 *                                                                          *
 *  GEN_WIKI   GEN_WIKI   GEN_WIKI   GEN_WIKI   GEN_WIKI   GEN_WIKI         *
 *                                                                          *
 ****************************************************************************/
#include"gen_wiki.h"
#elif defined(GEN_PY)
/****************************************************************************
 *                                                                          *
 *  GEN_PY  GEN_PY  GEN_PY  GEN_PY  GEN_PY  GEN_PY  GEN_PY  GEN_PY  GEN_PY  *
 *                                                                          *
 ****************************************************************************/
#include"gen_py.h"
#else
/****************************************************************************
 *                                                                          *
 *  C   C   C   C   C   C   C   C   C   C   C                               *
 *                                                                          *
 ****************************************************************************/
//#define sINFO_MESSAGE(fname) printf("process: %s\n", #fname);
#include"gen_c.h"
#include"gen_init.h"

TBCM BCM;

PROTOHI<BCMDEV, PROTOBCM> Device;

static void BCM_run(void* arg);

// WAIT
#define WAIT_PREF(name) BCM.name

static int bcm_wait_autosave(double timeout)
{
	epicsTimeStamp t0;
	epicsTimeStamp t1;
	double to;
	int success_count = 0;
	int fail_count = 0;
	epicsTimeGetCurrent(&t0);
	DECL_BCM(WAIT);
	epicsTimeGetCurrent (&t1);
	D(0, ("success wait %i, fail wait %i, saverestor time: %g\n",
				success_count, fail_count, epicsTimeDiffInSeconds( &t1, &t0)));
	return 0;
}

static void BCM_init(int i)
{
	D(0, ("TRACE\n"));
	D(0, ("argv0=%s argv1=%s\n", argv0, argv1));
	sig_handler_install();
	if(work_event==0) {
		work_event = epicsEventCreate(epicsEventEmpty);
#define INIT_PREF(v) BCM.v
		DECL_BCM(INIT);
	}

	if(i!=0) {
		if(work_th==0) {
			work_th = epicsThreadCreate("BCM_run",
					epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium),
					BCM_run, NULL);
		}
		else {
			printf("already executed\n");
		}
	}
	else {
		ioc_work = 0;
		sig_handler_uninstall();
	}
	// stab_run(NULL);
	D(0, ("TRACE\n"));
}

static void BCM_run(void* arg)
{
	int count = 0;
	ioc_work = 1;
	bcm_wait_autosave(5.0);
	BCM.gainK = 2;
	BCM.QK = 1;
	double val = 0;
	int i;
	for (i = 0; i < BCM.arr_ne; i++){
		BCM.arrXt[i] = val; 
	//	val += BCMWAVEFORM_LENGTH_TIME;
	}
	BCM.arrXt_ne = BCM.arr_ne;
	D(0, ("TRACE\n"));
	while(ioc_work) {
		if(	epicsEventWaitWithTimeout(work_event, 1.0) == epicsEventWaitOK) {
			D(3,("произошло обновление pv для записи или мониторинга\n"));
		}

		if(	epicsEventTryWait(BCM.connect_event) == epicsEventWaitOK) {
			int state;
      BCM.connected = Device.connect(BCM.hostname, BCM.portno);
			D(0, ("Connection: %d\n", BCM.connected));
			post_event(CONNECTED_EVENT);
			post_event(K_EVENT);
		}
		if(	epicsEventTryWait(BCM.gain_event) == epicsEventWaitOK) {
      Device.set_K_gain(BCM.gain);
			BCM.gainK = BCM.gain * 2;
			post_event(K2_EVENT);
		}
		if(	epicsEventTryWait(BCM.update_stats_event) == epicsEventWaitOK) {
//			BCM.Q = calcQ(BCM.arr, BCM.wndLen, BCM.wnd1, BCM.wnd2, BCM.QK, BCM.gain, BCM.gainK);
//			BCM.timeQ = timeQ(BCM.arr, &BCM.timeQY, BCM.wndLen, BCM.wnd1, BCM.wnd2, BCM.minmax);
			BCM.update_stats = 0;
		}
		if(	epicsEventTryWait(BCM.wndBeg_event) == epicsEventWaitOK ||
			epicsEventTryWait(BCM.wndLen_event) == epicsEventWaitOK) {
				if (BCM.wndBeg > BCM.wndLen){
					double temp = BCM.wndBeg;
					BCM.wndBeg = BCM.wndLen;
					BCM.wndLen = temp;
				}
		}
		//
		//epicsEventWaitWithTimeout(work_event, 1.0);
		//stab.injection = count;
		count++;
		if((count % 5) == 0) {
			int connecting = BCM.connected; 
			while (connecting == 1){
        Device.get_ADC_buffer(BCM.arr, BCM.arr_ne);
			}
			post_event(DATA_EVENT);
			post_event(K2_EVENT);
		}
		//if(count>10)
		//	ioc_work = 0;
	}
	D(0, ("TRACE\n"));
	if(signal_exit) {
		term_echo_on();
		epicsExit(-1);
	}
	work_th = (epicsThreadId)0;
}

int wait_exit()
{
	ioc_work = 0;
	while(work_th) {
		D(2,("wait exit\n"));
		epicsThreadSleep(1);
	}
	return 0;
}


#endif


DECL_C_VAR(int,debug_level_ioc,2)
DECL_C_VAR(int,debug_sub_exec,0)
#define DECL_GLOBAL_PREF BCM.
DECL_BCM(DECL_GLOBAL)
//DECL_VAR_FUNC(int,command,0,3)
//MEAS_LIST(CA_MEAS_DATA)
DECL_FUNC_i(BCM_init)
	
