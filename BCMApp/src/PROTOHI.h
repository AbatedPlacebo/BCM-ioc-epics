#ifndef PROTOHI_H
#define PROTOHI_H

#include "chk.h"

extern int debug_level;

template <typename DEV, template<typename> typename PROTOCOL> class PROTOHI {
  public:
    using INFO = DEV;
    PROTOHI();
    int get_ADC_buffer(double* buffer, int size);
    int read_register(unsigned int regn,
        unsigned int *param);
    int write_register(unsigned int regn,
        unsigned int param);
    int execute_command(unsigned int com);
    int set_K_gain(unsigned int value);
    int set_start_mode(bool mode);
    int connect(const char* _hostname, int _port);
    int disconnect();
    int start_generator();
    int start_measurement();
    int is_connected() const;
    template<typename CFG> int config(CFG& cfg);
    int print_all_regs();
    ~PROTOHI();
  private:
    PROTOCOL<DEV> connection;
    const char* hostname;
    int port;
    struct encode {
      uint32_t remote_start(int v){ return v ? 0b10 : 0b00; }
      uint32_t k_gain(int v){ return v ? v & 0b11000 : 0b00000; }
      uint32_t ndel0(int v){ return v ? v & 0x10000 : 0x00000; }
    } encode;
    struct decode {
      int remote_start(uint32_t r){ return (r & 0b10) == 0b10; }
      float k_gain(uint32_t r){ return r; }
      int n_del0(uint32_t r){ return r; }
    } decode;
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
int PROTOHI<DEV, PROTOCOL>::is_connected() const{
  return connection.is_connected();
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
int PROTOHI<DEV, PROTOCOL>::get_ADC_buffer(double* buffer, int size){
  int err = -1;
  int arr[size];
  int pages_total = (int)((size - 1)/512) + 1;
  CHKTRUE(size > 0 && size <= 65536);
  CHK(err = connection.rd_ADC(arr, size, 0, pages_total));
  for (int i = 0; i < size; i++){
    buffer[i] = (double)arr[i] - 2048;
  }
  return err;
CHK_ERR:
  return err;
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


template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::start_measurement()
{
  int err = -1;
  unsigned int checking_value;
  CHK(err = connection.start());
  D(2, ("Measurement started!\n"));
CHK_ERR:
  return err;
}


template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::read_register(unsigned int regn,
    unsigned int *param)
{
  int err = -1;
  CHK(err = connection.rd_reg(regn, param));
  D(2, ("Register has been read!"));
CHK_ERR:
  return err;
}


template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::write_register(unsigned int regn,
    unsigned int param)
{
  int err = -1;
  CHK(err = connection.wr_reg(regn, param));
  D(2, ("Register has been overwritten!"));
CHK_ERR:
  return err;
}

template <typename DEV, template<typename> typename PROTOCOL>
template <typename CFG>
int PROTOHI<DEV, PROTOCOL>::config(CFG& cfg)
{
  int err = -1;
  unsigned int mode = encode.remote_start(cfg.remote_start);
  unsigned int k_gain = encode.k_gain(cfg.k_gain);
  unsigned int ndel0 = encode.ndel0(cfg.ndel0);
  D(0,("mode = %d\n",mode));
  CHK(err = connection.wr_reg(DEV::REG::R0, mode));
  CHK(err = connection.wr_reg(DEV::REG::R1, ndel0));
  CHK(err = connection.wr_reg(DEV::REG::R2, k_gain));
  return 0;
CHK_ERR:
  return err < 0 ? err : -1;
}

template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::print_all_regs(){
  int err = -1;
  uint32_t r;
  typename DEV::REG_t regs[DEV::REG_SIZE];
  for (int i = 0; i < DEV::REG_SIZE; ++i) {
    CHK(err = connection.rd_reg(i, &r));
    regs[i] = r;
    switch(i)
    {
      case DEV::REG::R0:
        D(0, ("reg %i = 0x%04x(%i) start_mode=%s \n", i, r, r,
              decode.start_mode(r) ? "int" : "ext"));
        break;
      default:
        D(0, ("reg %i = 0x%04x(%i)\n", i, r, r));
    }
  }
CHK_ERR:
  return err;
}


template <typename DEV, template<typename> typename PROTOCOL>
int PROTOHI<DEV, PROTOCOL>::execute_command(unsigned int com)
{
  int err = -1;
  CHK(err = connection.exec_com(com));
  D(2, ("Command has been executed!"));
CHK_ERR:
  return err;
}

#endif
