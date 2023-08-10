#include "UDPLIB.h"
#include "PROTOHI.h"
#include "BCMDEV.h"

int debug_level;

int main(){
  debug_level = 2;
  int err = -1;
  PROTOHI<BCMDEV, UDPLIB> Connection("192.168.147.9", 2195);
  Connection.set_start_mode(true);
  return 0;
}
