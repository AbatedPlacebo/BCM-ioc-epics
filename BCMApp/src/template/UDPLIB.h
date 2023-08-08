#ifndef UDPLIB_H
#define UDPLIB_H

#include <netinet/in.h>

class UDPLIB {
  protected:
    int sock;
    struct sockaddr_in serv_addr;
    int m_conf;
    int conf();
    int close();
  public:
    UDPLIB();
    virtual ~UDPLIB();
    // low level
    int send(void *buf, int buf_size);
    int recv(void *buf, int buf_size);
    // middle level
    int connect(const char *peer, int port);
    int disconnect();
    int is_connected() const;
    int socket();
    int send_com(int instr, int nreg, int param1, int param2 = 0);
    int ack_to(int cmd, int regn, int to_ms = 10, int count = 1, int repeat = 1);
    int recv_to(void *_buf, int _size, int _to_ms, const int unet_flag = 0);
    int ready_udp(int timeout_ms);
    static int cunet_print(int _debug_level, const char* str, uint8_t* buf, int size);

    void set_m_conf(int val);
    int get_m_conf();
};

#include"chk.h"
extern int debug_level;

#define CUNET_PRINT(_debug_level, str, buf, size) \
	do{ \
		D(_debug_level,(" ")); \
		UDPLIB::cunet_print(_debug_level, str,buf,size); \
	}while(0)

#endif
