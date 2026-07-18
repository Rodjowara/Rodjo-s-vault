#include <stdio.h>
#include <driver/gpio.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#define BUTTON_PIN 13
#define ADC_CHANNEL ADC1_CHANNEL_6
#define DHT_GPIO 15

static portMUX_TYPE dht_mux = portMUX_INITIALIZER_UNLOCKED;

void button_init(){
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE  
    };
    gpio_config(&io_config);
}

void adc_init(){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
}

int adc_read(){
    return adc1_get_raw(ADC_CHANNEL);
}

void dht_start(){
    gpio_set_direction(DHT_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(DHT_GPIO, 0);
    esp_rom_delay_us(1200);

    gpio_set_level(DHT_GPIO, 1);
    esp_rom_delay_us(30);

    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);
}

static void dht_resync(){
    int timeout = 20000;

    while (gpio_get_level(DHT_GPIO) == 0 && timeout--) {
        esp_rom_delay_us(1);
    }

    timeout = 20000;
    while (gpio_get_level(DHT_GPIO) == 1 && timeout--) {
        esp_rom_delay_us(1);
    }
}

static int dht_read_bit(){

    int timeout = 20000;

    while (gpio_get_level(DHT_GPIO) == 1 && timeout--) {
        esp_rom_delay_us(1);
    }

    timeout = 20000;
    while (gpio_get_level(DHT_GPIO) == 0 && timeout--) {
        esp_rom_delay_us(1);
    }

    int64_t start = esp_timer_get_time();

    timeout = 20000;
    while (gpio_get_level(DHT_GPIO) == 1 && timeout--) {

    }

    int duration = esp_timer_get_time() - start;

    return (duration > 65) ? 1 : 0;
}

int dht_decode(float *temp, float *hum){
    uint8_t data[5] = {0};

    dht_start();
    dht_resync();

    if (gpio_get_level(DHT_GPIO) == 1) {
        printf("DHT not responding\n");
        return -1;
    }

    taskENTER_CRITICAL(&dht_mux);

    for(int i = 0; i < 40; i++){
        int bit = dht_read_bit();

        data[i/8] <<= 1;
        data[i/8] |= bit;

        esp_rom_delay_us(2);
    }

    taskEXIT_CRITICAL(&dht_mux);

    if(data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)){
        printf("Checksum error\n");
        return -1;
    }

    *hum  = ((data[0] << 8) | data[1]) * 0.1f;

    int16_t t = ((data[2] & 0x7F) << 8) | data[3];
    *temp = t * 0.1f;

    if (data[2] & 0x80) {
        *temp *= -1;
    }

    return 0;
}

float convert_to_temp(int adc_value){
    float vout = (adc_value / 4095.0f) * 3.3f;

    float r_fixed = 10000.0f;
    float r_therm = r_fixed * vout / (3.3f - vout);

    float T0 = 298.15f;
    float B  = 3950.0f;
    float R0 = 10000.0f;

    float tempK = 1.0f / ((1.0f / T0) + (1.0f / B) * log(r_therm / R0));

    return tempK - 273.15f;
}

void app_main(void)
{
    button_init();
    adc_init();

    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << DHT_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_config);

    bool released = true;

    while (1) {
        int adc_value = adc_read();
        float temperature = convert_to_temp(adc_value);

        int pressed = 1;

        if (gpio_get_level(BUTTON_PIN) == 0) {
            vTaskDelay(pdMS_TO_TICKS(20));
            if (gpio_get_level(BUTTON_PIN) == 0) {
                pressed = 0;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(20));
            if (gpio_get_level(BUTTON_PIN) == 1) {
                pressed = 1;
            }
        }

        if (pressed == 0 && released) {
            released = false;

            printf("Temperature ADC: %.2fC\n", temperature);

            float temp, hum;
            if(dht_decode(&temp, &hum) == 0){
                printf("DHT Temperature: %.2fC, Humidity: %.2f%%\n", temp, hum);
            } else {
                printf("Failed to read DHT\n");
            }
        }

        if (pressed == 1) {
            released = true;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}