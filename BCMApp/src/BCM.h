#ifndef BCM_H
#define BCM_H
#include "BCMDEV.h"

#define VAR_MBB_V0 0
#define VAR_MBB_V1 1
#define VAR_MBB_V2 2
#define VAR_MBB_V3 3

#define MBB_FIELD_mbb \
  MBB_FIELD_N(VAR_MBB_,V0) \
  MBB_FIELD_N(VAR_MBB_,V1) \
  MBB_FIELD_N(VAR_MBB_,V2) \
  MBB_FIELD_N(VAR_MBB_,V3)

#define MBB_FIELD_mbb_ro MBB_FIELD_mbb

/* PLACE_MBB_INSERT */

#define DECL_BCM(decl_zone) \
  /* общие параметры */ \
  decl_zone##_BIN(connect, CFG_EVENT, 0) \
  decl_zone##_BIN_RO(connected, CFG_EVENT) \
  decl_zone##_STR(hostname, 20, "192.168.147.9") \
  decl_zone##_VAR(int,portno,0,65535,CFG_EVENT,2195) \
  decl_zone##_VAR(int,ready_cfg,0,100000000,DATA_EVENT, 0) /* триггер параметров конфига, меняется последним*/ \
  /* параметры измерения */ \
  decl_zone##_VAR(int,k_gain,0,24,CFG_EVENT,1)  \
  decl_zone##_VAR(int,ndel0,0,15,CFG_EVENT,0)  \
  decl_zone##_BIN(remote_start, DATA_EVENT, 0) \
  decl_zone##_VAR(double,wnd1,0.0,320.0,DATA_EVENT,0) /* 1-ая граница интегрирования */ \
  decl_zone##_VAR(double,wnd2,0.0,320.0,DATA_EVENT,0) /* 2-ая граница интегрирования */ \
  decl_zone##_BIN(minmax, DATA_EVENT, 0) \
  decl_zone##_BIN(update_stats, DATA_EVENT, 0) \
  decl_zone##_VAR(double,wndBeg,0.0,320.0,DATA_EVENT,0.0) \
  decl_zone##_VAR(double,wndLen,0.0,320.0,DATA_EVENT,320.0) \
  decl_zone##_VAR(double,QK,0,100000,DATA_EVENT,1) \
  decl_zone##_BIN(osc_mode,CFG_EVENT, 0) /* триггер запроса результата */ \
  decl_zone##_BIN(osc_auto,CFG_EVENT, 0) /* триггер запроса результата */ \
  decl_zone##_VAR(int,osc_auto_deadtime, 0, 100000000, CFG_EVENT, 3) /* триггер запроса результата */ \
  decl_zone##_VAR(int,connection_timeout, 0, 100000000, CFG_EVENT, 3) /* триггер запроса результата */ \
  /* результат измерения обычно здесть только _RO переменные */ \
  decl_zone##_VAR_RO(int,osc_mode_ready,0,10000000000,CFG_EVENT) /* счетчик успешных выполнений */ \
  decl_zone##_VAR_RO(int,error,0,10000000000,CFG_EVENT) /* счетчик ошибок */\
  decl_zone##_VAR_RO(int,gain,0,1000000,CFG_EVENT) \
  decl_zone##_ARR_RO(double,arrXt,OSCSIZE,0,320,DATA_EVENT) \
  decl_zone##_ARR(double,arr,OSCSIZE,-2048,2048,DATA_EVENT, {0}) \
  decl_zone##_VAR_RO(double,Q,0,100000,DATA_EVENT) \
  decl_zone##_VAR_RO(double,timeQ,0,320,DATA_EVENT) \
  decl_zone##_VAR_RO(int,timeQY,-2048,2048,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parab0,100,-2048,2048,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parab1,100,-2048,2048,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parab2,100,-2048,2048,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parab3,100,-2048,2048,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parabx0,100,0,320,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parabx1,100,0,320,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parabx2,100,0,320,DATA_EVENT) \
  decl_zone##_ARR_RO(double,parabx3,100,0,320,DATA_EVENT) \



/* конец DECL_BCM  */

 
#include "gen_h.h"

struct TBCM {
  DECL_BCM(STRUCT);
};
typedef struct TBCM TBCM;

#endif
