/****************************************************************
* SH1106 BASED OLED MODULE (128 x 64)
* I2C MODE
*
* ---- ARM VERSION ----
*
* NOTE: THE OLED CONTROLLER SUPPORTS BOTH I2C AND SPI, BUT THE
* 		MODULES BOUGHT USUALLY HARDWIRE TO SELECT A PARTICULAR
* 		MODE
*
* 		THE ORIGINAL ADAFRUIT ONES USE THE SSD1306 CONTROLLER,
* 		BUT THE CHEAPER ONES BOUGHT FROM ALIEXPRESS N ALL USE
* 		THE CHEAPER SH1306 CONTROLLER, WHICH IS SIMILAR BUT
* 		NOT EXACTLY THE SAME TO THIS ONE !
* 		SH1106
* 			(1) DISPLAY BUFFER = 132x64 (THIS IS MAPPED TO 128x64 OF THE OLED)
* 			(2) ONLY SUPPORTS PAGE ADDRESSING MODE (SSD1306 PROVIDES VARIOUS MODES)
* 				1 BYTE WRITES ALL THE ROWS IN A PAGE (8) AND THEN THE CURSOR MOVES
* 				ON TO THE NEXT COLUMN. COLUMN INCREAMENTS ACUTOMATICALLY. HOWEVER
* 				WHEN REACHED THE END OF PAGE, NEED TO SET THE NEXT PAGE MANUALLY
* 			(3) DOES NOT SUPPORT CUSTOM WINDOW WITHIN THE DISPLAY RAM
*
* MAY 29 2017
*
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
*
* REFERENCES
* ------------
*   (1) http://www.picbasic.co.uk/forum/archive/index.php/t-21213.html
*   (2)	https://arduino.stackexchange.com/questions/13975/porting-sh1106-oled-driver-128-x-64-128-x-32-screen-is-garbled-but-partially
*   (3) http://forum.arduino.cc/index.php?topic=265557.0
****************************************************************/

#ifndef _SH1106_I2C_H_
#define _SH1106_I2C_H_

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "FONT_INFO.h"

#include "internal_i2c.h"

#define _sh1106_i2c_backend_init			INTERNAL_I2C_Init
#define	_sh1106_i2c_send_start_function		INTERNAL_I2C_SendStart
#define _sh1106_i2c_send_stop_function		INTERNAL_I2C_SendStop
#define _sh1106_i2c_send_byte_function		INTERNAL_I2C_SendByte
#define debug_printf(...) 					

//I2C ADDRESS
#define SH1106_I2C_ADDRESS_1						0x3C
#define SH1106_I2C_ADDRESS_2						0x3D

//SCREEN PIXEL SIZE
#define SH1106_I2C_OLED_MAX_COLUMN					127u
#define SH1106_I2C_OLED_MAX_PAGE					7u

//CONTROL BYTES
#define SH1106_I2C_CONTROL_BYTE_CMD_SINGLE			0x80
#define SH1106_I2C_CONTROL_BYTE_CMD_STREAM			0x00
#define SH1106_I2C_CONTROL_BYTE_DATA_SINGLE			0xC0
#define SH1106_I2C_CONTROL_BYTE_DATA_STREAM			0x40

//FUNDAMENTAL COMMANDS (DATASHEET PG 29)
#define SH1106_I2C_CMD_SET_COLUMN_LOWER_4			0x00
#define SH1106_I2C_CMD_SET_COLUMN_UPPER_4			0x10
#define SH1106_I2C_CMD_SET_CHARGE_PUMP_VOLTAGE		0x30
#define SH1106_I2C_CMD_SET_DISPLAY_START_LINE		0x40
#define SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE	0x81
#define SH1106_I2C_CMD_SET_SEGMENT_REMAP			0xA0
#define SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_OFF		0xA5
#define SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_ON		0xA4
#define SH1106_I2C_CMD_SET_DISPLAY_NORMAL			0xA6
#define SH1106_I2C_CMD_SET_DISPLAY_REVERSED			0xA7
#define SH1106_I2C_CMD_SET_MULTIPLEX_RATIO			0xA8
#define SH1106_I2C_CMD_SET_DC_DC_MODE				0xAD
#define SH1106_I2C_CMD_SET_DISPLAY_ON				0xAF
#define SH1106_I2C_CMD_SET_DISPLAY_OFF				0xAE
#define SH1106_I2C_CMD_SET_PAGE_ADDRESS				0xB0
#define SH1106_I2C_CMD_SET_COMMON_SCAN_DIRECTION	0xC0
#define SH1106_I2C_CMD_SET_DISPLAY_OFFSET_MODE		0xD3
#define SH1106_I2C_CMD_SET_OSCILLATOR_FREQUENCY		0xD5
#define SH1106_I2C_CMD_SET_DISCHARGE_PRECHARGE		0xD9
#define SH1106_I2C_CMD_COMMON_PADS_HARDWARE_CONFIG	0xDA
#define SH1106_I2C_CMD_COMMON_PADS_OUTPUT_VOLTAGE	0xDB
#define SH1106_I2C_CMD_SET_READ_MODIFY_WRITE		0xE0
#define SH1106_I2C_CMD_SET_READ_MODIFY_WRITE_END	0xEE
#define SH1106_I2C_CMD_NOP							0xE3

#define SH1106_I2C_SCREEN_FILL_PATTERN_CLEAR		0x00
#define SH1106_I2C_SCREEN_FILL_PATTERN_FILL			0xFF

//FUNCTION PROTOTYPES/////////////////////////////////////
//CONFIGURATION FUNCTIONS
void  SH1106_I2C_SetDebug(uint8_t debug_on);
void  SH1106_I2C_SetDeviceAddress(uint8_t address);
void  SH1106_I2C_Init(void);

//CONTROL FUNCTIONS
void  SH1106_I2C_SetDisplayOnOff(uint8_t on);
void  SH1106_I2C_SetDisplayContrast(uint8_t contrast_val);
void  SH1106_I2C_SetDisplayNormal(void);
void  SH1106_I2C_SetDisplayInverted(void);
void  SH1106_I2C_ResetAndClearScreen(const uint8_t* fill_pattern, uint8_t len);
void  SH1106_I2C_UpdateDisplay(void);

//DRAWING FUNCTIONS
void  SH1106_I2C_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void  SH1106_I2C_DrawLineVertical(uint8_t x, uint8_t y_start, uint8_t y_end, uint8_t color);
void  SH1106_I2C_DrawLineHorizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t color);
void  SH1106_I2C_DrawBoxEmpty(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color);
void  SH1106_I2C_DrawBoxFilled(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color);
void  SH1106_I2C_DrawCircleEmpty(int8_t x, int8_t y, int8_t radius, uint8_t color);
void  SH1106_I2C_DrawCircleFilled(int8_t x, int8_t y, int8_t radius, uint8_t color);

//COMPLEX DRAWING FUNCTIONS
void  SH1106_I2C_DrawString(char* str, uint8_t x, uint8_t y, const FONT_INFO font, uint8_t color);
void  SH1106_I2C_DrawBitmap(uint8_t* bitmap, uint8_t x, uint8_t y, uint8_t x_len_bits, uint8_t y_len_bits, uint8_t color);
//END FUNCTION PROTOTYPES/////////////////////////////////
#endif
