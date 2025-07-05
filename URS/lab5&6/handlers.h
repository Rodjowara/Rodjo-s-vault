#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdint.h>

extern volatile uint32_t msTicks;
void SysTick_Handler(void);

#endif