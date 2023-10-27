#ifndef BCMDEV_H
#define BCMDEV_H
#define OSCSIZE 65536

#include <stdint.h> 

typedef struct BCMDEV {
  enum CONSTANTS {
    ACK_PACKET = 0x10,
    ACK_LENGTH = 4,
    ADC_LENGTH = 1034,
    CONF_LENGTH = 2,
    CONF_PACKET = 0x11,
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
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R8 = 8,
    R9,
    R14,
    R15,
    R16,
    R17,
    R18,
    R19,
    R20,
    R21,
    R22,
    R23,
    R24,
    R25,
    R26,
    R27,
    R28,
    R29,
    R30,
  };
  static const int REG_SIZE = 32;
  typedef uint32_t REG_t;
  typedef uint32_t REG_CACHE_MASK_t;
  static const REG_CACHE_MASK_t REG_CACHE_MASK = (1 << REG::STATUS) | (1 << REG::R1);
} BCMDEV;

#endif
