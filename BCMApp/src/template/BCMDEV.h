#ifndef BCMDEV_H
#define  BCMDEV_H

#include <cstdint>

struct BCMDEV {
  enum CMD
  {
    WRITE_REGISTER,
    START_CYCLE,
    READ_REGISTER,
    STOP_CYCLE,
    READ_BUFFER,
    START_GENERATOR,
    COUNT_RESET 
  };
  enum REG{
    STATUS,
    R1,
    R2,
    R3,
    R8,
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

struct R0{
  static const unsigned int VAL = 0;
  enum {
    ENABLE = 0b10,
    DISABLE = 0b00,
  };
} R0;

};

#endif
