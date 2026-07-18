#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led.h"
#include "esp_timer.h"

#include "esp_adc/adc_oneshot.h"

#define ADC_PIN             ADC_CHANNEL_9 // spojen je na pin 26, sto je 2 unit, 9 kanal
#define ADC_UNIT            ADC_UNIT_2

#define SIGNAL_PIN 26

int tolerancija = 50; // mV
int sredina = 2500;
int frekvencija;

#define BROJ_PERIODA 5

void ADC_Task(void *pvParameters)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_PIN, &config);

    int adc_value = 0;
    float napon, napon_prev;
    int64_t start_time, end_time;
    float ukupna_frekvencija;

    while (1)
    {
        ukupna_frekvencija = 0;

        for (int i = 0; i < BROJ_PERIODA; i++)
        {   
            // cekaj da izadje iz sredine
            do {
                napon_prev = napon;
                adc_oneshot_read(adc_handle, ADC_PIN, &adc_value);
                napon = adc_value * 3300. / 4095.;
            } while (napon > (sredina - tolerancija) && napon < (sredina + tolerancija));

            do {
                napon_prev = napon;
                adc_oneshot_read(adc_handle, ADC_PIN, &adc_value);
                napon = adc_value * 3300. / 4095.;
            } while (!((napon_prev < sredina) && (napon >= sredina)));

            // u sredini je, zabiljezi vrijeme
            start_time = esp_timer_get_time();

            // cekaj da izadje iz sredine
            do {
                napon_prev = napon;
                adc_oneshot_read(adc_handle, ADC_PIN, &adc_value);
                napon = adc_value * 3300. / 4095.;
            } while (napon > (sredina - tolerancija) && napon < (sredina + tolerancija));

            do {
                napon_prev = napon;
                adc_oneshot_read(adc_handle, ADC_PIN, &adc_value);
                napon = adc_value * 3300. / 4095.;
            } while (!((napon_prev < sredina) && (napon >= sredina)));

            // ponovo dosao do sredine, zabiljezi vrijeme
            end_time = esp_timer_get_time();

            int64_t period = end_time - start_time;
            float frekvencija_tmp = 1000000. / period;

            ukupna_frekvencija += frekvencija_tmp;
        }

        float srednja_frekvencija = ukupna_frekvencija / BROJ_PERIODA;

        ESP_LOGI("ADC", "frekvencija : %.2f Hz", srednja_frekvencija);

        frekvencija = (int)srednja_frekvencija;

        ESP_LOGI("ADC", "frekvencija u intu : %d Hz", frekvencija);

        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

void display_task(void *pvParameter)
{
    init_gpio(); 

    int digits[DIGITS] = {0}; 

    while (1) {
        extract_digits(frekvencija, digits);

        for (int j = 0; j < 2; j++) {
            set_digit(j, digits[j], false);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void app_main(void)
{
    xTaskCreate(ADC_Task, "ADC Task", 2048, NULL, 5, NULL);
    xTaskCreate(display_task, "Display Task", 2048, NULL, 5, NULL);
}
