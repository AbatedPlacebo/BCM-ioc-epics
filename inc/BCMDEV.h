#ifndef BCMDEV_H
#define BCMDEV_H
#define OSCSIZE 65536

#define SEND_MESSAGE_SIZE 6

#define MIN_PAGE 0
#define PAGE_POINTS_START 10

#define MAX_POINT_VALUE 2048
#define MIN_POINT_VALUE -2048

#define ACK_MESSAGE_SIZE 4

#define MAX_RECV_MESSAGE_SIZE 1034

#define MAX_BYTE_BUFFER MAX_RECV_MESSAGE_SIZE * (MAX_PAGE + 1)


#include <stdint.h>

typedef struct BCMDEV {
  enum CONST {
    TOTAL_PAGE_POINTS = 512,
    MAX_PAGE = 127,
    ACK_PACKET = 0x10,
    ACK_LENGTH = 4,
    ADC_LENGTH = 1034,
    CONF_LENGTH = 2,
    CONF_PACKET = 0x11,
    MAX_POINTS = TOTAL_PAGE_POINTS * (MAX_PAGE + 1),
    MAX_OSC_TIME = 320,
  };
  enum CMD
  {
    CMD_WRREG     = 0x00,
    CMD_START     = 0x03,
    CMD_RDREG     = 0x04,
    CMD_STOP      = 0x05,
    CMD_STARTGEN  = 0x06,
    CMD_RESETCNT  = 0x07,
    CMD_RDADC     = 0x08,
    CMD_FLASHWR   = 0x09,
    CMD_IPREWR    = 0x0a,
    CMD_WRRDREG   = 0x0c,
    CMD_FLASHRD   = 0x0f,
  };
  enum REG{
    STATUS,
    REG_MODE = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R8 = 8,
    REG_IPWR = 9,
    REG_WRIPHI = 14,
    REG_WRIPLO = 15,
    R16,
    R17,
    R18,
    R19,
    R20,
    R21,
    REG_BUFIPHI = 22,
    REG_BUFIPLO = 23,
    R24,
    R25,
    R26,
    R27,
    R28,
    R29,
    R30,
  };
  static const int REG_SIZE = 32;
  typedef uint32_t REGHW_t;
  typedef uint32_t REG_CACHE_MASK_t;
  static const REG_CACHE_MASK_t REG_CACHE_MASK =
    (1 << REG::REG_MODE) | (1 << REG::R1) |
    (1 << REG::R2) | (1 << REG::R3);
} BCMDEV;

#endif
