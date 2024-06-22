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

/*OV2640 Registers When 0xFF = 00*/

#define R_BYPASS_REG            0x05
#define Q_SCALE_REG             0x44
#define CTRLI_REG               0x50
#define HSIZE_REG               0x51
#define VSIZE_REG               0x52
#define XOFFL_REG               0x53
#define YOFFL_REG               0x54
#define VHYX_REG                0x55
#define DPRP_REG                0x56
#define TEST_REG                0x57
#define ZMOW_REG                0x5A
#define ZMOH_REG                0x5B
#define ZMHH_REG                0x5C
#define BPADDR_REG              0x7C
#define BPDATA_REG              0x7D
#define CTRL2_REG               0x86
#define CTRL3_REG               0x87
#define SIZEL_REG               0x8C
#define HSIZE8_REG              0xC0
#define VSIZE8_REG              0xC1
#define CTRL0_REG               0xC2
#define CTRL1_REG               0xC3
#define R_DVP_SP_REG            0xD3
#define IMAGE_MODE_REG          0xDA
#define RESET_REG               0xE0
#define REGED_REG               0xED
#define MS_SP_REG               0xF0
#define SS_ID_REG               0xF7
#define SS_CTRL_REG             0xF8
#define MC_BIST_REG             0xF9
#define MC_AL_REG               0xFA
#define MC_AH_REG               0xFB
#define MC_D_REG                0xFC
#define P_CMD_REG               0xFD
#define P_STATUS_REG            0xFE
#define RA_DLMT_REG             0xFF

/*OV2640 Registers When 0xFF = 01*/

#define GAIN_REG                0x00
#define COM1_REG                0x03
#define REG04_REG               0x04
#define REG08_REG               0x08
#define COM2_REG                0x09
#define PIDH_REG                0x0A
#define PIDL_REG                0x0B
#define COM3_REG                0x0C
#define AEC_REG                 0x10
#define CLKRC_REG               0x11
#define COM7_REG                0x12
#define COM8_REG                0x13
#define COM9_REG                0x14
#define COM10_REG               0x15
#define HREFST_REG              0x17
#define HREFEND_REG             0x18
#define VSTRT_REG               0x19
#define VEND_REG                0x1A
#define MIDH_REG                0x1C
#define MIDL_REG                0x1D
#define AEW_REG                 0x24
#define AEB_REG                 0x25
#define VV_REG                  0x26
#define REG2A_REG               0x2A
#define FRARL_REG               0x2B
#define ADDVSL_REG              0x2D
#define ADDVSH_REG              0x2E
#define YAVG_REG                0x2F
#define REG32_REG               0x32
#define ARCOM2_REG              0x34
#define REG45_REG               0x45
#define FLL_REG                 0x46
#define FLH_REG                 0x47
#define COM19_REG               0x48
#define ZOOMS_REG               0x49
#define COM22_REG               0x4B
#define COM25_REG               0x4E
#define BD50_REG                0x4F
#define BD60_REG                0x50
#define REG5D_REG               0x5D
#define REG5E_REG               0x5E
#define REG5F_REG               0x5F
#define REG60_REG               0x60
#define HISTO_LOW_REG           0x61
#define HISTO_HIGH_REG          0x62

/* Bit Position insie REG04_REG */
#define REG04_HORIZONTAL_MIRROR_POS     7 //Bit 7: Horizontal Mirror
#define REG04_VERTICAL_FLIP_POS         6 //Bit 6: Vertical Flip
#define REG04_VREF_BIT0_POS             4 //Bit 4: VREF bit 0
#define REG04_HREF_BIT0_POS             3 //Bit 3: HREF bit 0
#define REG04_AEC_BIT0_1_POS            0 //Bits [1:0] : AEC bits [1:0]

/* BitMask for REG04_REG bits */
#define REG04_HORIZONTAL_MIRROR_MASK     (1UL << REG04_HORIZONTAL_MIRROR_POS)
#define REG04_VERTICAL_FLIP_MASK         (1UL << REG04_VERTICAL_FLIP_POS)
#define REG04_VREF_BIT0_MASK             (1UL << REG04_VREF_BIT0_POS)
#define REG04_HREF_BIT0_MASK             (1UL << REG04_HREF_BIT0_POS)
#define REG04_AEC_BIT0_1_MASK            (0x03 << REG04_AEC_BIT0_1_POS)




void vTaskStartCamera(void *pvParameters);

/* Get Automatic Gain Control value */
void ov2640_get_agc_value(void);

/*Set Gain Control into manual mode and set a value (and disable Automatic Image Brightness adjustments)*/
void ov2640_set_manual_agc_value(uint8_t gain_value);//gain_value range is 0-255, AGC range is 1-31

/* Set Automatic Gain Control */
void ov2640_enable_agc(void);

void CameraComponentTest(void);


#endif