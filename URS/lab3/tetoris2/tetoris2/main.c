#define F_CPU 3333333
#define BAUD_RATE 115200
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint16_t adc_value = 0;
volatile uint8_t adc_ready = 0;

int usart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

int usart_putchar(char c, FILE *stream) {
	while (!(USART3.STATUS & USART_DREIF_bm));
	USART3.TXDATAL = c;
	return 0;
}

void init(){
	stdout = &mystdout;
	PORTB.DIR |= PIN0_bm;// PORTB.0 = '1' (output)
	USART3.BAUD = ((uint32_t)F_CPU * 64) / (16 * (uint32_t)BAUD_RATE); // 115200 bps
	USART3.CTRLB |= USART_TXEN_bm;
}

// ADC0: ulaz s PD0 (AIN18), referenca = VDD
void ADC0_init(void) {
	//VREF.ADC0REF = VREF;							 // Referenca: VDD = 3.3V
	//ADC0.CTRLC = ADC_PRESC_DIV4_gc;                  // ADC clock = F_CPU / 4
	//ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;                // Ulaz: pin1 ADC-a
	ADC0.INTCTRL = ADC_RESRDY_bm;                    // Omogu?i interrupt na zavr?etak konverzije
	ADC0.CTRLA = ADC_ENABLE_bm;                      // Omogu?i ADC
	
	ADC0.CTRLC = ADC_PRESC_DIV16_gc
	| ADC_SAMPCAP_bm
	| ADC_REFSEL_VDDREF_gc;
	
	ADC0.CTRLB = ADC_SAMPNUM_ACC1_gc;
	
	/* Select ADC channel */
	ADC0.MUXPOS  = ADC_MUXPOS_AIN0_gc;
}

// Pokreni ADC konverziju
void ADC0_start_conversion(void) {
	ADC0.COMMAND = ADC_STCONV_bm;
}

// ISR za kraj konverzije
ISR(ADC0_RESRDY_vect) {
	adc_value = ADC0.RES;
	adc_ready = 1;
}


int main(void) {
	init();
	ADC0_init();
	sei(); // Omogu?i globalne prekide

	float sum = 0.0;
	uint16_t counter = 0;

	while (1) {
		ADC0_start_conversion();

		while (!adc_ready); // ?ekaj da konverzija zavr?i
		adc_ready = 0;
		
		cli();
		float voltage = adc_value * 3.3 / 1023.0; // ADC u voltima
		sum += voltage;
		counter++;
		sei();
		
		cli();
		if (counter >= 100) { // svaka 1 sekunda (100x10ms)
			float avg_voltage = sum / counter;
			uint16_t millivolts = (uint16_t)(avg_voltage*1000);
			printf("%d\n\r", millivolts); // ispis u milivoltima
			sum = 0.0;
			counter = 0;
		}
		sei();
		
		_delay_ms(10);
	}
}

