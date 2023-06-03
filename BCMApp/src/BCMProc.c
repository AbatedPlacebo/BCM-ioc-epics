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

// Driver library
#include "BCMCommunication.h"
#include "BCMMath.h"
#include "BCMVariables.h"

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
#define debug_level debug_level_ioc
extern int debug_level_ioc;

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
#define EXTRA_Q field(PREC,3) field(EGU,"kQ")
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
commandlist* list;
commandlist* buffer;
commandlist* buf;
connection_credentials con;

static void BCM_run(void* arg);

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
	D(0, ("TRACE\n"));

	while(ioc_work) {
		if(	epicsEventWaitWithTimeout(work_event, 1.0) == epicsEventWaitOK) {
			D(3,("произошло обновление pv для записи или мониторинга\n"));
		}

		if(	epicsEventTryWait(BCM.connect_event) == epicsEventWaitOK) {
			int state;
			if (BCM.connect == 1){
				con.hostname = BCM.hostname;
				con.portno = BCM.portno;
				create_next_command_node(&list, START_GENERATOR);
				create_next_command_node(&list, WRITE_REGISTER, 0, BCM.remote_start << 1);
				create_next_command_node(&buffer, STOP_CYCLE);
				create_next_command_node(&buffer, START_CYCLE);
				create_next_command_node(&buffer, READ_BUFFER, 0, 127);
				if (initiate_connection(&con) == 0){
					BCM.connected = 1;
					commandlist* ptr = list;
					while (ptr != NULL){
						D(0,("%d\n", command_execution(ptr, &con)));
						ptr = ptr->next;
					}
				}
			}
			else {
				if (close_connection(&con) == 0){
					BCM.connected = 0;
					free_list(&buffer);
					free_list(&list);
				}
			}
			D(0, ("Connection: %d\n", BCM.connected));
			post_event(CONNECTED_EVENT);
			post_event(K_EVENT);
		}
		if(	epicsEventTryWait(BCM.gain_event) == epicsEventWaitOK) {
			create_next_command_node(&buf, WRITE_REGISTER, 2, BCM.gain);
			command_execution(buf, &con);
			BCM.gainK = BCM.gain * 2;
			free_list(&buf);
			post_event(K2_EVENT);
		}
		if(	epicsEventTryWait(BCM.wndBeg_event) == epicsEventWaitOK ||
			epicsEventTryWait(BCM.wndLen_event) == epicsEventWaitOK) {
				if (BCM.wndBeg > BCM.wndLen){
					int temp = BCM.wndBeg;
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
			commandlist* ptr = buffer;	
			while (connecting == 1 && ptr != NULL){
				command_execution(ptr, &con);
				int i, j;
				switch(ptr->number){
					case READ_BUFFER:
						D(0,("reading %d\n", ptr->result_size));
						if (ptr->result_size == 0)
							break;
						for (i = 0, j = 0; i < ptr->result_size; i++, j++)
							BCM.arr[i] = ptr->result[i];
						BCM.arr_ne = ptr->result_size;
						if (BCM.QK == 0)
							BCM.QK = 1;
						BCM.Q = calcQ(BCM.arr, BCM.wndLen, BCM.wnd1, BCM.wnd2, BCM.QK, BCM.gain, BCM.gainK);
						BCM.timeQ = timeQ(BCM.arr, BCM.wndLen, BCM.wnd1, BCM.wnd2, BCM.minmax);
						break;
					default:
						break;
				}
				ptr = ptr->next;
			}
			post_event(DATA_EVENT);
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
	
