#ifndef _PACKET_C_H_
#define	_PACKET_C_H_

#ifdef __cplusplus
 extern "C" {
#endif

//extern uint16_t txObj(const T& val, const uint16_t& index = 0, const uint16_t& len = sizeof(T));
extern void *Packet_C_New(bool& _debug, uint32_t& _timeout);
extern uint8_t Packet_C_parse(const uint8_t& recChar, const bool& valid);

#ifdef __cplusplus
}
#endif

#endif
