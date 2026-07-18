#include "led.h"

int segment_pins[7] = {SEBMENT_A, SEBMENT_B, SEBMENT_C, SEBMENT_D, SEBMENT_E, SEBMENT_F, SEBMENT_G}; 
int digit_pins[DIGITS]  = {DIG_1, DIG_2}; 

void set_segments(int number, bool show_decimal) {
    for (int i = 0; i < 7; i++) {
        gpio_set_level(segment_pins[i], patterns[number][i]);
    }
}

void clear_digits() {
    for (int i = 0; i < DIGITS; i++) {
        gpio_set_level(digit_pins[i], 0); 
    }
}

void set_digit(int digit, int value, bool show_decimal) {
    clear_digits(); 
    set_segments(value, show_decimal); 
    gpio_set_level(digit_pins[digit], 1); 
}

void extract_digits(int number, int *digits) {
    for (int i = 0; i < DIGITS; i++) {
        digits[i] = number % 10; 
        number /= 10;
    }
}

void init_gpio() {
    for (int i = 0; i < 7; i++) {
        gpio_set_direction(segment_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(segment_pins[i], 0);
    }

    for (int i = 0; i < DIGITS; i++) {
        gpio_set_direction(digit_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(digit_pins[i], 0);
    }
}
