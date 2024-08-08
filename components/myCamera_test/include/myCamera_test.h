#ifndef MYCAMERA_H_
#define MYCAMERA_H_

/* Camera Settings */

#define OV2640_I2C_ADDRESS      0x30                    //7-bit Address. Write is 0x60 and read is 0x61 
#define OV2640_I2C_HZ_FREQ      100000                  //100kHz
#define OV2640_MCLK_HZ_FREQ     16000000                //24MHz is too much for esp32-S3, max is 20MHz. OV2640 range 6MHz-20MHz. Try Using 16MHz
#define PIXEL_PERIOD            62.5                    //62.5ns
#define TLINE                   (1922 * PIXEL_PERIOD)

#define UXGA_MODE               0
#define CIF_MODE                2
#define SVGA_MODE               4

#define YUV422_OUTPUT           0
#define RAW10_OUTPUT            1
#define RGB565_OUTPUT           2

#define RES_COLUMNS             1600
#define RES_ROWS                1200

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

/* Bit Position inside REG04_REG */
#define REG04_REG_HORIZONTAL_MIRROR_POS     7 //Bit 7: Horizontal Mirror
#define REG04_REG_VERTICAL_FLIP_POS         6 //Bit 6: Vertical Flip
#define REG04_REG_VREF_BIT0_POS             4 //Bit 4: VREF bit 0
#define REG04_REG_HREF_BIT0_POS             3 //Bit 3: HREF bit 0
#define REG04_REG_AEC_BIT1_0_POS            0 //Bits [1:0] : AEC bits [1:0]

/* BitMask for REG04_REG bits */
#define REG04_REG_HORIZONTAL_MIRROR_MASK     (1UL << REG04_REG_HORIZONTAL_MIRROR_POS)
#define REG04_REG_VERTICAL_FLIP_MASK         (1UL << REG04_REG_VERTICAL_FLIP_POS)
#define REG04_REG_VREF_BIT0_MASK             (1UL << REG04_REG_VREF_BIT0_POS)
#define REG04_REG_HREF_BIT0_MASK             (1UL << REG04_REG_HREF_BIT0_POS)
#define REG04_REG_AEC_BIT1_0_MASK            (0x03 << REG04_REG_AEC_BIT1_0_POS)

/* Bit Position inside AEC_REG */
#define AEC_REG_AEC_BIT9_2_POS            0 //Bits [7:0] : AEC bits [9:2]

/* BitMask for AEC_REG bits */
#define AEC_REG_AEC_BIT9_2_MASK           (0xFF << AEC_REG_AEC_BIT9_2_POS)

/* Bit Position inside CLKRC_REG */
#define CLKRC_REG_INTERNAL_PLL_SEL_POS         7 //Bit 7: Internal Frequency doubler (ON/OFF)
#define CLKRC_REG_CLK_DIVIDER_POS              0 //Bit 0: Clock Divider

/* BitMask for CLKRC_REG bits */
#define CLKRC_REG_INTERNAL_PLL_SEL_MASK       (1UL << CLKRC_REG_INTERNAL_PLL_SEL_POS)
#define CLKRC_REG_CLK_DIVIDER_MASK            (0x3F << CLKRC_REG_CLK_DIVIDER_POS)

/* Bit Position inside COM7_REG */
#define COM7_REG_SRST_POS                       7 //Bit 7: Software Reset
#define COM7_REG_RESOLUTION_SEL_POS             4 //Bits [6:4]: UXGA/CIF/SVGA Resolution selection
#define COM7_REG_ZOOM_MODE_POS                  2 //Bit 2: Zoom Mode
#define COM7_REG_COLOR_BAR_TEST_SEL_POS         1 //Bit 1: Color bar test pattern ON/OFF

/* BitMask for COM7_REG bits */
#define COM7_REG_SRST_MASK                      (1UL << COM7_REG_SRST_POS)
#define COM7_REG_RESOLUTION_SEL_MASK            (0x07 << COM7_REG_RESOLUTION_SEL_POS)
#define COM7_REG_ZOOM_MODE_MASK                 (1UL << COM7_REG_ZOOM_MODE_POS)
#define COM7_REG_COLOR_BAR_TEST_SEL_MASK        (1UL << COM7_REG_COLOR_BAR_TEST_SEL_POS)

/* Bit Position inside COM8_REG */
#define COM8_REG_BANDING_FILTER_SEL_POS            5 //Bit 5: Banding Filter
#define COM8_REG_AUTO_MANUAL_GAIN_CTRL_POS         2 //Bit 2: Automatic Gain Control (AGC) ON/OFF
#define COM8_REG_AUTO_MANUAL_EXPOSURE_CTRL_POS     0 //Bit 0: Automatic Exposure Controle (AEC) ON/OFF

/* BitMask for COM8_REG bits */
#define COM8_REG_BANDING_FILTER_SEL_MASK            (1UL << COM8_REG_BANDING_FILTER_SEL_POS)
#define COM8_REG_AUTO_MANUAL_GAIN_CTRL_MASK         (1UL << COM8_REG_AUTO_MANUAL_GAIN_CTRL_POS)
#define COM8_REG_AUTO_MANUAL_EXPOSURE_CTRL_MASK     (1UL << COM8_REG_AUTO_MANUAL_EXPOSURE_CTRL_POS)


/* Bit Position inside REG45_REG */
#define REG45_REG_AGC_BIT9_8_POS           6 //Bits [7:6] : AGC bits [9:8]
#define REG45_REG_AEC_BIT15_10_POS         0 //Bits [5:0] : AEC bits [15:10]

/* BitMask for REG45_REG bits */
#define REG45_REG_AGC_BIT9_8_MASK          (0x03 << REG45_REG_AGC_BIT9_8_POS)
#define REG45_REG_AEC_BIT15_10_MASK        (0x3F << REG45_REG_AEC_BIT15_10_POS)


/* Bit Position inside IMAGE_MODE_REG */
#define IMAGE_MODE_REG_Y8_EN_POS            6 //Bit 6 : Y8 Enable
#define IMAGE_MODE_REG_JPEG_EN_POS          4 //Bit 4 : Enable/Disable JPEG Output
#define IMAGE_MODE_REG_OUTPUT_FORMAT_POS    2 //Bits [3:2] : DVP Output Format (YUV422/RAW10/GBG565)
#define IMAGE_MODE_REG_HREF_TIMING_POS      1 //Bit 1 : HREF Timing Select (in DVP JPEG Output mode)
#define IMAGE_MODE_REG_DVP_BYTE_SWAP_POS    0 //Bit 0 : Byte swap enable for DVP


/* Bitmask for IMAGE_MODE_REG */
#define IMAGE_MODE_REG_Y8_EN_MASK           (1UL << IMAGE_MODE_REG_Y8_EN_POS)
#define IMAGE_MODE_REG_JPEG_EN_MASK         (1UL << IMAGE_MODE_REG_JPEG_EN_POS)
#define IMAGE_MODE_REG_OUTPUT_FORMAT_MASK   (0x03 << IMAGE_MODE_REG_OUTPUT_FORMAT_POS)
#define IMAGE_MODE_REG_HREF_TIMING_MASK     (1UL << IMAGE_MODE_REG_HREF_TIMING_POS)
#define IMAGE_MODE_REG_DVP_BYTE_SWAP_MASK   (1UL << IMAGE_MODE_REG_DVP_BYTE_SWAP_POS)


void vTaskStartCamera(void *pvParameters);

/* Horizontal image flip*/
void ov2640_hflip(uint8_t state);

/* Vertical Image flip*/
void ov2640_vflip(uint8_t state);

/* Get Automatic Gain Control value */
void ov2640_get_agc_value(void);

/*Set Gain Control into manual mode and set a value (and disable Automatic Image Brightness adjustments)*/
void ov2640_set_manual_agc_value(uint8_t gain_value);//gain_value range is 0-255, AGC range is 1-31

/* Set Automatic Gain Control */
void ov2640_enable_agc(void);

/* Get Exposure Time */
void ov2640_get_exposure_time(void);//Get Exposure Control Value. tp = 27.8ns, Tline = 1922tp. Tex = Tline * AEC[15:0]

/* Set Exposure Time. If Tex > 1 frame period, maximum exposure time is 1 frame period, even if TEX > 1 frame period (OV2640 Behavior) */
void ov2640_set_manual_exposure_time(uint16_t aec_value);//set Exposure Time. 1/PCLK = 62.5ns, Tline = 1922tp. Tex = Tline * AEC[15:0]

/* Enable Automatic Exposure Control */
void ov2640_enable_aec(void);



void CameraComponentTest(void);


#endif