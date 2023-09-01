#ifndef UDPLIB_H
#define UDPLIB_H

#include <netinet/in.h>

class UDPLIB {
  protected:
    int m_conf;
    int conf();
  public:
    UDPLIB();
    virtual ~UDPLIB();
};

#
