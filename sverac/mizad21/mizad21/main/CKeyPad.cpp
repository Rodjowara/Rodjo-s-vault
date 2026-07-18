#include "CKeyPad.h"

CKeyPad::CKeyPad(gpio_num_t r1, gpio_num_t r2, gpio_num_t r3, gpio_num_t r4, gpio_num_t c1, gpio_num_t c2, gpio_num_t c3) { 
    gpio_reset_pin(r1);
    gpio_reset_pin(r2);
    gpio_reset_pin(r3);
    gpio_reset_pin(r4);
    gpio_reset_pin(c1);
    gpio_reset_pin(c2);
    gpio_reset_pin(c3);

    gpio_set_direction(r1, GPIO_MODE_OUTPUT);
    gpio_set_direction(r2, GPIO_MODE_OUTPUT);
    gpio_set_direction(r3, GPIO_MODE_OUTPUT);
    gpio_set_direction(r4, GPIO_MODE_OUTPUT);
    gpio_set_direction(c1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(c1, GPIO_PULLUP_ONLY);
    gpio_set_direction(c2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(c2, GPIO_PULLUP_ONLY);
    gpio_set_direction(c3, GPIO_MODE_INPUT);
    gpio_set_pull_mode(c3, GPIO_PULLUP_ONLY);
}

int checkPress() {
    int key = -1;

    gpio_set_level((gpio_num_t)R1, 0);
    gpio_set_level((gpio_num_t)R2, 1);
    gpio_set_level((gpio_num_t)R3, 1);
    gpio_set_level((gpio_num_t)R4, 1);

    if (gpio_get_level((gpio_num_t)C1) == 0) {
        key = 1;
    }
    if (gpio_get_level((gpio_num_t)C2) == 0) {
        key = 2;
    }
    if (gpio_get_level((gpio_num_t)C3) == 0) {
        key = 3;
    }

    gpio_set_level((gpio_num_t)R1, 1);
    gpio_set_level((gpio_num_t)R2, 0);
    gpio_set_level((gpio_num_t)R3, 1);
    gpio_set_level((gpio_num_t)R4, 1);

    if (gpio_get_level((gpio_num_t)C1) == 0) {
        key = 4;
    }
    if (gpio_get_level((gpio_num_t)C2) == 0) {
        key = 5;
    }
    if (gpio_get_level((gpio_num_t)C3) == 0) {
        key = 6;
    }

    gpio_set_level((gpio_num_t)R1, 1);
    gpio_set_level((gpio_num_t)R2, 1);
    gpio_set_level((gpio_num_t)R3, 0);
    gpio_set_level((gpio_num_t)R4, 1);

    if (gpio_get_level((gpio_num_t)C1) == 0) {
        key = 7;
    }
    if (gpio_get_level((gpio_num_t)C2) == 0) {
        key = 8;
    }
    if (gpio_get_level((gpio_num_t)C3) == 0) {
        key = 9;
    }

    gpio_set_level((gpio_num_t)R1, 1);
    gpio_set_level((gpio_num_t)R2, 1);
    gpio_set_level((gpio_num_t)R3, 1);
    gpio_set_level((gpio_num_t)R4, 0);

    if (gpio_get_level((gpio_num_t)C1) == 0) {
        key = 10;
    }
    if (gpio_get_level((gpio_num_t)C2) == 0) {
        key = 11;
    }
    if (gpio_get_level((gpio_num_t)C3) == 0) {
        key = 12;
    }

    return key;
}

void CKeyPad::tick() {
    int key = checkPress();

    switch (m_buttonState) {
    case IDLE:
        if (lastRelease + DOUBLE_TIME > esp_timer_get_time()) {
            m_buttonState = IDLE;
        } else if (key != -1) {
            //ESP_LOGI(LogName, "Key pressed: %d", key);
            m_lastKey = key;
            m_buttonState = PRESSED;
            startPress = esp_timer_get_time();
        }
        break;

    case PRESSED:
        if (key == -1) { 
            lastRelease = esp_timer_get_time();
            m_buttonState = RELEASED;
        } else if (esp_timer_get_time() - startPress > LONG_TIME) {
            longPress(m_lastKey);
            m_buttonState = LONG_PRESS;
        }
        break;

    case RELEASED:
        if (esp_timer_get_time() - lastRelease < DOUBLE_TIME) {
            if (key != -1) {
                doubleClick(m_lastKey);
                lastRelease = esp_timer_get_time();
                m_buttonState = IDLE;
                m_lastKey = -1;
            }
        } else {
            singleClick(m_lastKey);
            m_buttonState = IDLE;
            m_lastKey = -1;
        }
        break;

    case LONG_PRESS:
        if (key == -1) {
            m_buttonState = IDLE;
            m_lastKey = -1;
        }
        break;

    default:
        m_buttonState = IDLE;
        m_lastKey = -1;
        break;
    }
}
