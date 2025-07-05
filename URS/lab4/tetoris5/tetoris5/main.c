#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 3333333
#define BAUD_TWI 100000

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
	while(i <= 100){
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

void I2C_Init() { // init TWI interface
	TWI0.MBAUD = (uint8_t)((F_CPU / 2 / BAUD_TWI) - 5); // 100 kbps
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; // force bus into idle state
	TWI0.MCTRLA = TWI_ENABLE_bm; // enable TWI
}

void I2C_Start(uint8_t address, uint8_t rw) {// start transmission by sending address; rw=0 write, =1 read
	TWI0.MADDR = address << 1 | rw;
	while(!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm)));
	if(rw == 0){
		TWI0.MSTATUS |= TWI_WIF_bm;
	}else{
		TWI0.MSTATUS |= TWI_RIF_bm;
	}
	while(TWI0.MSTATUS & TWI_RXACK_bm);
}

uint8_t I2C_Write(uint8_t data) { // write data
	TWI0.MDATA = data;// send data
	while (!(TWI0.MSTATUS & (TWI_WIF_bm)));// Wait for write interrupt flag
	TWI0.MSTATUS |= TWI_WIF_bm;
	TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;// Do nothing on bus (no ACK etc.)
	return !(TWI0.MSTATUS & TWI_RXACK_bm); // Returns true (!0) if slave gave an ACK
}

void I2C_Stop(void) { // end transmission
	TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc; // Send ACK / STOP
}

uint8_t I2C_read(uint8_t last_byte) {
	while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm)); // Wait for Clk Hold flag to be high
	if( last_byte ) {
		TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc; // send NACK & stop cond
	}
	else {
		TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc; // send ACK
	}
	return TWI0.MDATA;
}

uint16_t read_half(uint8_t addr, uint8_t reg){
	uint8_t high;
	uint8_t low;
	
	I2C_Start(addr, 0);
	
	if(I2C_Write(reg) == 0){
		I2C_Stop();
		return 0xFFFF;
	}
		
	I2C_Start(addr, 1);
	
	high = I2C_read(0);
	low = I2C_read(1);
	
	I2C_Stop();
	
	return (high << 8) | low;

}

uint8_t convert(uint16_t raw){
	uint8_t high = raw >> 8;
	uint8_t low = raw & 0xFF;
	
	uint8_t temp = high;
	if(low & 0x80) temp += 1;
	
	return temp;
}

void main(){
	
	PORTC.DIR = 0xFF;
	PORTE.DIRSET = (1 << 2) | (1 << 3);
	PORTA.DIR = (1 << 2) | (1 << 3);
	PORTA.DIRSET = (1 << 2) | (1 << 3);
	PORTA.PIN2CTRL = 1 << 3;
	PORTA.PIN3CTRL = 1 << 3; 
	display_num(24);
	
	I2C_Init();
	
    while (1) {
		uint16_t result;
		result = read_half(0x48, 0x01);
		if(result == 0xFFFF) continue;
		uint8_t converted = convert(result);
		display_num(converted);
    }
}

