#include "PROTOMED.h"

template <typename DEV, typename PROTOLOW> class PROTOHI {
private:
  PROTOMED<DEV, PROTOLOW> MED;
public:
  int get_ADC_buffer();
  int set_K_Gain();
  int write_register(unsigned int, unsigned int);
};
