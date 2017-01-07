#include <avr/io.h>
#include "bit_helper.h"


#define INIT_COMPLETE_STAGE_1 PORTD SET_BIT5
#define INIT_COMPLETE_STAGE_2 PORTD SET_BIT6

#define ERROR_WARN  PORTD CLEAR_BIT6;\
					PORTD CLEAR_BIT5;\
					PORTD SET_BIT4
#define ERROR_FATAL PORTD CLEAR_BIT6;\
					PORTD CLEAR_BIT5;\
					PORTD SET_BIT7



void ledInit(void)
{
	//The AT90USBKey includes 2 bi-color LEDs (green/red) implemented on one line. 
	//They are connected to the high nibble of Port D of AT90USB (PORTD[4..7]).
	//To light on a LED, the corresponding port pin must drive a high level. 
	//To light off a LED, the corresponding port pin must drive a low level.

	//Set portD pins 4..7 to output mode (this doesn't set what the output will be)

	DDRD SET_BIT4;
	DDRD SET_BIT5;
	DDRD SET_BIT6;
	DDRD SET_BIT7;
}

void adcInit(void)
{
	//set ADC clock prescalar so that ADC clock is sysclock/64 to get 125khz
	ADCSRA |=  (1 << ADPS2) | (1 << ADPS1);
	ADCSRA &= ~(1 << ADPS0);

	//set ADC reference voltage to AVCC
	ADMUX |=  (1 << REFS0); 
	ADMUX &= ~(1 << REFS1); 

	//disable digital input on ADC pins to save power
	//(1 << ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D);
	DIDR0 SET_ALL;

		//set ADC enable
	ADCSRA |= (1 << ADEN);
}


int main(void)
{
	ledInit();
	adcInit();
	INIT_COMPLETE_STAGE_1;

	// Start conversion. Hardware will clear this bit when conversion is complete
	ADCSRA |= ( 1 << ADSC);
	
	do
	{
		asm volatile ("NOP");
	}
    while (ADCSRA & (1<<ADSC)); // Now wait for ADSC to be set (indicating conversion is finished)

	INIT_COMPLETE_STAGE_2;

	while(1);
}

