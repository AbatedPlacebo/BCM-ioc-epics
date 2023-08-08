#include "UDPLIB.h"
#include "PROTO.h"
#include "BCMDEV.h"

int debug_level;

int main(){
  debug_level = 0;
  PROTOHI<BCMDEV, UDPLIB> Connection;
  Connection.wr_reg(BCMDEV::R0::VAL, BCMDEV::R0::ENABLE);
  return 0;
}
