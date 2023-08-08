#include "UDPLIB.h"
#include "PROTOMED.h"
#include "BCMDEV.h"

int debug_level;

int main(){
  debug_level = 0;
  PROTOHI<BCMDEV, UDPLIB> Connection;
  Connection.write_register(BCMDEV::REG::R0, BCMDEV::CONSTANTS::EXTERNAL_START);
  return 0;
}
