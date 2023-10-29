/*
 * IncFile1.h
 *
 * Created: 10/10/2023 10:45:19 AM
 *  Author: Inovace - Sozib
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_
#include "port.h"
#include "stdint.h"


void led2Int(uint8_t pin);
void gpioWrite(uint8_t pin, uint8_t state);

#endif /* INCFILE1_H_ */