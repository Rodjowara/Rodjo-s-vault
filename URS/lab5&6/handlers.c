#include "stm32f4xx.h"

//static volatile uint32_t msTicks;

volatile uint32_t msTicks; // Protekle ms
void __attribute__((interrupt)) SysTick_Handler(void) {
	msTicks++; // Povecava se za 1 svake milisekunde
}