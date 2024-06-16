#ifndef MYCAMERA_H_
#define MYCAMERA_H_

/* Camera Settings */

#define OV2640_I2C_ADDRESS      0x30            //7-bit Address. Write is 0x60 and read is 0x61 
#define OV2640_I2C_HZ_FREQ      100000          //100kHz
#define OV2640_PCLK_HZ_FREQ     36000000        //36MHz
#define OV2640_MCLK_HZ_FREQ     16000000        //24MHz is to much for esp32-S3, max is 20MHz. OV2640 range 6MHz-20MHz. Try Using 16MHz

/* Definition of Camera pins */

#define CAMERA_PIN_D0           39
#define CAMERA_PIN_D1           40
#define CAMERA_PIN_D2           35
#define CAMERA_PIN_D3           37
#define CAMERA_PIN_D4           38
#define CAMERA_PIN_D5           36
#define CAMERA_PIN_D6           48
#define CAMERA_PIN_D7           21
#define CAMERA_PIN_D8           14
#define CAMERA_PIN_D9           13

#define CAMERA_PIN_RESET        12          //Output, No Interrupt, No Pull
#define CAMERA_PIN_PWDN         10          //Output, No Interrupt, No Pull

#define CAMERA_PIN_VSYNC        11
#define CAMERA_PIN_HREF         9

#define CAMERA_PIN_PCLK         47
#define CAMERA_PIN_MCLK         44

#define CAMERA_PIN_SDA          6
#define CAMERA_PIN_SCL          7

#define CAMERA_PIN_STROBE       8

void vTaskStartCamera(void *pvParameters);
void CameraComponentTest(void);


#endif