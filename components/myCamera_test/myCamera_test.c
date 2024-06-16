#include <stdio.h>
#include "myCamera_test.h"
#include "esp_log.h"
#include "driver/i2c_master.h"      //To use ESP as I2C master
#include "driver/gpio.h"            //To configure some gpios
#include "driver/ledc.h"            //To Generate PWM for MCLK
#include "freertos/FreeRTOS.h"      //For real time operation libraries
#include "freertos/task.h"          //For Multitasking libraries


#define HIGH    1                   //GPIO level High
#define LOW     0                   //GPIO level Low

#define GPIO_OUT_PIN_SEL_TYPE_1 ((1ULL<<CAMERA_PIN_RESET) | (1ULL<<CAMERA_PIN_PWDN))    //Bit mask for type 1 selected output pins (No pull, no interrupt)

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
void error_check_device_probe(esp_err_t err, uint16_t device_address)
{
    switch (err){
        case ESP_OK:
        ESP_LOGI(TAG,"Found I2C Device at address (0x%02X)!", device_address);
        break;

        case ESP_ERR_NOT_FOUND:
        ESP_LOGE(TAG,"I2C Device Probe Failed. Couldn't find the device at given address");
        break;

        case ESP_ERR_TIMEOUT:
        ESP_LOGE(TAG,"I2C Time out (Larger than xfer_timeout_ms). Bus is busy or Hardware crash");
        break;
    }   
}
void error_check_mclk_timer_config(esp_err_t err)
{
    switch (err){
        case ESP_OK:
        ESP_LOGI(TAG, "MCLK Timer Initialization Successful");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG, "Parameter Error. MCLK Timer Init Failed");
        break;

        case ESP_FAIL:
        ESP_LOGE(TAG, "Cannot find a proper pre-divider number based on the given frequency and the current duty_resolution. MCLK Timer Init Failed");
        break;

        case ESP_ERR_INVALID_STATE:
        ESP_LOGE(TAG, "Timer Cannot be de-configured, because timer is not configured or is not paused. MCLK Timer Deinit Failed");
        break;
    }
}
void error_check_mclk_channel_config(esp_err_t err)
{
    switch (err){
        case ESP_OK:
        ESP_LOGI(TAG, "MCLK Channel configuration successful");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG, "Parameter Error. MCLK CHannel configuration Failed");
        break;
    }
}
void error_check_mclk_get_freq(uint32_t freq)
{
    if (freq == 0)
    {
        ESP_LOGE(TAG, "Error. Couldn't get MCLK Frequency");
    }
    else
    {
        ESP_LOGI(TAG, "MCLK Frequency is %lu Hz", freq);
    }  
}
void error_check_mclk_get_duty(uint32_t duty, uint32_t duty_resolution)
{
    if (duty == LEDC_ERR_DUTY)
    {
        ESP_LOGE(TAG, "Parameter Error. Couldn't get MCLK Duty-Cycle");
    }
    else
    {
        ESP_LOGI(TAG, "MCLK Duty-Cycle is %lu%%", (duty*100)/((1 << duty_resolution))); //Conversion of duty value to %
    }  
}    

/* Initialize Necessary Clock (MCLK) to allow SCCB Operation (OV2640 doesn't work without this clock applied)*/
void init_clock_mlck(uint32_t freq)
{

    ledc_timer_config_t mclk_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = OV2640_MCLK_HZ_FREQ,
        .duty_resolution = 1, //Duty Cycle Resolution is 1 bit (Values are 0%, 50%)
        .clk_cfg = LEDC_USE_XTAL_CLK
    };

    ledc_channel_config_t mclk_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_0,
        .duty = 1, //Set Duty-Cycle to 50%
        .hpoint = 0,
        .gpio_num = CAMERA_PIN_MCLK,
    };

    error_check_mclk_timer_config(ledc_timer_config(&mclk_timer));
    error_check_mclk_channel_config(ledc_channel_config(&mclk_channel));
    error_check_mclk_get_freq(ledc_get_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0));
    error_check_mclk_get_duty(ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0), mclk_timer.duty_resolution);    

}

/* Initialize the I2C Master to communicate with the Camera Sensor OV2640 */
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

    ESP_LOGI(TAG,"Initializing I2C");
    error_check_new_master(i2c_new_master_bus(&bus_config, &i2c_bus_handle));
    error_check_add_device(i2c_master_bus_add_device(i2c_bus_handle, &device_config, &i2c_dev_handle));

    ESP_LOGI(TAG,"Probing for OV2640 Module at address 0x%02X...", device_config.device_address);
    error_check_device_probe(i2c_master_probe(i2c_bus_handle, device_config.device_address, 5), device_config.device_address);

    uint8_t data_read[1];
    uint8_t data_read_buf[2];
    uint8_t pidh_reg[1] = {0x0A};
    uint8_t pidl_reg[1] = {0x0B};

    size_t register_size = sizeof(uint8_t);
    size_t data_size = sizeof(data_read);

    i2c_master_transmit_receive(i2c_dev_handle, pidh_reg, register_size, data_read, data_size, -1);
    data_read_buf[0] = data_read[0];

    vTaskDelay(pdMS_TO_TICKS(5));

    i2c_master_transmit_receive(i2c_dev_handle, pidl_reg, register_size, data_read, data_size, -1);
    data_read_buf[1] = data_read[0];    

    ESP_LOGI(TAG, "Read register PIDH and PIDL: 0x%02X 0x%02X", data_read_buf[0], data_read_buf[1]);

    uint8_t data_read2[1];
    uint8_t data_read_buf2[2];
    uint8_t midh_reg[1] = {0x1C};
    uint8_t midl_reg[1] = {0x1D};

    size_t data_size2 = sizeof(data_read2);

    i2c_master_transmit_receive(i2c_dev_handle, midh_reg, register_size, data_read2, data_size2, -1);
    data_read_buf2[0] = data_read2[0];

    vTaskDelay(pdMS_TO_TICKS(5));

    i2c_master_transmit_receive(i2c_dev_handle, midl_reg, register_size, data_read2, data_size2, -1);
    data_read_buf2[1] = data_read2[0];    

    ESP_LOGI(TAG, "Read register MIDH and MIDL: 0x%02X 0x%02X", data_read_buf2[0], data_read_buf2[1]);
    
}

void CameraComponentTest(void)
{
    ESP_LOGI(TAG, "Camera component successfully linked to main.c");
    //gpio_dump_io_configuration(stdout,SOC_GPIO_VALID_GPIO_MASK);
    
    gpio_config_t gpio_config_out_type_1 = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .pin_bit_mask = GPIO_OUT_PIN_SEL_TYPE_1,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&gpio_config_out_type_1);

    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(CAMERA_PIN_RESET, LOW);
    gpio_set_level(CAMERA_PIN_PWDN, LOW);

    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(CAMERA_PIN_RESET, HIGH);

    vTaskDelay(pdMS_TO_TICKS(5));
    init_clock_mlck(OV2640_MCLK_HZ_FREQ);

    init_i2c_master();
}
