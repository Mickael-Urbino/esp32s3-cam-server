#include <stdio.h>
#include "myCamera_test.h"
#include "esp_log.h"
#include "driver/i2c_master.h" //To use ESP as I2C master

static const char* TAG = "myCamera";

void error_check_new_master(esp_err_t err)
{
    switch (err){
        case ESP_OK:
        ESP_LOGI(TAG,"I2C Bus Master properly initialized");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG,"I2C bus initialization failed due to invalid argument");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG,"Create I2C bus failed because of insufficient memory");
        break;

        case ESP_ERR_NOT_FOUND:
        ESP_LOGE(TAG,"No more free bus");
        break;
    }   
}
void error_check_add_device(esp_err_t err)
{
    switch (err){
        case ESP_OK:
        ESP_LOGI(TAG,"I2C Master Device Created Successfully");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG,"I2C bus initialization failed due to invalid argument");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG,"Create I2C bus failed because of insufficient memory");
        break;
    }   
}


void init_i2c_master(void)
{
    /* I2C MASTER BUS Configuration Structure */
    i2c_master_bus_config_t bus_config = {
        .i2c_port = -1,
        .sda_io_num = CAMERA_PIN_SDA,
        .scl_io_num = CAMERA_PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
    };

    /* I2C MASTER Device Configuration Structure */
    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = OV2640_I2C_ADDRESS,
        .scl_speed_hz = OV2640_I2C_HZ_FREQ,
    };

    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_dev_handle_t i2c_dev_handle;

    error_check_new_master(i2c_new_master_bus(&bus_config, &i2c_bus_handle));
    error_check_add_device(i2c_master_bus_add_device(i2c_bus_handle, &device_config, &i2c_dev_handle));

    if (i2c_master_probe(i2c_bus_handle, OV2640_I2C_ADDRESS, -1) != ESP_OK)
    {
        ESP_LOGE(TAG,"Failed to probe OV2640 Camera");
    }
    else
    {
        ESP_LOGI(TAG,"Found OV2640 Camera (I2C Probe Successful)");
    }
    
}

void CameraComponentTest(void)
{
    ESP_LOGI(TAG, "Camera component successfully linked to main.c");
    init_i2c_master();
}
