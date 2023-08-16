#include "PROTOMED.h"


template <typename DEV, typename PROTOLOW> class PROTOHI {
  private:
    const char* hostname;
    int port;
    PROTOMED<DEV, PROTOLOW> MED;
  public:
    PROTOHI();
    int get_ADC_buffer(float*, int);
    int set_K_gain(unsigned int);
    int set_start_mode(bool);
    int connect(const char*, int);
    int start_generator();
    ~PROTOHI();
};


template <typename DEV, typename PROTOLOW>
PROTOHI<DEV, PROTOLOW>::PROTOHI() { }

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::start_generator() 
{ 
  int err = -1;
  int frequency = 0;
  int retries = 2;
  unsigned int R8_code_value = 0;
  for (retries = 1; retries > 0; retries--){
    D(2, ("Starting frequency generator...\n"));
    CHK(err = MED.init_generator());
    CHK(err = MED.rd_reg(DEV::REG::R8, &R8_code_value));
    frequency = (50 * R8_code_value / 8192);
    D(2, ("%d\n", frequency));
    if (frequency == 0)
      retries++;
  }
  D(2, ("Generator has been started! Current base frequency: %d MHz\n", frequency));
  return err;
CHK_ERR:
  D(2, ("Initialisation of the generator has been failed\n"));
  return err;
}


template <typename DEV, typename PROTOLOW>
PROTOHI<DEV, PROTOLOW>::~PROTOHI(){
  if (MED.is_connected() == 1)
    MED.disconnect();
}



template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::connect(const char* _hostname, int _port){
  hostname = _hostname;
  port = _port;
  int err = -1;
  int retries = 2;
  CHKTRUE(hostname != nullptr);
  CHKTRUE(port != -1);
  CHK(err = MED.connect(hostname, port));
  D(2, ("Connection successful\n"));
  return err;
CHK_ERR:
  D(2, ("Connection failed\n"));
  return err;
}
template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::set_start_mode(bool mode){
  int err = -1;
  unsigned int value = mode << 1;
  CHK(err = MED.wr_reg(DEV::REG::R0, value)); 
  CHK(err = MED.rd_reg(DEV::REG::R0, &value)); 
  D(2, ("Mode start is set to: %d\n", value));
  return err;
CHK_ERR:
  return err;
}

template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::get_ADC_buffer(float* buffer, int size){
  return 0;
}


  template <typename DEV, typename PROTOLOW>
int PROTOHI<DEV, PROTOLOW>::set_K_gain(unsigned int value)
{
  int err = -1;
  CHK(err = MED.wr_reg(DEV::REG::R2, value));
  unsigned int checking_value;
  CHK(err = MED.rd_reg(DEV::REG::R2, &value));
  D(2, ("K_gain is set to: %d\n", checking_value));

CHK_ERR:
  return err;
}

