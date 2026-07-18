#include <stdio.h>
#include <driver/gpio.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#define RX_SEND 25
#define TX_RECIEVE 26

void sensor_send(uint8_t data){
    gpio_set_level(RX_SEND, 0);
    esp_rom_delay_us(1000);

    for(int i = 0; i < 8; i++){
        int bit = (data >> i) & 1;
        gpio_set_level(RX_SEND, bit);
        esp_rom_delay_us(1000);
    }

    gpio_set_level(RX_SEND, 1);
}

uint8_t sensor_read(){
    while(gpio_get_level(TX_RECIEVE) == 1){
    }
    esp_rom_delay_us(1000);

    uint8_t received = 0;
    for(int i = 0; i < 8; i++){
        int bit = gpio_get_level(TX_RECIEVE);
        received |= (bit << i);
        esp_rom_delay_us(1000);
    }
    return received;
}

void gpio_init(){
    gpio_config_t tx_config = {
        .pin_bit_mask = (1ULL << TX_RECIEVE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE  
    };
    gpio_config(&tx_config);

    gpio_config_t rx_config = {
        .pin_bit_mask = (1ULL << RX_SEND),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE  
    };
    gpio_config(&rx_config);

    gpio_set_level(RX_SEND, 1);
}

void app_main(void)
{
    gpio_init();

    for(int i = 100; i < 201; i++){
        uint8_t data = (uint8_t)i;

        printf("Sending data: %d\n", data);
        sensor_send(data);
        uint8_t received = sensor_read();
        printf("Received data: %d\n", received);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}