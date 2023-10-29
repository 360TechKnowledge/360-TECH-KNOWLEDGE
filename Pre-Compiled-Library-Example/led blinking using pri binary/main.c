#include <atmel_start.h>

#include "Gpio/led2.h"
#include "avr/delay.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	led2Int(7);
	/* Replace with your application code */
	while (1) 
	{
	gpioWrite(7,0);
	_delay_ms(500);
	gpioWrite(7,1);
	_delay_ms(500);
	}
}
