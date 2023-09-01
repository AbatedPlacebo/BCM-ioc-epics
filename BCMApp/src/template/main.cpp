#include "PROTOHI.h"
#include "BCMDEV.h"
#include "PROTOBCM.h"

int debug_level;

int main(){
  debug_level = 2;
  PROTOHI<BCMDEV, PROTOBCM> Connection;
  Connection.connect("192.168.147.9", 2195);
  Connection.set_start_mode(true);
  Connection.set_K_gain(2);
  Connection.disconnect();
  return 0;
}
