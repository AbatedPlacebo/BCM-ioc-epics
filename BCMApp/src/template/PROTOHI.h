#include "PROTOMED.h"

template <typename DEV, typename PROTOLOW> class PROTOHI {
  private:
    PROTOMED<DEV, PROTOLOW> MED;
  public:
    PROTOHI(const char*, int);
    int get_ADC_buffer(float*, int);
    int set_K_Gain();
    int set_start_mode(bool);
};


template <typename DEV, typename PROTOLOW>
PROTOHI<DEV, PROTOLOW>::PROTOHI(const char *peer, int port) {
  MED.connect(peer, port);
}

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::set_start_mode(bool mode){
  unsigned int value = mode << 1;
  MED.wr_reg(DEV::REG::R0, value); 
  return 0;
}

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::get_ADC_buffer(float* buffer, int size){
  return 0;
}
