#ifndef MYCAMERA_H_
#define MYCAMERA_H_

/* Camera Settings */

#define OV2640_I2C_ADDRESS      0x60 //Write Address

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

#define CAMERA_PIN_RESET        12
#define CAMERA_PIN_PWDN         10

#define CAMERA_PIN_VSYNC        11
#define CAMERA_PIN_HREF         9

#define CAMERA_PIN_PCLK         47
#define CAMERA_PIN_MCLK         36

#define CAMERA_PIN_SDA          6
#define CAMERA_PIN_SCL          7

#define CAMERA_PIN_STROBE       8

void CameraComponentTest(void);


#endif