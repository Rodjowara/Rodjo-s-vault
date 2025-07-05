#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 3333333


void main(){
	
	uint16_t period;
	
	PORTF.DIR |= 1 << 5;
	TCA0.SPLIT.CTRLD |= TCA_SPLIT_SPLITM_bm;
	TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP2EN_bm;
	
	period = (uint32_t)(F_CPU/(64*1000UL) - 1);
	
	TCA0.SPLIT.HPER = period;
	TCA0.SPLIT.HCMP2 = (uint8_t) 0.75 * (period + 1);
	
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	TCA0.SPLIT.CTRLA |= TCA_SPLIT_CLKSEL_DIV64_gc | TCA_SPLIT_ENABLE_bm;
	
    while (1) 
    {
    }
}

