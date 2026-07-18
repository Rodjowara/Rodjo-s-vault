#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C1_ADDR         0x20 
#define I2C2_ADDR         0x22 
#define I2C3_ADDR         0x24 

#define REG_OP            0x00
#define REG_PAR1          0x01
#define REG_PAR2          0x02

#define I2C1_ADD           0x01
#define I2C1_SUB           0x02

#define I2C2_MUL           0x01
#define I2C2_DIV           0x02

#define I2C3_POWER         0x01
#define I2C3_SQRT          0x02

typedef union {
    float f;
    uint8_t bytes[4];
} float_union_t;

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 5,
        .scl_io_num = 15,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    i2c_config_t conf2 = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 25,
        .scl_io_num = 26,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(I2C_NUM_1, &conf2);
    i2c_driver_install(I2C_NUM_1, conf2.mode, 0, 0, 0);
}

void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_port_t i2c_bus;
    if (addr == I2C3_ADDR || addr == I2C2_ADDR) {
        i2c_bus = I2C_NUM_1;
    } else {
        i2c_bus = I2C_NUM_0;
    }
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void i2c_read_result(uint8_t addr, float *result) {
    i2c_port_t i2c_bus;
    if (addr == I2C3_ADDR || addr == I2C2_ADDR) {
        i2c_bus = I2C_NUM_1;
    } else {
        i2c_bus = I2C_NUM_0;
    }
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, (uint8_t *)result, 4, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_bus, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

float i2c_calculate(float a, float b, float c) {
    float_union_t converter;
    float result = 0.0f;
    uint8_t op;

    float b2 = 0.0f;
    while(result == 0.0f) {
        converter.f = b;
        i2c_write_reg(I2C3_ADDR, REG_PAR1, converter.bytes, 4);
        op = I2C3_POWER;
        i2c_write_reg(I2C3_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        i2c_read_result(I2C3_ADDR, &result);
        b2 = result;
        //printf("b2: %f\n", b2);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    result = 0.0f;

    float ac = 0.0f;
    while(result == 0.0f) {
        //printf("a: %f, c: %f\n", a, c);
        converter.f = a;
        i2c_write_reg(I2C2_ADDR, REG_PAR1, converter.bytes, 4);
        converter.f = c;
        i2c_write_reg(I2C2_ADDR, REG_PAR2, converter.bytes, 4);
        op = I2C2_MUL;
        i2c_write_reg(I2C2_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        i2c_read_result(I2C2_ADDR, &result);
        ac = result;
        //printf("ac: %f\n", ac);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    result = 0.0f;

    float pod_korijenom = 0.0f;
    while(result == 0.0f) {
        converter.f = b2;
        i2c_write_reg(I2C1_ADDR, REG_PAR1, converter.bytes, 4);
        converter.f = ac;
        i2c_write_reg(I2C1_ADDR, REG_PAR2, converter.bytes, 4);
        op = I2C1_SUB;
        i2c_write_reg(I2C1_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        i2c_read_result(I2C1_ADDR, &result);
        pod_korijenom = result;
        //printf("pod_korijenom: %f\n", pod_korijenom);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    result = 0.0f;

    float sqrt_pod_korijenom = 0.0f;
    while(result == 0.0f){
        converter.f = pod_korijenom;
        i2c_write_reg(I2C3_ADDR, REG_PAR1, converter.bytes, 4);
        op = I2C3_SQRT;
        i2c_write_reg(I2C3_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        i2c_read_result(I2C3_ADDR, &result);
        sqrt_pod_korijenom = result;
        //printf("sqrt_pod_korijenom: %f\n", sqrt_pod_korijenom);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    result = 0.0f;

    float brojnik = 0.0f;
    while(result == 0.0f) {
        converter.f = b;
        i2c_write_reg(I2C1_ADDR, REG_PAR1, converter.bytes, 4);
        converter.f = sqrt_pod_korijenom;
        i2c_write_reg(I2C1_ADDR, REG_PAR2, converter.bytes, 4);
        op = I2C1_ADD;
        i2c_write_reg(I2C1_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        i2c_read_result(I2C1_ADDR, &result);
        brojnik = result;
        //printf("brojnik: %f\n", brojnik);
    }
    result = 0.0f;

    while(result == 0.0f) {
        converter.f = brojnik;
        i2c_write_reg(I2C2_ADDR, REG_PAR1, converter.bytes, 4);
        converter.f = a;
        i2c_write_reg(I2C2_ADDR, REG_PAR2, converter.bytes, 4);
        op = I2C2_DIV;
        i2c_write_reg(I2C2_ADDR, REG_OP, &op, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        i2c_read_result(I2C2_ADDR, &result);
    }
    return result;
}

void app_main() {
    float a = 10.0f;
    float b = 11.5f;
    float c = 12.7f;
    
    i2c_master_init();
    
    float i2c_calc = i2c_calculate(a, b, c);
    float manual = (b + sqrt(b*b - a*c)) / a;

    ESP_LOGI("I2C", "results: i2c: %f, manual: %f", i2c_calc, manual);
}