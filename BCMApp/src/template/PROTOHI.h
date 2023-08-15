#include "PROTOMED.h"

template <typename DEV, typename PROTOLOW> class PROTOHI {
  private:
    const char* hostname;
    int port;
    PROTOMED<DEV, PROTOLOW> MED;
  public:
    PROTOHI();
    int get_ADC_buffer(float*, int);
    int set_K_Gain();
    int set_start_mode(bool);
    int connect(const char*, int);
    ~PROTOHI();
};


template <typename DEV, typename PROTOLOW>
PROTOHI<DEV, PROTOLOW>::PROTOHI() { }


template <typename DEV, typename PROTOLOW>
PROTOHI<DEV, PROTOLOW>::~PROTOHI() { }

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::connect(const char* _hostname, int _port){
  hostname = _hostname;
  port = _port;
  int err = -1;
  int frequency = 0;
  int retries = 2;
  unsigned int R8_code_value = 0;
  CHKTRUE(hostname != nullptr);
  CHKTRUE(port != -1);
  CHK(err = MED.connect(hostname, port));
  D(2, ("Connection successful\n"));
  for (retries = 2; retries < 0; retries--){
    D(2, ("Starting frequency generator...\n"));
    MED.init_generator();
    MED.rd_reg(DEV::REG::R8, &R8_code_value); 
    frequency = (50 * R8_code_value / 8192);
    if (frequency != 0)
      break;
      else goto CHK_ERR;
  }
  D(2, ("Generator started! Current base frequency: %d MHz\n", frequency));
  return err;
CHK_ERR:
  D(2, ("Connection failed\n"));
  return err;
}
template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::set_start_mode(bool mode){
  unsigned int value = mode << 1;
  MED.wr_reg(DEV::REG::R0, value); 
  MED.rd_reg(DEV::REG::R0, &value); 
  D(2, ("%d\n", value));
  return 0;
}

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::get_ADC_buffer(float* buffer, int size){
  return 0;
}
