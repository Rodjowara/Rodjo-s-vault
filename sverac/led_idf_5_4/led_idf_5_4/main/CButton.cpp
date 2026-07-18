#include "CButton.h"
// #include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_timer.h"

gpio_num_t m_pinNumber;
const char *LogName = "CButton";

CButton::CButton(int port){
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure port[%d] to input!!!", port);
    gpio_reset_pin(m_pinNumber);
    gpio_set_direction(m_pinNumber, GPIO_MODE_INPUT);
    gpio_set_pull_mode(m_pinNumber, GPIO_PULLUP_ONLY);
}

void CButton::tick(){
    static bool lastReading = 1;
    static bool buttonState = 1;

    static int64_t lastDebounceTime = 0;
    static int64_t pressStartTime = 0;
    static int64_t lastReleaseTime = 0;
    static int clickCount = 0;

    const int debounceDelay = 50;
    const int doubleClickDelay = 300;
    const int longPressDelay = 1000;

    bool reading = gpio_get_level(m_pinNumber);
    int64_t now = esp_timer_get_time() / 1000;

    //ESP_LOGI(LogName, "Reading: %d", reading);

    if (reading != lastReading) {
        lastDebounceTime = now;
    }

    if ((now - lastDebounceTime) > debounceDelay) {

        if(reading == 0){
            if(now - lastDebounceTime >= longPressDelay) {
                    if (longPress) longPress();
                    clickCount = 0;
            }
        }

        if (reading != buttonState) {
            buttonState = reading;

            // PRITISAK
            if (buttonState == 0) {
                pressStartTime = now;
            }

            // OTPUŠTANJE
            else {
                int pressDuration = now - pressStartTime;

                if (pressDuration >= longPressDelay) {
                    if (longPress) longPress();
                    clickCount = 0;
                } else {
                    clickCount++;
                    lastReleaseTime = now;
                }
            }
        }
    }

    // SINGLE / DOUBLE
    if (clickCount > 0 && (now - lastReleaseTime) > doubleClickDelay) {
        if (clickCount == 1) {
            if (singleClick) singleClick();
        } else {
            if (doubleClick) doubleClick();
        }
        clickCount = 0;
    }

    lastReading = reading;
}