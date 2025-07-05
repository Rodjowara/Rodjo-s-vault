#define F_CPU 3333333
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PA7_LOW !(PORTA.IN & PIN7_bm)
#define PA7_INTERRUPT PORTA.INTFLAGS & PIN7_bm
#define PA7_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS |= PIN7_bm

#define PA6_LOW !(PORTA.IN & PIN6_bm)
#define PA6_INTERRUPT PORTA.INTFLAGS & PIN6_bm
#define PA6_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS |= PIN6_bm

#define PA5_LOW !(PORTA.IN & PIN5_bm)
#define PA5_INTERRUPT PORTA.INTFLAGS & PIN5_bm
#define PA5_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS |= PIN5_bm

#define PA4_LOW !(PORTA.IN & PIN4_bm)
#define PA4_INTERRUPT PORTA.INTFLAGS & PIN4_bm
#define PA4_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS |= PIN4_bm

volatile uint8_t pa7flag;
volatile uint8_t pa6flag;
volatile uint8_t pa5flag;
volatile uint8_t pa4flag;

ISR(PORTA_PORT_vect)
{
	if (PA7_INTERRUPT)
	{
		pa7flag = 1;
		PA7_CLEAR_INTERRUPT_FLAG;
	}
	
	if (PA6_INTERRUPT)
	{
		pa6flag = 1;
		PA6_CLEAR_INTERRUPT_FLAG;
	}
	
	if (PA5_INTERRUPT)
	{
		pa5flag = 1;
		PA5_CLEAR_INTERRUPT_FLAG;
	}
	
	if (PA4_INTERRUPT)
	{
		pa4flag = 1;
		PA4_CLEAR_INTERRUPT_FLAG;
	}

}

void main()
{
	PORTD.DIR |= (1 << 7); // set PD.7 as output (LED1)
	PORTD.DIR |= (1 << 6); // set PD.6 as output (LED2)
	PORTD.DIR |= (1 << 5); // set PD.5 as output (LED3)
	PORTD.DIR |= (1 << 4); // set PD.4 as output (LED4)
	
	pa7flag = 0;
	pa6flag = 0;
	pa5flag = 0;
	pa4flag = 0;
	
	PORTA.DIR &= ~ PIN7_bm; // PA.7 (KEY1) as input
	PORTA.DIR &= ~ PIN6_bm; // PA.6 (KEY1) as input
	PORTA.DIR &= ~ PIN5_bm; // PA.5 (KEY3) as input
	PORTA.DIR &= ~ PIN4_bm; // PA.4 (KEY4) as input
	
	// PA.7: internal pull-up; generate interrupt on both edges
	PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	// PA.6: internal pull-up; generate interrupt on both edges
	PORTA.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	// PA.5: internal pull-up; generate interrupt on both edges
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	// PA.4: internal pull-up; generate interrupt on both edges
	PORTA.PIN4CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	
	sei(); // global enable interrupts
	while(1) {
		if (pa7flag) {
			if(PA7_LOW) {
				PORTD.OUT |= (1 << 7); // LED1 ON
				PORTD.OUT &= ~(1 << 6); // LED2 OFF
				PORTD.OUT &= ~(1 << 5); // LED3 OFF
				PORTD.OUT &= ~(1 << 4); // LED4 OFF
			}
			
			pa7flag = 0;
		}
		
		if (pa6flag) {
			if(PA6_LOW) {
				PORTD.OUT |= (1 << 6); // LED2 ON
				PORTD.OUT &= ~(1 << 7); // LED1 OFF
				PORTD.OUT &= ~(1 << 5); // LED3 OFF
				PORTD.OUT &= ~(1 << 4); // LED4 OFF
			}
			
			pa6flag = 0;
		}
		
		if (pa5flag) {
			if(PA5_LOW) {
				PORTD.OUT |= (1 << 5); // LED3 ON
				PORTD.OUT &= ~(1 << 6); // LED2 OFF
				PORTD.OUT &= ~(1 << 7); // LED1 OFF
				PORTD.OUT &= ~(1 << 4); // LED4 OFF
			}
			
			pa5flag = 0;
		}
		
		if (pa4flag) {
			if(PA4_LOW) {
				PORTD.OUT |= (1 << 4); // LED4 ON
				PORTD.OUT &= ~(1 << 6); // LED2 OFF
				PORTD.OUT &= ~(1 << 5); // LED3 OFF	
				PORTD.OUT &= ~(1 << 7); // LED1 OFF
			}
			
			pa4flag = 0;
		}
		
		_delay_ms(10);
	}
}