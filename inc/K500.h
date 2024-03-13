#ifndef K500_H
#define K500_H
// Constants for K500

#define OSCSIZE 65536

#define SEND_MESSAGE_SIZE 6

#define MIN_PAGE 0
#define PAGE_POINTS_START 10

#define MAX_POINT_VALUE 2048
#define MIN_POINT_VALUE -2048

#define ACK_MESSAGE_SIZE 4

#define MAX_RECV_MESSAGE_SIZE 1034

#define MAX_BYTE_BUFFER MAX_RECV_MESSAGE_SIZE * (MAX_PAGE + 1)

#define TOTAL_PAGE_POINTS 512

#define MAX_PAGE 127

#define ACK_PACKET 0x10

#define ACK_LENGTH 4

#define ADC_LENGTH 1034

#define CONF_LENGTH 2

#define CONF_PACKET 0x11

#define MAX_POINTS TOTAL_PAGE_POINTS * (MAX_PAGE + 1)

#define MAX_OSC_TIME 320

#define WAVEFORM_LENGTH_TIME (double)MAX_OSC_TIME / (double)MAX_POINTS

#endif
