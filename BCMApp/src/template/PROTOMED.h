#ifndef PROTOMED_H
#define PROTOMED_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "chk.h"

extern int debug_level;

template <typename DEV, typename PROTOLOW> class PROTOMED {
  private:
    PROTOLOW LOW;
  public:
    PROTOMED(){ }; 
    ~PROTOMED(){ };

    int wr_reg(unsigned int regn, unsigned int param)
    {
      int err = -1;
      uint8_t ack[DEV::CONSTANTS::ACK_LENGTH];
      int cnt;
      int rep = 1;
REP:
      D(3,("write_reg %i %i(%04x)\n", regn, param, param));
      CHK(err = LOW.send_com(DEV::CMD::CMD_WRREG, regn, param)); for(cnt = 1; cnt > 0; --cnt) { CHK(err = LOW.recv_to(ack, sizeof(ack), 10/*, 0*/));
        if(err == 0 && rep > 0) {
          D(2,("repeat %i TUDPLIB<>::write_reg\n", rep));
          --rep;
          goto REP;
        }
        if(err == 2 && ack[0] == DEV::CONSTANTS::CONF_PACKET) {
          ++cnt;
          continue;
        }
        CHKTRUEMESG(err == sizeof(ack),("err=%i\n", err));
        CHKTRUE(ack[0] == DEV::CONSTANTS::ACK_PACKET);
        CHKTRUE(ack[1] == DEV::CMD::CMD_WRREG);
        CHKTRUE(ack[2] == regn);
        CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
      }
      return err;
CHK_ERR:
      if(err > 0)
        D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
      return -1;
    }
    int rd_reg(unsigned int reg, unsigned int *val)
    {
      return 0;
    }
    int start()
    {

      return 0;
    }
    int stop()
    {

      return 0;
    }


};

#endif
