#if !(defined(GEN_DB) || defined(GEN_DBD) || defined(GEN_WIKI) || defined(GEN_PY))
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <execinfo.h>

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
#include "Timer.h"

#define debug_level debug_level_ioc
extern int debug_level_ioc;

extern int error_timeout;

#include "chk_dt.h"
#include "chk.h"

// Driver libraries
#include "PROTOBCM.h"
#include "PROTOHI.h"
#include "BCMDEV.h"
#include "BCMMath.h"
#include "cacheDev.h"

#endif

#include "BCM.h"

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
#define EXTRA_PHAS_ready_cfg field(PHAS,24)
#define EXTRA_PHAS_ready field(PHAS,20)

/* Extra parameters for PV */

#define EXTRA_timeQ field(PREC,3) field(EGU,"us")
#define EXTRA_Q field(PREC,3) field(EGU,"nQ")
#define EXTRA_timeQY field(PREC,3) field(EGU,"mA")
#define EXTRA_minmax field(ZNAM,"MIN") field(ONAM,"MAX")
#define EXTRA_current_coef field(PREC,3)

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


#define TIMEOUT_LIMIT 10


TBCM BCM;

PROTOHI<BCMDEV, SCACHE_RW<PROTOBCM>::type, TBCM> Device;

SignalProcessing<TBCM> Math(&BCM);

epicsEventId curEvent = nullptr;

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
}

static void BCM_run(void* arg)
{
  ioc_work = 1;
  bcm_wait_autosave(5.0);
  double val = 0;
  int i;
  double timeout = 0;
  const double WAVEFORM_LENGTH_TIME = (double)BCMDEV::MAX_OSC_TIME / (double)BCMDEV::MAX_POINTS;
  WFM(BCM.arr).resize(OSCSIZE);
  WFM(BCM.arrXt).linspace(WAVEFORM_LENGTH_TIME, WAVEFORM_LENGTH_TIME, WFM(BCM.arr).size());
  Timer t0;
  D(0, ("Starting IOC...\n"));
  while(ioc_work) {
    if (BCM.connect != BCM.connected){
      if (BCM.connect){
        if (lastConnectionTime() < timeout) {
          epicsEventWaitWithTimeout(work_event, 1.0);
          continue;
        }
        lastConnectionTime = 0;
        timeout = (timeout <= TIMEOUT_LIMIT) ? error_timeout * 2 : TIMEOUT_LIMIT;
        CHK(Device.connect(BCM.hostname, BCM.portno));
        CHK(Device.config(BCM));
        BCM.connected = Device.is_connected();
        if (curEvent != nullptr)
          epicsEventSignal(curEvent);
      }
      else {
        Device.disconnect();
        BCM.connected = 0;
        error_timeout = 0;
      }
      PROCESS_CFG_EVENT();
    }
    if(!BCM.connected) {
      epicsEventWaitWithTimeout(work_event, 1.0);
      continue; // единственное что мы можем здесь сделать это проверить необходимость подключения
    }
    if(epicsEventTryWait(curEvent = BCM.wndBeg_event) == epicsEventWaitOK ||
        epicsEventTryWait(curEvent = BCM.wndEnd_event) == epicsEventWaitOK) {
      if (BCM.wndBeg > BCM.wndEnd){
        double temp = BCM.wndBeg;
        BCM.wndBeg = BCM.wndEnd;
        BCM.wndEnd = temp;
      }
    }

    if(epicsEventTryWait(curEvent = BCM.update_stats_event) == epicsEventWaitOK) {
      if (BCM.update_stats == 1){
        Math.doAll();
        BCM.update_stats = 0;
        post_event(DATA_EVENT);
      }
      continue;
    }
    if (BCM.osc_mode){
        CHK(Device.start_measurement());
        CHK(Device.get_ADC_buffer(WFM(BCM.arr), BCM.wndBeg, BCM.wndEnd));
        WFM(BCM.arr) *= BCM.current_coef;
        lastMeasurement = 0;
        Math.doAll();
        BCM.osc_mode = 0;
        BCM.osc_mode_ready++;
        post_event(DATA_EVENT);
        post_event(CFG_EVENT);
    }
    if (BCM.osc_auto) {
      if (lastMeasurement() >= BCM.osc_auto_deadtime) {
        BCM.osc_mode = 1;
        post_event(CFG_EVENT);
        epicsEventWaitWithTimeout(BCM.osc_mode_event, 0.50);
        epicsEventWaitWithTimeout(BCM.osc_mode_event, 0.50);
        continue;
      }
    }
    CHK(Device.config(BCM));
    continue;
CHK_ERR:
    Device.disconnect();
    BCM.connected = 0;
    BCM.error++;
    error_timeout++;
    D(1, ("Current error = %d\n", error_timeout));
    post_event(CFG_EVENT);
  }
  Device.disconnect();
  D(0, ("Exiting...\n"));
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
    D(1,("wait exit\n"));
    epicsThreadSleep(1);
  }
  return 0;
}

#endif


DECL_C_VAR(int,debug_level_ioc,1)
DECL_C_VAR(int,debug_sub_exec,0)
DECL_C_VAR(int,error_timeout,0)
#define DECL_GLOBAL_PREF BCM.
DECL_BCM(DECL_GLOBAL)
//DECL_VAR_FUNC(int,command,0,3)
//MEAS_LIST(CA_MEAS_DATA)
DECL_FUNC_i(BCM_init)


