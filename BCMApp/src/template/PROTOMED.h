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
#include "UDPLIB.h"

extern int debug_level;

template <typename DEV, typename PROTOLOW> class PROTOMED : public PROTOLOW {
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
      CHK(err = PROTOLOW::send_com(DEV::CMD::CMD_WRREG, regn, param)); 
      for (cnt = 1; cnt > 0; --cnt) { 
      CHK(err = PROTOLOW::recv_to(ack, sizeof(ack), 10/*, 0*/));
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
    int rd_reg(unsigned int regn, unsigned int *param)
    {
      int err = -1;
      uint8_t ack[4];
      int cnt;
      int rep = 1;
REP:
      D(3,("read_reg %i\n", regn));
      CHK(err = send_com(DEV::CMD_RDREG, regn, 0));
      for(cnt = 2; cnt > 0; --cnt) {
        CHK(err = PROTOLOW::recv_to(ack, sizeof(ack), 10/*, 0*/));
        if(err == 0 && rep > 0) {
          D(2,("repeat %i TUDPLIB<>::read_reg %s\n", rep, __FUNCTION__));
          --rep;
          goto REP;
        }
        if(err == 2 && ack[0] == 0x11) {
          ++cnt;
          PROTOLOW::m_conf++;
          continue;
        }
        CHKTRUE(err == sizeof(ack));
        CHKTRUE(ack[0] == 0xF4 || ack[0] == 0x10);
        if(ack[0] == 0x10) {
          int pack = 1;
          WARNTRUE(ack[0] == 0x10                     || (pack = 0));
          WARNTRUE(ack[1] == DEV::CMD_RDREG           || (pack = 0));
          WARNTRUE(ack[2] == regn                     || (pack = 0));
          WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
          if(pack == 0)
            D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
        }
        else if(ack[0] == 0xf4) {
          int pack = 1;
          WARNTRUE(ack[0] == 0xF4                     || (pack = 0));
          WARNTRUE(ack[1] == regn                     || (pack = 0));
          if(pack == 0)
            D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
          *param = (((unsigned int) ack[2]) << 8) | ack[3];
          D(3,("val %i(%04x)\n", *param, *param));
        }
      }
      return err;

CHK_ERR:
      if(err > 0)
        CUNET_PRINT(2, "ack", ack, err);
      return -1;
      return 0;
    }
    int start(){
      int err = -1;
      uint8_t ack[DEV::ACK:ACK_LENGTH];
      PROTOLOW::conf();
      CHK(err = send_com(DEV::CMD_START, 0, 0));
      CHK(err = PROTOLOW::recv_to(ack, sizeof(ack), 10/*, 0*/));
      CHKTRUE(err == sizeof(ack));
      CHKTRUE(ack[0] == 0x10);
      CHKTRUE(ack[1] == DEV::CMD_START);
      CHKTRUE(ack[2] == 0);
      CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
      return err;
CHK_ERR:
      if(err > 0)
        CUNET_PRINT(2, "ack", ack, err);
      return -1;
    }
    int stop(){
      int err = -1;
      uint8_t ack[4];
      int cnt;
      int rep = 1;
REP:
      CHK(err = send_com(DEV::CMD_STOP, 0, 0));
      for(cnt = 1; cnt > 0; --cnt) {
        CHK(err = PROTOLOW::recv_to(ack, sizeof(ack), 10/*, 0*/));
        if(err == 0 && rep > 0) {
          D(2,("repeat %i %s\n", rep, __FUNCTION__));
          --rep;
          goto REP;
        }
        if(err == 2 && ack[0] == 0x11) {
          ++cnt;
          continue;
        }
        CHKTRUE(err == sizeof(ack));
        CHKTRUE(ack[0] == 0x10);
        CHKTRUE(ack[1] == DEV::CMD_STOP);
        CHKTRUE(ack[2] == 0);
        CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
      }
      PROTOLOW::conf();
      return err;
CHK_ERR:
      PROTOLOW::conf();
      if(err > 0)
        CUNET_PRINT(2, "ack", ack, err);
      return -1;
    }

};

#endif
