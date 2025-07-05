#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 3333333

#define SEG_A (1 << 1)
#define SEG_B (1 << 0)
#define SEG_C (1 << 4)
#define SEG_D (1 << 7)
#define SEG_E (1 << 5)
#define SEG_F (1 << 2)
#define SEG_G (1 << 3)
#define SEG_DP (1 << 6)
const unsigned char segment_codes[10] = {
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,// 0
	SEG_B | SEG_C, // 1
	SEG_A | SEG_B | SEG_G | SEG_E | SEG_D, // 2
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_G, // 3
	SEG_F | SEG_G | SEG_B | SEG_C, // 4
	SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // 5
	SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E,// 6
	SEG_A | SEG_B | SEG_C, // 7
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G}; // 9
void display_num(uint8_t num){
	uint8_t prvi = num / 10;
	uint8_t drugi = num % 10;
	
	int i = 0;
	while(i <= 200){
		// Prikaz desetica (SEL_DISP_1)
		PORTE.OUT |= (1 << 2);    // Uklju?i SEL_DISP_1 (desetice)
		PORTE.OUT &= ~(1 << 3);   // Isklju?i SEL_DISP_2 (jedinice)
		PORTC.OUT = segment_codes[prvi];  // Prikaz desetice
		_delay_ms(5);

		// Prikaz jedinica (SEL_DISP_2)
		PORTE.OUT |= (1 << 3);    // Uklju?i SEL_DISP_2 (jedinice)
		PORTE.OUT &= ~(1 << 2);   // Isklju?i SEL_DISP_1 (desetice)
		PORTC.OUT = segment_codes[drugi];  // Prikaz jedinice
		_delay_ms(5);
		
		i++;
	}
}

void main(){
    PORTC.DIR = 0xFF;
	PORTE.DIRSET = (1 << 2) | (1 << 3);
	
    while (1) {
		for(int i = 0; i <= 99; i++){
			display_num(i);
		}
    }
}

