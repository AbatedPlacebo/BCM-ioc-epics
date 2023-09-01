#include "PROTOBCM.h"

template <typename DEV, template<typename> typename PROTOCOL> class PROTOHI {
  private:
    const char* hostname;
    int port;
    PROTOBCM<DEV> connection;
  public:
    PROTOHI();
    int get_ADC_buffer(float*, int);
    int set_K_gain(unsigned int);
    int set_start_mode(bool);
    int connect(const char*, int);
    int disconnect();
    int start_generator();
    ~PROTOHI();
};

template <typename DEV, template<typename> typename PROTOCOL>
PROTOHI<DEV, PROTOCOL>::PROTOHI() { }

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::start_generator() 
{ 
  int err = -1;
  int frequency = 0;
  int retries;
  unsigned int R8_code_value = 0;
  for (retries = 1; retries > 0; retries--)
  {
    D(2, ("Starting frequency generator...\n"));
    CHK(err = connection.init_generator());
    CHK(err = connection.rd_reg(DEV::REG::R8, &R8_code_value));
    frequency = (50 * R8_code_value / 8192);
    D(2, ("%d\n", frequency));
    if (frequency == 0)
      continue;
  }
  D(2, ("Generator has been started! Current base frequency: %d MHz\n", frequency));
  return err;
CHK_ERR:
  D(2, ("Initialisation of the generator has been failed\n"));
  return err;
}

template <typename DEV, template<typename> typename PROTOCOL>
PROTOHI<DEV, PROTOCOL>::~PROTOHI(){
  disconnect();
}

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::connect(const char* _hostname, int _port)
{
  hostname = _hostname;
  port = _port;
  int err = -1;
  int retries = 2;
  unsigned int frequency = 0;
  unsigned int R8_code_value = 0;
  CHKTRUE(hostname != nullptr);
  CHKTRUE(port != -1);
  CHK(err = connection.connect(hostname, port));
  D(2, ("Connection successful\n"));
  for (retries = 2; retries > 0; retries--)
  {
    CHK(err = connection.rd_reg(DEV::REG::R8, &R8_code_value));
    frequency = (50 * R8_code_value / 8192);
    D(2, ("Register 8 frequency is: %d\n", frequency));
    if (frequency < 158 || frequency > 161) 
    {
      CHK(err = start_generator());
      break;
    }
  }
  return err;
CHK_ERR:
  D(2, ("Connection failed\n"));
  return err;
}

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::disconnect()
{
  int err = -1;
  CHK(err = connection.disconnect());
  return err;
CHK_ERR:
  D(2, ("Disconnection failed\n"));
  return err;
}


template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::set_start_mode(bool mode){
  int err = -1;
  unsigned int value = mode << 1;
  CHK(err = connection.wrrd_reg(DEV::REG::R0, &value)); 
  D(2, ("Mode start is set to: %d\n", value));
  return err;
CHK_ERR:
  return err;
}

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::get_ADC_buffer(float* buffer, int size){
  return 0;
}

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::set_K_gain(unsigned int value)
{
  int err = -1;
  unsigned int checking_value;
  CHK(err = connection.wrrd_reg(DEV::REG::R2, &value));
  D(2, ("K_gain is set to: %d\n", value));

CHK_ERR:
  return err;
}

