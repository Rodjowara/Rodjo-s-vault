#include "driver/i2c.h"
#include <stdint.h>

struct RTC_time{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t dow;
    uint8_t day;
    uint8_t month;
    uint16_t year;

    uint8_t mode;
    uint8_t meridiem;
};

class DS1307{
private:
    i2c_port_t i2c_port;
    uint8_t address;

    uint8_t decToBcd(uint8_t val);
    uint8_t bcdToDec(uint8_t val);

public:
    DS1307(i2c_port_t = I2C_NUM_0, uint8_t addr = 0x22);
    esp_err_t begin(gpio_num_t sda, gpio_num_t scl, uint32_t freq = 100000);
    esp_err_t setTime(const RTC_time &t);
    esp_err_t getTime(RTC_time &t);
    esp_err_t readRegister(uint8_t reg, uint8_t &value);
    esp_err_t writeRegister(uint8_t reg, uint8_t value);
};