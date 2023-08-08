#ifndef CRC8_CRC16_H
#define CRC8_CRC16_H

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;

/*Unsigned*/
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;


extern uint8_t get_CRC8_check_sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);

extern uint32_t verify_CRC8_check_sum(unsigned char *pchMessage, unsigned int dwLength);

extern void append_CRC8_check_sum(unsigned char *pchMessage, unsigned int dwLength);

extern uint16_t get_CRC16_check_sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);

extern uint32_t verify_CRC16_check_sum(uint8_t *pchMessage, uint32_t dwLength);

extern void append_CRC16_check_sum(uint8_t * pchMessage,uint32_t dwLength);

#endif // CRC8_CRC16_H
