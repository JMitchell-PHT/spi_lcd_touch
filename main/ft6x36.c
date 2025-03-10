#include <esp_log.h>
#include "lvgl.h"
#include "ft6x36.h"
#include "driver/i2c.h"
#include <inttypes.h>

#define TAG "FT6X36"
#define I2C_PORT I2C_NUM_0  // Define the I2C port being used
#define I2C_SDA_PIN 8  // Change to your actual SDA pin
#define I2C_SCL_PIN 9  // Change to your actual SCL pin
#define I2C_FREQ_HZ 40000
void i2c_scan() {
    uint8_t data = 0;
    esp_err_t ret;

    for (int addr = 0x08; addr < 0x78; addr++) {
        ret = i2c_master_write_to_device(I2C_PORT, addr, &data, 0, 100 / portTICK_PERIOD_MS);
        if (ret == ESP_OK) {
            ESP_LOGI("I2C", "Device found at 0x%X", addr);
        }
    }
}
void i2c_init() {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ
    };
    i2c_param_config(I2C_NUM_0, &i2c_conf);
    i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0);

    i2c_scan();
}
//static ft6x36_status_t ft6x36_status;
static uint8_t current_dev_addr;
static bool ft6x36_initialized = false;

static esp_err_t ft6x36_i2c_read(uint8_t reg, uint8_t *data, size_t len) {
    if (!ft6x36_initialized) return ESP_FAIL;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (current_dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (current_dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void ft6x06_init(uint16_t dev_addr) {
    i2c_init();
    current_dev_addr = dev_addr;
    ft6x36_initialized = true;
    ESP_LOGI(TAG, "FT6X36 initialized at I2C address 0x%X", dev_addr);
}

void ft6x36_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    if (!ft6x36_initialized) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    uint8_t buf[5];
    if (ft6x36_i2c_read(0x02, buf, 5) != ESP_OK) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    if ((buf[0] & 0x0F) == 0) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    data->point.x = ((buf[1] & 0x0F) << 8) | buf[2];
    data->point.y = ((buf[3] & 0x0F) << 8) | buf[4];
    data->state = LV_INDEV_STATE_PRESSED;

    ESP_LOGD(TAG, "Touch: X=%" PRId32 " Y=%" PRId32, data->point.x, data->point.y);
}