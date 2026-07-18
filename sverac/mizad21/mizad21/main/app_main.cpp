#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "CKeyPad.h"

char convertKey(int pressed_key) {
    char keyChar = 0;
    if (pressed_key < 10) {
        keyChar = pressed_key + '0';
    } else if (pressed_key == 10) {
        keyChar = '*';
    } else if (pressed_key == 11) {
        keyChar = '0';
    } else if (pressed_key == 12) {
        keyChar = '#';
    }

    return keyChar;
}

void handleSingleClick(int pressed_key) {
    printf("Single click: %c\n", convertKey(pressed_key));
}

void handleDoubleClick(int pressed_key) {
    printf("Double click: %c\n", convertKey(pressed_key));
}

void handleLongClick(int pressed_key) {
    printf("Long press: %c\n", convertKey(pressed_key));
}

extern "C" void app_main(void) {

    CKeyPad *keypad = new CKeyPad((gpio_num_t)R1, (gpio_num_t)R2, (gpio_num_t)R3, (gpio_num_t)R4, (gpio_num_t)C1, (gpio_num_t)C2, (gpio_num_t)C3);
    keypad->attachSingleClick(handleSingleClick);
    keypad->attachDoubleClick(handleDoubleClick);
    keypad->attachLongPress(handleLongClick);

    while(1) {
        keypad->tick();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}