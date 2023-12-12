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

#include "waveFormMap.h"
#include "BCMMath.h"
#include "Timer.h"

#endif

#include "BCM.h"
#include "chk_dt.h"
#include "chk.h"

#define debug_level debug_level_ioc
extern int debug_level_ioc;

// Driver library
#include "PROTOHI.h"
#include "BCMDEV.h"
#include "PROTOBCM.h"


#define ALIAS(name) name

#define DATA_EVENT 2
#define CFG_EVENT 3

#define PROCESS_CFG_EVENT() do { \
  epicsEventTryWait(BCM.ready_cfg_event); \
  post_event(CFG_EVENT); \
  epicsEventWaitWithTimeout(BCM.ready_cfg_event, 1.0); \
} while(0)



#ifndef EPICS_VERSION_INT
#define VERSION_INT(v,r,m,p) (((v) << 24) | ((r) << 16) | ((m) << 8) | (p))
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
#define EXTRA_PHAS_ready_cfg field(PHAS,20)
#define EXTRA_PHAS_ready field(PHAS,20)


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
#include "gen_c.h"
#include "gen_init.h"

TBCM BCM;

PROTOHI<BCMDEV, PROTOBCM> Device;

epicsEventId curEvent = nullptr;

Timer timeoutConnection;
Timer lastConnectionTime;
Timer lastMeasurement;


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
  epicsTimeGetCurrent(&t1);
  D(0, ("success wait %i, fail wait %i, saverestore time: %g\n",
        success_count, fail_count, epicsTimeDiffInSeconds(&t1, &t0)));
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
  WFM(BCM.arr).resize(OSCSIZE);
  WFM(BCM.arrXt).linspace(WAVEFORM_LENGTH_TIME, WAVEFORM_LENGTH_TIME, WFM(BCM.arr).size());
  D(0, ("TRACE\n"));
  while(ioc_work) {
    if (BCM.connect != BCM.connected){
      if (BCM.connect){
        if (lastConnectionTime < timeoutConnection) {
          epicsEventWaitWithTimeout(work_event, 1.0);
          lastConnectionTime += 1;
          D(0, ("BCM.error = %d\n", BCM.error));
          continue;
        }
        lastConnectionTime = 0;
        timeoutConnection = (BCM.error + 1) * 2;
        CHK(Device.connect(BCM.hostname, BCM.portno));
        CHK(Device.set_start_mode(BCM.remote_start));
        CHK(Device.set_K_gain(BCM.gain));
        BCM.gainK = BCM.gain * 2;
        BCM.connected = Device.is_connected();
        BCM.error++;
        if (curEvent != nullptr)
          epicsEventSignal(curEvent);
      }
      else {
        Device.disconnect();
        BCM.connected = 0;
        BCM.error = 0;
      }
    }
    PROCESS_CFG_EVENT();
    if(!BCM.connected) {
      epicsEventWaitWithTimeout(work_event, 1.0);
      continue; // единственное что мы можем здесь сделать это проверить необходимость подключения
    }

    if(epicsEventWaitWithTimeout(work_event, 1.0) == epicsEventWaitOK) {
      D(3,("произошло обновление pv для записи или мониторинга\n"));
    }
    if(epicsEventTryWait(curEvent = BCM.wndBeg_event) == epicsEventWaitOK ||
        epicsEventTryWait(curEvent = BCM.wndLen_event) == epicsEventWaitOK) {
      if (BCM.wndBeg > BCM.wndLen){
        double temp = BCM.wndBeg;
        BCM.wndBeg = BCM.wndLen;
        BCM.wndLen = temp;
      }
    }
    count++;
    if (epicsEventTryWait(curEvent = BCM.osc_mode_event) == epicsEventWaitOK){
      if (BCM.osc_mode == 1) {
        int connecting = BCM.connected;
        if (connecting == 1){
          Device.start_measurement();
          Device.get_ADC_buffer(BCM.arr, BCM.arr_ne);
          lastMeasurement = 0;
          calcQ(&BCM);
          timeQ(&BCM);
          interpolate<double>(&BCM);
          BCM.osc_mode = 0;
          BCM.osc_mode_ready++;
          post_event(DATA_EVENT);
          post_event(CFG_EVENT);
        }
      }
      if (BCM.osc_auto) {
        Timer timer;
        timer = 0;
        if (timer.diff(lastMeasurement) > BCM.osc_auto_deadtime) {
          BCM.osc_mode = 1;
          post_event(CFG_EVENT);
          epicsEventWaitWithTimeout(BCM.osc_mode_event, 0.50);
          epicsEventWaitWithTimeout(BCM.osc_mode_event, 0.50);
          continue;
        }
      }
    }
    BCM.error = 0;
    continue;
CHK_ERR:
    Device.disconnect();
    BCM.connected = 0;
    BCM.error++;
    post_event(CFG_EVENT);
  }
  Device.disconnect();
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


