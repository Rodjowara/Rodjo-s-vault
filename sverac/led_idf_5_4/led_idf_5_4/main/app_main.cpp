#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "CButton.h"

static const char *TAG = "MAIN";

#define BUTTON_GPIO 0

void onSingleClick() {
    ESP_LOGI(TAG, "Single click\n");
}

void onDoubleClick() {
    ESP_LOGI(TAG, "Double click\n");
}

void onLongPress() {
    ESP_LOGI(TAG, "Long press\n");
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Start MAIN.");
    
    CButton button(BUTTON_GPIO);

    button.attachSingleClick(onSingleClick);
    button.attachDoubleClick(onDoubleClick);
    button.attachLongPress(onLongPress); 
    
    //Main loop
    while(1) {
        button.tick();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}