#ifndef BCM_H
#define BCM_H

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
  decl_zone##_ARR(int,arr,65536,-2048,2048,DATA_EVENT,{0}) \
  decl_zone##_ARR_RO(double,arrXt,65536,0,320,DATA_EVENT) \
  decl_zone##_VAR(int,gain,0,24,K_EVENT,1) \
  decl_zone##_VAR_RO(int,gainK,0,1000000,K2_EVENT) \
  decl_zone##_STR(hostname, 20, "192.168.147.9") \
  decl_zone##_VAR(int,portno,0,65535,DATA_EVENT,2195) \
  decl_zone##_BIN(connect, CONNECT_EVENT, 0) \
  decl_zone##_BIN_RO(connected, CONNECTED_EVENT) \
  decl_zone##_BIN(remote_start, DATA_EVENT, 0) \
  decl_zone##_VAR(double,wnd1,0.0,320.0,DATA_EVENT,0) \
  decl_zone##_VAR(double,wnd2,0.0,320.0,DATA_EVENT,0) \
  decl_zone##_BIN(minmax, DATA_EVENT, 0) \
  decl_zone##_BIN(update_stats, DATA_EVENT, 0) \
  decl_zone##_VAR_RO(double,Q,0,100000,DATA_EVENT) \
  decl_zone##_VAR(double,QK,0,100000,DATA_EVENT,1) \
  decl_zone##_VAR(double,wndBeg,0.0,320.0,DATA_EVENT,0.0) \
  decl_zone##_VAR(double,wndLen,0.0,320.0,DATA_EVENT,320.0) \
  decl_zone##_VAR_RO(double,timeQ,0,320,DATA_EVENT) \
  decl_zone##_VAR_RO(int,timeQY,-2048,2048,DATA_EVENT) \

#include "gen_h.h"

struct TBCM{
  DECL_BCM(STRUCT);
};
typedef struct TBCM TBCM;

#endif
