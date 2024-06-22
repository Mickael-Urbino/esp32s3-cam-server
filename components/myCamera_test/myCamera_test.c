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
#define ON      1                   //Functionality is ON
#define OFF     0                   //Functionality is OFF

#define  SELECT_DSP_BANK     0x00
#define  SELECT_SENSOR_BANK  0x01

#define GPIO_OUT_PIN_SEL_TYPE_1 ((1ULL<<CAMERA_PIN_RESET) | (1ULL<<CAMERA_PIN_PWDN))    //Bit mask for type 1 selected output pins (No pull, no interrupt)

static const char* TAG = "myCamera";

/* Define DSP_BANK and SENSOR_BANK Register names + Size of the array containing them */
uint8_t dsp_bank[] = {
    R_BYPASS_REG,
    Q_SCALE_REG,
    CTRLI_REG,   
    HSIZE_REG,
    VSIZE_REG,
    XOFFL_REG,
    YOFFL_REG,
    VHYX_REG,
    DPRP_REG,
    TEST_REG,
    ZMOW_REG,
    ZMOH_REG,
    ZMHH_REG,
    BPADDR_REG,
    BPDATA_REG,
    CTRL2_REG,
    CTRL3_REG,
    SIZEL_REG,
    HSIZE8_REG,
    VSIZE8_REG,
    CTRL0_REG,
    CTRL1_REG,
    R_DVP_SP_REG,
    IMAGE_MODE_REG,
    RESET_REG,
    REGED_REG,
    MS_SP_REG,
    SS_ID_REG,
    SS_CTRL_REG,
    MC_BIST_REG,
    MC_AL_REG,
    MC_AH_REG,
    MC_D_REG,
    P_CMD_REG,
    P_STATUS_REG,
    RA_DLMT_REG,
};
uint8_t sensor_bank[] = {
    GAIN_REG,
    COM1_REG,
    REG04_REG,
    REG08_REG,
    COM2_REG,
    PIDH_REG,
    PIDL_REG,
    COM3_REG,
    AEC_REG,
    CLKRC_REG,
    COM7_REG,
    COM8_REG,
    COM9_REG,
    COM10_REG,
    HREFST_REG,
    HREFEND_REG,
    VSTRT_REG,
    VEND_REG,
    MIDH_REG,
    MIDL_REG,
    AEW_REG,
    AEB_REG,
    VV_REG,
    REG2A_REG,
    FRARL_REG,
    ADDVSL_REG,
    ADDVSH_REG,
    YAVG_REG,
    REG32_REG,
    ARCOM2_REG,
    REG45_REG,
    FLL_REG,
    FLH_REG,
    COM19_REG,
    ZOOMS_REG,
    COM22_REG,
    COM25_REG,
    BD50_REG,
    BD60_REG,
    REG5D_REG,
    REG5E_REG,
    REG5F_REG,
    REG60_REG,
    HISTO_LOW_REG,
    HISTO_HIGH_REG,
};

size_t size_dsp_bank = sizeof(dsp_bank)/sizeof(dsp_bank[0]); 
size_t size_sensor_bank = sizeof(sensor_bank)/sizeof(sensor_bank[0]);


/* I2C MASTER Setting */
i2c_master_bus_config_t bus_config = {          //I2C MASTER BUS Configuration Structure
    .i2c_port = -1,
    .sda_io_num = CAMERA_PIN_SDA,
    .scl_io_num = CAMERA_PIN_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .intr_priority = 0,
    
};

i2c_device_config_t device_config = {           //I2C MASTER Device Configuration Structure
    .dev_addr_length = I2C_ADDR_BIT_7,
    .device_address = OV2640_I2C_ADDRESS,
    .scl_speed_hz = OV2640_I2C_HZ_FREQ,
};

i2c_master_bus_handle_t i2c_bus_handle;
i2c_master_dev_handle_t i2c_dev_handle;


/* ERROR CHECKS */
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

/* 3-Phase SCCB Transaction to READ value from selected register. (SCCB supposedly don't allow for more)*/
uint8_t sccb_read_register(i2c_master_dev_handle_t i2c_dev_handle, uint8_t register_address)
{
    uint8_t read_buffer[1];
    uint8_t write_buffer[1] = {register_address};
    size_t byte = sizeof(uint8_t);

    esp_err_t err = i2c_master_transmit_receive(i2c_dev_handle, write_buffer, byte, read_buffer, byte, 5);

    if (err != ESP_OK)
    {
        switch (err)
        {
            case ESP_ERR_INVALID_ARG:
            ESP_LOGE(TAG, "I2C Master transmit parameter invalid" );
            break;

            case ESP_ERR_TIMEOUT:
            ESP_LOGE(TAG, "Operation Timeout (Larger than xfer_timeout_ms), because the bus is busy or hardware crash" );
            break;
        }
        return 0;
    }

    return read_buffer[0];
}

/* 3-Phase SCCB Transaction to WRITE value into selected register. (SCCB supposedly don't allow for more)*/
void sccb_write_register(i2c_master_dev_handle_t i2c_dev_handle, uint8_t register_address, uint8_t register_data)
{
    uint8_t write_buffer[2] = {register_address, register_data};
    size_t byte = sizeof(uint8_t);

    esp_err_t err = i2c_master_transmit(i2c_dev_handle, write_buffer, 2*byte, 5);

    if (err != ESP_OK)
    {
        switch (err)
        {
            case ESP_ERR_INVALID_ARG:
            ESP_LOGE(TAG, "I2C Master transmit parameter invalid" );
            break;

            case ESP_ERR_TIMEOUT:
            ESP_LOGE(TAG, "Operation Timeout (Larger than xfer_timeout_ms), because the bus is busy or hardware crash" );
            break;
        }
        return;
    }

}

/*Get 2 bytes register value for identification (use for PID and MID)*/
void get_2_bytes_ID(i2c_master_dev_handle_t i2c_dev_handle, uint8_t start_register)
{
    uint8_t read_data[2];

    read_data[0] = sccb_read_register(i2c_dev_handle, start_register);
    read_data[1] = sccb_read_register(i2c_dev_handle, start_register + 1);

    switch (start_register)
    {
        case PIDH_REG:
        ESP_LOGI(TAG, "Product ID: 0x%02X%02X", read_data[0], read_data[1]);
        break;

        case MIDH_REG:
        ESP_LOGI(TAG, "Manufacturer ID: 0x%02X%02X", read_data[0], read_data[1]);
        break;
    }
    
}

/* Probe I2C line to see if 0V2640 (0x30) is connected, then try to read its PID and MID*/
void test_ov2640_i2c_communication(void)
{
    ESP_LOGI(TAG,"Probing for OV2640 Module at address 0x%02X...", device_config.device_address);
    error_check_device_probe(i2c_master_probe(i2c_bus_handle, device_config.device_address, 5), device_config.device_address);

    /* Reading Data from Sensor. Function Should be optimized. SCCB Protocol prevents transmission cycles of more than 3 phases 
    For Writing: Maximum is Slave Adressing + RegisterAdressing + Write 
    For Reading: Maximum is 2 phases. Slave Adressing + Read. It must be done after a 2-phase or 3-phase write transmission cycle*/

    get_2_bytes_ID(i2c_dev_handle, PIDH_REG); //Confirming I2C can read register by fetching PID
    get_2_bytes_ID(i2c_dev_handle, MIDH_REG); //Confirming I2C can read register by fetching MID
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

    ESP_LOGI(TAG,"Initializing I2C");
    error_check_new_master(i2c_new_master_bus(&bus_config, &i2c_bus_handle));
    error_check_add_device(i2c_master_bus_add_device(i2c_bus_handle, &device_config, &i2c_dev_handle));
    
}

/* Real all registers of OV2640 to get an overview of the config*/
void read_ov2640_register(uint8_t *register_bank, uint8_t bank_select)
{
    uint8_t register_and_data[2] = {RA_DLMT_REG};
    size_t size = 0;

    switch (bank_select)
    {
        case SELECT_DSP_BANK:
        register_and_data[1] = 0x00;
        ESP_LOGI(TAG, "Reading DSP Bank Registers");
        i2c_master_transmit(i2c_dev_handle, register_and_data, (sizeof(uint8_t) + 1), 5); //Register Address to write to + Set Proper Value to 0xFF to access DSP Bank register in one transaction
        size = size_dsp_bank;
        break;

        case SELECT_SENSOR_BANK:
        register_and_data[1] = 0x01;
        ESP_LOGI(TAG, "Reading Sensor Bank Registers");
        i2c_master_transmit(i2c_dev_handle, register_and_data, (sizeof(uint8_t) + 1), 5); //Register Address to write to + Set Proper Value to 0xFF to access Sensor Bank register in one transaction
        size = size_sensor_bank;
        break;

        default:
        ESP_LOGE(TAG, "Can't read register. Invalid register bank parameter");
        break;
    } 

    ESP_LOGI(TAG, "Struct size = %u", size);

    uint8_t read_data[size];
    uint8_t read_buffer[1];

    uint8_t write_buffer[1] = {register_bank[0]};


    for (int i = 0; i < size; i = i+1)
    {
        write_buffer[0] = register_bank[i];
        i2c_master_transmit_receive(i2c_dev_handle, write_buffer, sizeof(uint8_t), read_buffer, sizeof(uint8_t), 5);
        read_data[i] = read_buffer[0];
        ESP_LOGI(TAG, "Register %02X value: %02X", register_bank[i], read_data[i]);
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    read_buffer[0] = sccb_read_register(i2c_dev_handle, RA_DLMT_REG);
    
    ESP_LOGI(TAG, "Bank Select Register Value: %02X", read_buffer[0]);
}

/* Get Automatic Gain Control value */
void ov2640_get_agc_value(void)
{
    uint8_t agc_value = sccb_read_register(i2c_dev_handle, GAIN_REG);
    //ESP_LOGI(TAG, "Automatic Gain Control is set to 0x%02X", agc_value);

    uint8_t bit7 = (agc_value & 0x80) >> 7; //is gain_value bit 7 = 1 or 0? then shift it 7 times to the right to use it as an easier binary value for later calculation
    uint8_t bit6 = (agc_value & 0x40) >> 6;
    uint8_t bit5 = (agc_value & 0x20) >> 5;
    uint8_t bit4 = (agc_value & 0x10) >> 4;
    uint8_t bit3_0 = agc_value & 0x0F;

    float gain = (bit7 + 1) * (bit6 + 1) * (bit5 + 1) * (bit4 + 1) * (1 + (((float)bit3_0)/16)); //maximum value is 16*1.9375 = 31
    int gain_int = (int)(gain * 10000); //To display 4 decimals

    ESP_LOGI(TAG, "Gain Control value: %i.%04i", (gain_int/10000), gain_int % 10000);

}

/*Set Gain Control into manual mode and set a value (and disable Automatic Image Brightness adjustments)*/
void ov2640_set_manual_agc_value(uint8_t gain_value)//gain_value range is 0-255, AGC range is 1-31
{
    uint8_t com8_register_value = sccb_read_register(i2c_dev_handle, COM8_REG);    
    sccb_write_register(i2c_dev_handle, COM8_REG, (com8_register_value &~ 0x04)); //set manual gain control

    com8_register_value = sccb_read_register(i2c_dev_handle, COM8_REG);
    uint8_t com8_agc_bit2 = (com8_register_value & 0x04) >> 1;

    if (com8_agc_bit2 == 0)
    {
        ESP_LOGI(TAG, "Setting AGC value");
        sccb_write_register(i2c_dev_handle, GAIN_REG, gain_value);
        ov2640_get_agc_value();
    }
    else
    {
        ESP_LOGE(TAG, "Manual Gain Control is disabled");
    }    
}

/* Enable Automatic Gain Control */
void ov2640_enable_agc(void)
{
    uint8_t com8_register_value = sccb_read_register(i2c_dev_handle, COM8_REG);
    sccb_write_register(i2c_dev_handle, COM8_REG, (com8_register_value | 0x04)); //set automatic gain control
    vTaskDelay(pdMS_TO_TICKS(200));

    com8_register_value = sccb_read_register(i2c_dev_handle, COM8_REG);
    uint8_t com8_agc_bit2 = (com8_register_value & 0x04) >> 2;

    if (com8_agc_bit2 != 1)
    {
        ESP_LOGE(TAG, "Failed to enable Automatic Gain Control");
    }
    ov2640_get_agc_value();
}

/* Horizontal image flip*/
void ov2640_hflip(uint8_t state)//Flip image Horizontally or unflip. State = ON or OFF
{
    uint8_t reg04_register_value = sccb_read_register(i2c_dev_handle, REG04_REG);
    sccb_write_register(i2c_dev_handle, REG04_REG, ((reg04_register_value & ~REG04_HORIZONTAL_MIRROR_MASK) | ((state << REG04_HORIZONTAL_MIRROR_POS) & REG04_HORIZONTAL_MIRROR_MASK)));

}

/* Vertical Image flip*/
void ov2640_vflip(uint8_t state)//Flip image Vertically or unflip. State = ON or OFF
{
    uint8_t reg04_register_value = sccb_read_register(i2c_dev_handle, REG04_REG);
    sccb_write_register(i2c_dev_handle, REG04_REG, ((reg04_register_value & ~REG04_VERTICAL_FLIP_MASK) | ((state << REG04_VERTICAL_FLIP_POS) & REG04_VERTICAL_FLIP_MASK)));

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
    test_ov2640_i2c_communication();

    read_ov2640_register(dsp_bank, SELECT_DSP_BANK);
    vTaskDelay(pdMS_TO_TICKS(5));
    read_ov2640_register(sensor_bank, SELECT_SENSOR_BANK);
    ov2640_get_agc_value();

    ov2640_hflip(ON);
    vTaskDelay(pdMS_TO_TICKS(5));
    ov2640_hflip(OFF);
}
