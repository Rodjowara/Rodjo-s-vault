#include "stm32f4xx.h"

void init_CLOCK(void);
void init_FlashAccess(void);
void init_SysTick(void);
void init_USART2(void);

void init_DefaultClock(void) {
 RCC->CR |= (uint32_t)0x00000001; // HSION=1
 RCC->CFGR = 0x00000000; // Reset CFGR register
 RCC->CR &= (uint32_t)0xFEF6FFFF; // HSEON, CSSON, PLLON =0
 RCC->PLLCFGR = 0x24003010; // Reset PLLCFGR register
 RCC->CR &= (uint32_t)0xFFFBFFFF; // HSEBYP=0
 RCC->CIR = 0x00000000; // Dissable all clock interrupts
} 

void init_CLOCK(void){
	init_DefaultClock();
	//HSE OSCILLATOR (for Discovery board: Quartz 8 MHz)
	RCC->CR |= 0x1UL<<16; // HSEON=1;
	while( !(RCC->CR & (0x1UL<<17)) ); // while(HSERDY==0);
	
	RCC->PLLCFGR = 0x24401908;
	
	RCC->CR |= 0x1UL<<24; // PLLON=1;
	while( !(RCC->CR & (0x1UL<<25)) ); // while(PLLRDY==0);
	
	// Clock for HCLK, APB1 presc. and APB2 presc.
	RCC->CFGR |= 0x0UL<<4; // HPRE: divide by 1
	
	// Maximmum prescaller value for APB1 and APB2
	RCC->CFGR |= 0x7UL<<10; // PPRE1: divide by 16
	RCC->CFGR |= 0x7UL<<13; // PPRE2: divide by 16
	
	// Selection of PLLCLK as SYSCLK
	RCC->CFGR |= 0x2UL; // SW=2;
	while( (RCC->CFGR & 0xCUL)>>2 != (RCC->CFGR & 0x3UL) );
	
	//Desired frequencies for APB1 and APB2 clocks
	RCC->CFGR &= ~(0x7UL<<10);
	RCC->CFGR |= (0x4UL<<10); // PPRE1: div. 2
	RCC->CFGR &= ~(0x7UL<<13); 
	RCC->CFGR |= (0x0UL<<13); // PPRE2: div. 1

}

void init_FlashAccess(void){
	unsigned long WS = 1;
	FLASH->ACR = WS; // desired no. of WS
	while( ((FLASH->ACR)&(0x7)) != WS); // wait acception
	
	FLASH->ACR = WS
								| (0x1UL<< 8) // FLASH_ACR_PRFTEN
								| (0x1UL<< 9) // FLASH_ACR_ICEN
								| (0x1UL<<10); // FLASH_ACR_DCEN
	while( ((FLASH->ACR)&(0x7))!= WS); // wait acception
}

void init_SysTick(void) {
	SCB->SHPR[11] = 0x2 << 4; // Priority of SysTick
	// SCB->SHPR is used in newer libraries
	SCB->AIRCR = (SCB->AIRCR & 0xF8FFUL) | 0x05FA0000UL;
	SysTick->LOAD = 6250; // LOAD
	SysTick->VAL = 0x0UL; // CURRENT=0
	SysTick->CTRL |= 0x3UL; // TICKINT=1, TICKEN=1
}

void init_USART2 (void) {
	volatile uint32_t tmp;
	RCC->APB1ENR |= (1UL << 17); // Enable USART2 clock
	tmp = RCC->APB1ENR; // Dummy read
	GPIOD->AFR[0] |= (7UL << 20); // USART2 TX => PD5
	GPIOD->AFR[0] |= (7UL << 24); // USART2 RX => PD6
	GPIOD->MODER &= ~(3UL << 10); // clear bits 10 & 11
	GPIOD->MODER |= 2UL << 10; // MODER5 => alt. func.
	GPIOD->MODER &= ~(3UL << 12); // Clear MODER6
	GPIOD->MODER |=  (2UL << 12); // Set MODER6 to AF
	// Configures USART2: 9600 baud @ 25MHz, 8 bits, 1 stop bit, no parity
	USART2->CR1 = 0x0UL;
	USART2->CR2 = 0x0UL;
	USART2->CR3 = 0x0UL;
	USART2->CR1 |= (1UL << 13); // UE => USART enable
	USART2->CR1 |= (0UL << 12); // M => 8 bits
	USART2->CR1 |= (0UL << 10); // PCE => No parity
	USART2->CR2 |= (0UL << 12); // STOP => 1 stop bit
	USART2->BRR = 0x0A2CUL; // Baud rate generator
	USART2->CR1 |= (1UL << 3); // TE => TX enable
}