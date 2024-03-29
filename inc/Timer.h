#include "epicsTime.h"

class Timer
{
  epicsTimeStamp t0;

  public:
  Timer() { epicsTimeGetCurrent(&t0); }

  Timer& operator=(double offset_sec)
  {
    epicsTimeGetCurrent(&t0);
    epicsTimeAddSeconds(&t0, offset_sec);
    return *this;
  }

  Timer& operator+=(double offset_sec)
  {
    epicsTimeAddSeconds(&t0, offset_sec);
    return *this;
  }

  double operator()()
  {
    epicsTimeStamp t1;
    epicsTimeGetCurrent(&t1);
    return epicsTimeDiffInSeconds(&t1, &t0);
  }


};

