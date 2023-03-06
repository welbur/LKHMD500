/*
01111110 00000000 11111111 00000000 00000000 00000000 ... 00000000 10000001
|      | |      | |      | |      | |      | |      | | | |      | |______|__Stop byte
|      | |      | |      | |      | |      | |      | | | |______|___________8-bit CRC
|      | |      | |      | |      | |      | |      | |_|____________________Rest of payload
|      | |      | |      | |      | |      | |______|________________________2nd payload byte
|      | |      | |      | |      | |______|_________________________________1st payload byte
|      | |      | |      | |______|__________________________________________# of payload bytes
|      | |      | |______|___________________________________________________COBS Overhead byte
|      | |______|____________________________________________________________Packet ID (0 by default)
|______|_____________________________________________________________________Start byte (constant)
*/
#ifndef _PACKET_C_H_
#define	_PACKET_C_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>

#define CONTINUE_C              3                       //const int8_t CONTINUE_C           = 3;
#define NEW_DATA_C              2                       //const int8_t NEW_DATA_C           = 2;
#define NO_DATA_C               1                       //const int8_t NO_DATA_C            = 1;
#define CRC_ERROR_C             0                       //const int8_t CRC_ERROR_C          = 0;
#define PAYLOAD_ERROR_C         -1                      //const int8_t PAYLOAD_ERROR_C      = -1;
#define STOP_BYTE_ERROR_C       -2                      //const int8_t STOP_BYTE_ERROR_C    = -2;
#define STALE_PACKET_ERROR_C    -3                      //const int8_t STALE_PACKET_ERROR_C = -3;


//extern uint16_t txObj(const T& val, const uint16_t& index = 0, const uint16_t& len = sizeof(T));
extern void *Packet_C_New(void);
extern uint8_t Packet_C_parse(void *packet, uint8_t recChar, int valid);
extern int8_t Packet_C_getPacketStatus(void *packet);
extern void Packet_C_reset(void *packet);

#ifdef __cplusplus
}
#endif

#endif
