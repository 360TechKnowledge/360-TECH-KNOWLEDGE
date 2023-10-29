/*
 * Led23.c
 *
 * Created: 10/10/2023 10:45:51 AM
 *  Author: Inovace - Sozib
 */ 
#include "led2.h"

void led2Int(uint8_t pin ){
	
	DDRD|=1<<pin;
}

void gpioWrite(uint8_t pin, uint8_t state){
	if(state)
	PORTD|=1<<pin;
	else 
	PORTD&= ~(1<<pin);
}