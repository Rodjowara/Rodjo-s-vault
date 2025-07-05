#include "stm32f4xx.h"
#include "handlers.h"
#include "stdio.h"
#include "init.h"

void Delay (uint32_t NoOfTicks) {
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < NoOfTicks);
}

void sendchar_USART2(int32_t c) {
	while (!(USART2->SR & 0x0080));
	USART2->DR = (c & 0xFF);
}

int main(void){
	volatile uint32_t tmp;
	
	// Enabling clock for GPIOD
	// |= to preserve other bits, e.g. CCMDATARAMEN
	RCC->AHB1ENR |= (0x1UL << 3); //GPIOD
	RCC->AHB1ENR |= (0x1UL << 1); 
	RCC->AHB1ENR |= (0x1UL << 4); //GPIOE
	
	// Dummy read. 2 cycles are needed for clock
	// to be turned on after writting into
	// AHB1ENR, AHB2ENR, AHB3ENR, APB1ENR and ABPB2ENR.
	tmp = RCC->AHB1ENR;
	
	// Configure pins 13, 12 as output.
	// 0b0000XXXX XXXXXXXX XXXXXXXX XXXXXXXX
	GPIOD->MODER &= ~((0x3UL << 26U) | (0x3UL << 24U));
	
	// 0b0101XXXX XXXXXXXX XXXXXXXX XXXXXXXX
	GPIOD->MODER |= ((0x1UL << 26U) | (0x1UL << 24U));
	
	// LED on pin 13 OFF, LED on pin 12 ON.
	GPIOD->ODR &= ~(0x1UL << 13U); // OFF
	GPIOD->ODR |= (0x1UL << 12U); // ON
	
	//inicijalizacija ledica
	GPIOB->MODER &= ~((0x3 << 26U) | (0x3 << 28U) | 0x3 << 30U);
	GPIOB->MODER |= ((0x1 << 26U) | (0x1 << 28U) | (0x1 << 30U));
	GPIOB->ODR &= ~((0x1 << 13U) | (0x1 << 14U) | (0x1 << 15U));
	
	GPIOE->MODER &= ~(0x3 << 22U);
	GPIOE->MODER |= (0x1 << 22U);
	
	//paljenje i gašenje RGB ledice
	for(int i = 0; i < 3; i++){
		GPIOB->ODR |= (0x1 << 13U);
		Delay(1000);
		GPIOB->ODR &= ~(0x1 << 13U);
		GPIOB->ODR |= (0x1 << 14U);
		Delay(1000);
		GPIOB->ODR &= ~(0x1 << 14U);
		GPIOB->ODR |= (0x1 << 15U);
		Delay(1000);
		GPIOB->ODR &= ~(0x1 << 15U);
	}
	
	init_USART2();
	//iskljuci sve ledice
	GPIOB->ODR &= ~((0x1 << 13U) | (0x1 << 14U) | (0x1 << 15U));
	printf("Redefinicija sistemskih poziva uspjesno obavljena!\r\n");
	
	FILE* fMorse=fopen ("Morse","w");
	fprintf (fMorse,"SOS");
	fclose (fMorse);
	
	while(1); // Stay here forever.
}