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

#include "chk_dt.h"
#include "chk.h"
#include "UDPLIB.h"

extern int debug_level;

UDPLIB::UDPLIB():sock(-1)
{
  conf();
}

UDPLIB::~UDPLIB()
{
  close();
}

int UDPLIB::close()
{
  if (sock >= 0) {
    shutdown(sock, SHUT_RDWR);
    ::close(sock);
    sock = -1;
  }
  return 0;
}

int UDPLIB::disconnect()
{
  close();
  return 0;
}

int UDPLIB::is_connected() const
{
  return ((sock >= 0) ? 1 : 0);
}

int UDPLIB::socket()
{
  return sock;
}

int UDPLIB::conf()
{
  int ret = m_conf;
  m_conf = 0;
  return ret;
}


#define __TARGET_LINUX__
#define SOCKET int
#define NO_ERROR 0
#define closesocket close

static int SetSockTO(SOCKET s, int to_msec)
{
  int Error;
#ifdef __TARGET_WIN32__
  int timeout = to_msec;
#elif defined(__TARGET_LINUX__)
  struct timeval timeout;
  timeout.tv_sec = to_msec / 1000;
  timeout.tv_usec = (to_msec % 1000) * 1000;
#else
#error not implement
#endif

  Error = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));
  if (Error != NO_ERROR) {
    assert(Error != NO_ERROR);
    D(0, ("Error SO_SNDTIMEO error=%i s=%i to=%i\n", Error, s, to_msec));
  }
#ifdef __TARGET_WIN32__
  timeout = to_msec;
#elif defined(__TARGET_LINUX__)
  timeout.tv_sec = to_msec / 1000;
  timeout.tv_usec = (to_msec % 1000) * 1000;
#else
#error not implement
#endif
  Error = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
  if (Error != NO_ERROR) {
    assert(Error != NO_ERROR);
    D(0, ("error so_rcvtimeo\n"));
  }
  return (Error);
}

int UDPLIB::connect(const char *peer, int port)
{
  struct hostent *host;

  //unet_regs(con->regs);

  CHKTRUE((host = gethostbyname(peer)) != NULL);
  CHK(sock = ::socket(AF_INET, SOCK_DGRAM, 0));

  memset(&serv_addr, 0, sizeof(serv_addr));
  memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  CHK(SetSockTO(sock, 2500));

  return 0;

CHK_ERR:
  close();
  return -1;
}

int print_buf(uint8_t* buf, int buf_size)
{
  int i;
  Dcont(0, ("size=%i buf=0x", buf_size));
  for(i=0; i < buf_size && i < 30; i++) {
    if(i == 10 || (i>10 && ((i-10)%4) == 0))
      Dcont(0, (" "));
    Dcont(0, ("%02x", buf[i]));
  }
  if(i < buf_size)
    Dcont(0,("...\n"));
  else
    Dcont(0,("\n"));
  return 0;
}

int UDPLIB::send(void *buf, int buf_size)
{
  int err = -1;
  CHKTRUE(sock >= 0);
  Dif(4) { D(0,("send "));print_buf((uint8_t*)buf, buf_size); }
  CHK((err = (::sendto(sock, buf, buf_size, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))));
  return err;

CHK_ERR:
  close();
  return err;
}

int UDPLIB::recv(void *buf, int buf_size)
{
  int err = -1;
  struct sockaddr addr;
  socklen_t addrlen = sizeof(addr);
  CHKTRUE(sock >= 0);
  CHK((err = (::recvfrom(sock, buf, buf_size, 0, &addr, &addrlen))));
  Dif(4) { D(0,("recv "));print_buf((uint8_t*)buf, err); }
  return err;

CHK_ERR:
  close();
  return err;
}

int UDPLIB::send_com(int instr, int nreg, int param1, int param2)
{
  int err = -1;
  unsigned char sen[6];
  CHKTRUE(is_connected());
  D(3,("send_com2 %i nreg %i(%04x) param %i(%04x) %i(%04x)\n", instr, nreg, nreg, param1, param1, param2, param2));
  sen[0] = instr;
  sen[1] = nreg;
  sen[2] = param1 >> 8;
  sen[3] = param1;
  sen[4] = param2 >> 8;
  sen[5] = param2;
  CHK(err = send(&sen[0], sizeof(sen)));
CHK_ERR:
  return err;
}

int UDPLIB::ack_to(int cmd, int regn, int to_ms, int count, int repeat)
{
  int err = -1;
  uint8_t ack[4];
  int cnt;
  int rep = repeat;
REP:
  D(3,("ack_to(%i,%i)\n", count, to_ms));
  for(cnt = count; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), to_ms));
    if(err == 0 && rep > 0) {
      D(2,("repeat %i UDPLIB<>::write_reg\n", rep));
      --rep;
      goto REP;
    }
    if(err == 2 && ack[0] == 0x11) {
      ++cnt;
      continue;
    }
    CHKTRUEMESG(err == sizeof(ack),("err=%i\n", err));
    CHKTRUE(ack[0] == 0x10);
    CHKTRUE(ack[1] == cmd);
    CHKTRUE(ack[2] == regn);
    CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
  }
  return err;
CHK_ERR:
  if(err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

int UDPLIB::recv_to(void *_buf, int _size, int _to_ms, const int unet_flag)
{ ;
  int err = -1;
  if(unet_flag == 0)
    CHKTRUE((err = ready_udp(_to_ms)) > 0);
  else {
    if(!((err = ready_udp(_to_ms)) > 0))
      goto CHK_ERR;
  }
  CHK(err = recv(((char *) _buf), _size ));
  return err;

CHK_ERR:
  return err;
}

int UDPLIB::ready_udp(int timeout_ms)
{
  fd_set fdset;
  int fdi_n = -1;
  struct timeval ktimeout;
  int ret;
  FD_ZERO(&fdset);
  if(fdi_n < socket())
    fdi_n = socket();
  FD_SET(socket(), &fdset);
  ktimeout.tv_sec = timeout_ms / 1000;
  ktimeout.tv_usec = (timeout_ms % 1000) * 1000;
  (ret=select(fdi_n + 1, &fdset, NULL, NULL, &ktimeout));
  if(ret > 0)
    return ret;
  else if(ret == 0)
    return ret;
  else
    D(0,("select error\n"));
  return ret;
}

int UDPLIB::cunet_print(int _debug_level, const char* str, uint8_t* buf, int size)
{
  int i;
  Dcont(_debug_level, ("size=%i %s=", size, str));
  for(i = 0; i < size; i++)
    Dcont(_debug_level, ("%s%02x", (i >= 10 && ((i - 10) % 4) == 0) ? " " : "", buf[i]));
  Dcont(_debug_level, ("\n"));
  return 0;
}
