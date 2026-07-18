#include "DS1307.h"

#include <string.h>
#include <stdio.h>

DS1307::DS1307(i2c_port_t port, uint8_t addr) {
    i2c_port = port;
    address = addr;
}

uint8_t DS1307::decToBcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t DS1307::bcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

esp_err_t DS1307::begin(gpio_num_t sda,
                        gpio_num_t scl,
                        uint32_t freq) {

    i2c_config_t config = {};

    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = sda;
    config.scl_io_num = scl;

    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;

    config.master.clk_speed = freq;

    ESP_ERROR_CHECK(i2c_param_config(i2c_port, &config));

    return i2c_driver_install(i2c_port,
                              config.mode,
                              0,
                              0,
                              0);
}

esp_err_t DS1307::writeRegister(uint8_t reg,
                                uint8_t value) {

    uint8_t data[2] = {reg, value};

    //konverzija floata u little-endian bajtove
    //uint8_t tx_buf[4] = {reg, 0, 0, 0, 0};
    //kopiraj float u bajtove
    //memcpy(&tx_buf[1], &value, sizeof(float));

    return i2c_master_write_to_device(
        i2c_port,   //koji kontroler (port)
        address,    //adresa slave uređaja
        data,       //podatci za slanje
        2,          //veličina podataka za slanje (za float 5 bajtova)
        pdMS_TO_TICKS(1000)     //timeout u milisekundama
    );
}

//konverzija bajtova u float:
// uint8_t data[4];

// ESP_ERROR_CHECK(i2c_master_receive(dev_handle, data, 4, -1));

// union {
//     uint32_t u32;
//     float f;
// } result;

// result.u32 =
//       ((uint32_t)data[0])
//     | ((uint32_t)data[1] << 8)
//     | ((uint32_t)data[2] << 16)
//     | ((uint32_t)data[3] << 24);

// printf("Rezultat = %f\n", result.f);

//pretvorba float u bajtove:
// union {
//     uint32_t u32;
//     float f;
// } value;

// value.f = 3.1415927f;

// uint8_t data[4];

// data[0] = (uint8_t)(value.u32);
// data[1] = (uint8_t)(value.u32 >> 8);
// data[2] = (uint8_t)(value.u32 >> 16);
// data[3] = (uint8_t)(value.u32 >> 24);

//čitanje sa slavea:
// i2c_master_read_from_device(
//     I2C_NUM_0,   port
//     0x20,        slave adresa
//     data,        gdje se spremaju podatci
//     4,           veličina podataka za čitanje
//     pdMS_TO_TICKS(100)
// );

esp_err_t DS1307::readRegister(uint8_t reg,
                               uint8_t &value) {

    return i2c_master_write_read_device(
        i2c_port,
        address,
        &reg,
        1,
        &value,
        1,
        pdMS_TO_TICKS(1000)
    );
}

esp_err_t DS1307::setTime(const RTC_time &t) {

    uint8_t data[8];

    data[0] = 0x00;

    data[1] = decToBcd(t.sec);
    data[2] = decToBcd(t.min);

    if (t.mode == 0) {

        data[3] = decToBcd(t.hour);

    } else {

        data[3] = decToBcd(t.hour);

        data[3] |= (1 << 6);

        if (t.meridiem == 1)
            data[3] |= (1 << 5);
    }

    data[4] = decToBcd(t.dow);
    data[5] = decToBcd(t.day);
    data[6] = decToBcd(t.month);
    data[7] = decToBcd(t.year - 2000);

    return i2c_master_write_to_device(
        i2c_port,
        address,
        data,
        8,
        pdMS_TO_TICKS(1000)
    );
}

esp_err_t DS1307::getTime(RTC_time &t) {

    uint8_t reg = 0x00;
    uint8_t data[7];

    esp_err_t err =
        i2c_master_write_read_device(
            i2c_port,
            address,
            &reg,
            1,
            data,
            7,
            pdMS_TO_TICKS(1000)
        );

    if (err != ESP_OK)
        return err;

    t.sec = bcdToDec(data[0] & 0x7F);
    t.min = bcdToDec(data[1]);

    if (data[2] & (1 << 6)) {

        t.mode = 1;

        t.hour = bcdToDec(data[2] & 0x1F);

        t.meridiem =
            (data[2] & (1 << 5))
            ? 1
            : 0;

    } else {

        t.mode = 0;

        t.hour = bcdToDec(data[2] & 0x3F);
    }
    t.dow = bcdToDec(data[3]);
    t.day = bcdToDec(data[4]);
    t.month = bcdToDec(data[5]);
    t.year = 2000 + bcdToDec(data[6]);

    return ESP_OK;
}