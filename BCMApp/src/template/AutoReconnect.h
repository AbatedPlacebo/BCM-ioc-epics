#ifndef AUTORECONNECT_H
#define AUTORECONNECT_H

class AutoReconnect 
{
  private:
    int cnt;
    int timeout[6] = {10, 100, 1000, 2000, 3000, 5000};
  public:
    AutoReconnect()
    {
      cnt = 0;
    }
    int time(int err)
    {
      if (err = -1)
      {
        cnt = 0;
        return timeout[cnt];
      }
      else
        return (cnt < 5) ? timeout[cnt++] : timeout[cnt];
    }
    void reset()
    {
      cnt = 0;
    }
};

#endif
