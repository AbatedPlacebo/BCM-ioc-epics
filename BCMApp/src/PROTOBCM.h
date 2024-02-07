#ifndef PROTOBCM_H
#define PROTOBCM_H

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
#include <vector>
#include <map>
#include <algorithm>
#include "chk.h"
#include "chk_dt.h"

std::vector<int> requestLostPackets(const std::map<int, std::vector<int>>& receivedPackets, int startRange, int endRange) {
  std::vector<int> lostPackets;
  for (int i = startRange; i <= endRange; ++i) {
    if (receivedPackets.find(i) == receivedPackets.end()) {
      lostPackets.push_back(i);
    }
  }
  return lostPackets;
}

template <typename DEV>
class PROTOBCM {
  private:
    int sock;
    struct sockaddr_in serv_addr;
    // low level
    int send(void *buf, int buf_size);
    int recv(void *buf, int buf_size);
    // middle level
    int socket();
    int send_com(int instr, int nreg, int param1, int param2 = 0);
    int ack_to(int cmd, int regn,
        int to_ms = 10, int count = 1, int repeat = 1);
    int recv_to(void *_buf, int _size, int _to_ms,
        const int unet_flag = 0);
    int ready_udp(int timeout_ms);
    static int cunet_print(int _debug_level,
        const char* str, uint8_t* buf, int size);
    int close();
  public:
    PROTOBCM();
    ~PROTOBCM();
    int connect(const char *peer, int port);
    int disconnect();
    int wr_reg(unsigned int regn, unsigned int param);
    int rd_reg(unsigned int regn, unsigned int *param);
    int start();
    int stop();
    int rd_ADC(int* arr, int size, unsigned int start_page,
        unsigned int end_page);
    int init_generator();
    int reset_measurement_cnt();
    int is_connected() const;
    int wrrd_reg(unsigned int regn, unsigned int *param);
    int exec_com(unsigned int regn);
};

#include "chk.h"
extern int debug_level;

#define CUNET_PRINT(_debug_level, str, buf, size) \
  do{ \
    D(_debug_level,(" ")); \
    PROTOBCM<DEV>::cunet_print(_debug_level, str,buf,size); \
  }while(0)


template <typename DEV>
PROTOBCM<DEV>::PROTOBCM():sock(-1)
{
}

template <typename DEV>
PROTOBCM<DEV>::~PROTOBCM()
{
  close();
}

template <typename DEV>
int PROTOBCM<DEV>::close()
{
  if (sock >= 0) {
    shutdown(sock, SHUT_RDWR);
    ::close(sock);
    sock = -1;
  }
  return 0;
}

template <typename DEV>
int PROTOBCM<DEV>::disconnect()
{
  if (is_connected())
    close();
  return 0;
}

template <typename DEV>
int PROTOBCM<DEV>::is_connected() const
{
  return (sock >= 0) ? 1 : 0;
}

template <typename DEV>
int PROTOBCM<DEV>::socket()
{
  return sock;
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

template <typename DEV>
int PROTOBCM<DEV>::connect(const char *peer, int port)
{
  struct hostent *host;

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

template <typename DEV>
int PROTOBCM<DEV>::send(void *buf, int buf_size)
{
  int err = -1;
  CHKTRUE(sock >= 0);
  Dif(4){ D(0,("send "));
    print_buf((uint8_t*)buf, buf_size); }
  CHK((err = (::sendto(sock, buf, buf_size, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))));
  return err;

CHK_ERR:
  close();
  return err;
}

template <typename DEV>
int PROTOBCM<DEV>::recv(void *buf, int buf_size)
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

template <typename DEV>
int PROTOBCM<DEV>::send_com(int instr, int nreg, int param1, int param2)
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

template <typename DEV>
int PROTOBCM<DEV>::ack_to(int cmd, int regn, int to_ms, int count, int repeat)
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
      D(3,("repeat %i PROTOBCM<>::write_reg\n", rep));
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

template <typename DEV>
int PROTOBCM<DEV>::recv_to(void *_buf, int _size,
    int _to_ms, const int unet_flag)
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

template <typename DEV>
int PROTOBCM<DEV>::ready_udp(int timeout_ms)
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

template <typename DEV>
int PROTOBCM<DEV>::cunet_print(int _debug_level, const char* str, uint8_t* buf, int size)
{
  int i;
  Dcont(_debug_level, ("size=%i %s=", size, str));
  for(i = 0; i < size; i++)
    Dcont(_debug_level, ("%s%02x", (i >= 10 && ((i - 10) % 4) == 0) ? " " : "", buf[i]));
  Dcont(_debug_level, ("\n"));
  return 0;
}

template <typename DEV>
int PROTOBCM<DEV>::wr_reg(unsigned int regn, unsigned int param){
  int err = -1;
  uint8_t ack[DEV::CONSTANTS::ACK_LENGTH];
  int cnt;
  int rep = 1;
REP:
  D(3,("write_reg %i %i(%04x)\n", regn, param, param));
  CHK(err = send_com(DEV::CMD::CMD_WRREG, regn, param)); 
  for (cnt = 1; cnt > 0; --cnt) { 
    CHK(err = recv_to(ack, sizeof(ack), 50/*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i PROTOBCM<>::write_reg\n", rep));
      --rep;
      goto REP;
    }
    ++cnt;
    continue;
  }
  CHKTRUEMESG(err == sizeof(ack),("err=%i\n", err));
  CHKTRUE(ack[0] == DEV::CONSTANTS::ACK_PACKET);
  CHKTRUE(ack[1] == DEV::CMD::CMD_WRREG);
  CHKTRUE(ack[2] == regn);
  CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
  return err;
CHK_ERR:
  if (err > 0)
    D(3,("err=%i ack=%02x%02x%02x%02x\n", 
          err, ack[0], ack[1], ack[2], ack[3]));
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::rd_reg(unsigned int regn, unsigned int *param) {
  int err = -1;
  uint8_t ack[DEV::CONSTANTS::ACK_LENGTH];
  int cnt;
  int rep = 5;
REP:
  D(3,("read_reg %i\n", regn));
  CHK(err = send_com(DEV::CMD::CMD_RDREG, regn, 0));
  for (cnt = 2; cnt > 0; --cnt) 
  {
    CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
    if (err == 0 && rep > 0) 
    {
      D(3,("repeat %i PROTOBCM<>::read_reg %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    if (err == 2 && ack[0] == 0x11) 
    {
      ++cnt;
      continue;
    }
    CHKTRUE(err == sizeof(ack));
    CHKTRUE(ack[0] == 0xF4 || ack[0] == 0x10);
    if (ack[0] == 0x10) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x10                     || (pack = 0));
      WARNTRUE(ack[1] == DEV::CMD_RDREG           || (pack = 0));
      WARNTRUE(ack[2] == regn                     || (pack = 0));
      WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n", 
              err, ack[0], ack[1], ack[2], ack[3]));
    }
    else if (ack[0] == 0xf4) {
      int pack = 1;
      WARNTRUE(ack[0] == 0xF4 || (pack = 0));
      WARNTRUE(ack[1] == regn || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n", 
              err, ack[0], ack[1], ack[2], ack[3]));
      *param = (((unsigned int) ack[2]) << 8) | ack[3];
      D(3,("val %i(%04x)\n", *param, *param));
    }
  }
  return err;

CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::stop() {
  int err = -1;
  uint8_t ack[4];
  int cnt;
  int rep = 1;
REP:
  CHK(err = send_com(DEV::CMD_STOP, 0, 0));
  for (cnt = 1; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    if (err == 2 && ack[0] == 0x11) {
      ++cnt;
      continue;
    }
    CHKTRUE(err == sizeof(ack));
    CHKTRUE(ack[0] == 0x10);
    CHKTRUE(ack[1] == DEV::CMD_STOP);
    CHKTRUE(ack[2] == 0);
    CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::start() {
  int err = -1;
  int rep = 2;
  int cnt;
  uint8_t ack[DEV::CONSTANTS::ACK_LENGTH];
REP:
  D(3,("Start measurement...\n"));
  CHK(err = send_com(DEV::CMD_START, 0, 0));
  for (cnt = 2; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 10000/*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i PROTOBCM<>::start %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    CHKTRUE(err == DEV::CONSTANTS::ACK_LENGTH ||
        err == DEV::CONSTANTS::CONF_LENGTH);
    CHKTRUE(ack[0] == 0x10 || ack[0] == 0x11);
    CHKTRUE(ack[1] == DEV::CMD_START);
    if (ack[0] == 0x10) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x10                     || (pack = 0));
      WARNTRUE(ack[1] == DEV::CMD_START           || (pack = 0));
      WARNTRUE(ack[2] == 0x00                     || (pack = 0));
      WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n", 
              err, ack[0], ack[1], ack[2], ack[3]));
    }
    else if (ack[0] == 0x11) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x11 || (pack = 0));
      WARNTRUE(ack[1] == 0x03 || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x\n", 
              err, ack[0], ack[1]));
    }
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::rd_ADC(int* arr, int size, unsigned int start_page, unsigned int end_page){
  int err = -1;
  uint8_t ack[1034];
  int cnt;
  int cnt_adc;
  int first_page = start_page;
  int second_page = end_page - 1;
  int rep = 2;
  int cur_page;
  int arr_size;
  int* ptr;
  std::map<int, std::vector<int>> receivedPacketsMap;
REP:
  D(3,("read_ADC %i %i\n", first_page, second_page));
  CHK(err = send_com(DEV::CMD_RDADC, 0, first_page, second_page));
  cnt_adc = second_page - first_page + 1;
  D(4,("cnt_adc: %d\n", cnt_adc));
  for (cnt = 1 + cnt_adc ; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 3000/*, 0*/));
    if (err == 0 && rep > 0) {
      std::vector<int> lostPackets = requestLostPackets(receivedPacketsMap, first_page, second_page);
      first_page = *std::min_element(std::begin(lostPackets), std::end(lostPackets));
      second_page = *std::max_element(std::begin(lostPackets), std::end(lostPackets));
      D(3,("repeat %i PROTOBCM<>::read_ADC %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    CHKTRUE(err == DEV::CONSTANTS::ACK_LENGTH ||
        err == DEV::CONSTANTS::ADC_LENGTH ||
        err == DEV::CONSTANTS::CONF_LENGTH);
    CHKTRUE(ack[0] == 0xF1 ||
        ack[0] == 0x10 ||
        ack[0] == 0x11
        );
    if (ack[0] == 0x10) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x10                     || (pack = 0));
      WARNTRUE(ack[1] == DEV::CMD_RDADC           || (pack = 0));
      WARNTRUE(ack[2] == 0x00                     || (pack = 0));
      WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n",
              err, ack[0], ack[1], ack[2], ack[3]));
    }
    else if (ack[0] == 0x11) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x11 || (pack = 0));
      WARNTRUE(ack[1] == 0x03 || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x\n",
              err, ack[0], ack[1]));
    }
    else if (ack[0] == 0xf1) {
      cur_page = (ack[3] << 8) | (ack[4] & 0xFF);
      D(3, ("got adc package! page #%d\n", cur_page));
      int pack = 1;
      WARNTRUE(ack[0] == 0xF1                     || (pack = 0));
      WARNTRUE(ack[1] == 0x08                     || (pack = 0));
      if (pack == 0)
      D(3,("err=%i ack=%02x%02x%02x%02x\n",
            err, ack[0], ack[1], ack[2], ack[3]));
      std::vector<int> data;
      for (int j = 10; j < 1034; j += 2){
        int value = (ack[j] << 8) | (ack[j+1] & 0xFF);
        data.push_back(value);
      }
      receivedPacketsMap.insert(std::pair<int, std::vector<int>>{cur_page, data});
      D(4,("val %i(%04x)\n", ack[10], ack[10]));
    }
  }
  size = 0;
  ptr = arr;
  for (auto it = receivedPacketsMap.begin(); it != receivedPacketsMap.end(); it++)
  {
    auto vec = (*it).second;
    ptr = std::copy(vec.begin(), vec.end(), ptr);
    arr_size += vec.size();
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::reset_measurement_cnt() {
  int err = -1;
  uint8_t ack[4];
  int cnt;
  int rep = 1;
REP:
  CHK(err = send_com(DEV::CMD_RESETCNT, 0, 0));
  for (cnt = 1; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    if (err == 2 && ack[0] == 0x11) {
      ++cnt;
      continue;
    }
    CHKTRUE(err == sizeof(ack));
    CHKTRUE(ack[0] == 0x10);
    CHKTRUE(ack[1] == DEV::CMD_RESETCNT);
    CHKTRUE(ack[2] == 0);
    CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::wrrd_reg(unsigned int regn, unsigned int *param) {
  int err = -1;
  uint8_t ack[DEV::CONSTANTS::ACK_LENGTH];
  int cnt;
  int rep = 1;
  unsigned int check_value;
REP:
  D(3,("write_read_reg %i\n", regn));
  CHK(err = send_com(DEV::CMD::CMD_WRRDREG, regn, *param));
  for (cnt = 2; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i PROTOBCM<>::write_read_reg %s\n", 
            rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    if (err == 2 && ack[0] == 0x11) {
      ++cnt;
      continue;
    }
    CHKTRUE(err == sizeof(ack));
    CHKTRUE(ack[0] == 0xF4 || ack[0] == 0x10);
    if (ack[0] == 0x10) {
      int pack = 1;
      WARNTRUE(ack[0] == 0x10                     || (pack = 0));
      WARNTRUE(ack[1] == DEV::CMD_WRRDREG         || (pack = 0));
      WARNTRUE(ack[2] == regn                     || (pack = 0));
      WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n", 
              err, ack[0], ack[1], ack[2], ack[3]));
    }
    else if (ack[0] == 0xf4) {
      int pack = 1;
      WARNTRUE(ack[0] == 0xF4                     || (pack = 0));
      WARNTRUE(ack[1] == regn                     || (pack = 0));
      if (pack == 0)
        D(3,("err=%i ack=%02x%02x%02x%02x\n", 
              err, ack[0], ack[1], ack[2], ack[3]));

      check_value = (((unsigned int) ack[2]) << 8) | ack[3];
      CHKTRUE(check_value == *param);
      D(3,("Written and read value: %i(%04x)\n", *param, *param));
    }
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}

template <typename DEV>
int PROTOBCM<DEV>::init_generator() {
  int err = -1;
  uint8_t ack[4];
  int cnt;
  int rep = 1;
REP:
  CHK(err = send_com(DEV::CMD_STARTGEN, 0, 0));
  for (cnt = 2; cnt > 0; --cnt) {
    CHK(err = recv_to(ack, sizeof(ack), 2000 + 10 /*, 0*/));
    if (err == 0 && rep > 0) {
      D(3,("repeat %i %s\n", rep, __FUNCTION__));
      --rep;
      goto REP;
    }
    if (err == 2 && ack[0] == 0x11) {
      CHKTRUE(ack[0] == 0x11);
      CHKTRUE(ack[1] == DEV::CMD_STARTGEN);
      break;
    }
    CHKTRUE(err == sizeof(ack));
    CHKTRUE(ack[0] == 0x10);
    CHKTRUE(ack[1] == DEV::CMD_STARTGEN);
    CHKTRUE(ack[2] == 0);
    CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
  }
  return err;
CHK_ERR:
  if (err > 0)
    CUNET_PRINT(2, "ack", ack, err);
  return -1;
}


template <typename DEV>
int PROTOBCM<DEV>::exec_com(unsigned int cmd) {
  int err = -1;
  CHK(err = send_com(cmd, 0, 0));
  CHK(err = ack_to(cmd, 0, 100, 1, 0));
CHK_ERR:
  return err;
}
#endif
