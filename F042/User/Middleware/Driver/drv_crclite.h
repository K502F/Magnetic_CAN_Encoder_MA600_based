#ifndef CRCLITE
#define CRCLITE

#include "stm32f0xx_hal.h"

class Class_crclite{
	public:
		uint16_t Get_CRC16_Check_Sum(const uint8_t * pchMessage, uint32_t dwLength, uint16_t wCRC);
    bool Verify_CRC16_Check_Sum(const uint8_t * pchMessage, uint32_t dwLength);
    void Append_CRC16_Check_Sum(uint8_t * pchMessage, uint32_t dwLength);
			
	};

#endif