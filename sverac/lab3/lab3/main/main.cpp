#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DS1307.h"

DS1307 ds1307;
const char* dow[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

extern "C" void app_main() {

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_26);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    ds1307.begin(GPIO_NUM_23,
              GPIO_NUM_22);

    // RTC_time setTime;
    // setTime.day = 22;
    // setTime.month = 7;
    // setTime.year = 2003;
    // setTime.hour = 8;
    // setTime.min = 0;
    // setTime.sec = 0;
    // setTime.meridiem = 1;
    // setTime.mode = 1;
    // ds1307.setTime(setTime);
    uint8_t raw;    
    // ds1307.writeRegister(0x00, 0x80);
    while (true) {

        RTC_time time;
        if(!gpio_get_level(GPIO_NUM_26)){
            if (ds1307.getTime(time) == ESP_OK) {
                // ds1307.readRegister(0x01, raw);
                // printf("Minute reg: 0x%02X\n", raw);
                printf("%02d:%02d:%02d  %02d.%02d.%04d (Day of week:%s)\n",
                    time.hour,
                    time.min,
                    time.sec,
                    time.day,
                    time.month,
                    time.year,
                    dow[time.dow - 1]);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}