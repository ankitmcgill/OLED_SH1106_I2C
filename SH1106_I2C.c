/****************************************************************
* SH1106 BASED OLED MODULE (128 x 64)
* I2C MODE
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
	* 			1 BYTE WRITES ALL THE ROWS IN A PAGE (8) AND THEN THE CURSOR MOVES
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

#include "SH1106_I2C.h"

//LOCAL LIBRARY VARIABLES////////////////////////////////
//DEBUG RELATED
static uint8_t _sh1106_i2c_debug;

//DEVICE RELATED
static uint8_t _sh1106_i2c_slave_address;
static uint8_t* _sh1106_framebuffer_pointer;
//END LOCAL LIBRARY VARIABLES/////////////////////////////

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDebug(uint8_t debug_on)
{
	//SET DEBUG PRINTF ON(1) OR OFF(0)

	_sh1106_i2c_debug = debug_on;
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDeviceAddress(uint8_t address)
{
	//SET THE I2C MODULE SLAVE ADDRESS
	//NOTE THIS IS THE 7 BIT ADDRESS (WITHOUT THE R/W BIT)

	_sh1106_i2c_slave_address = address;

	//INITIALIZE BACKEND I2C
	_sh1106_i2c_backend_init(_sh1106_i2c_slave_address);

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : I2C : Address set to %d\n", _sh1106_i2c_slave_address);
		debug_printf("SH1106 : I2C : Initialized backend I2C\n", _sh1106_i2c_slave_address);
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_Init(void)
{
	//INITIALIZE THE OLED MODULE AS PER THE DEFAULT PARAMETERS

	//INITIALIZE DISPLAY FRAMBUFFER
	_sh1106_framebuffer_pointer = (uint8_t*)os_zalloc((SH1106_I2C_OLED_MAX_COLUMN + 1) * (SH1106_I2C_OLED_MAX_PAGE + 1));

	//INITIALIZE THE DISPLAY
	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	//DISPLAY OFF
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFF);

	//SET COLUMN ADDRESS LOWER (2)
	//BECAUSE THIS CONTROLLER HAS RAM SIZE 132 X 64 WHEREAS OUR DISPLAY
	//IS 128 X 4. SO THE OLED IS MAPPED FROM RAM COLUMN 2 - COLUMN 130
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_LOWER_4 | 2);
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_UPPER_4 | 0);

	//SET START PAGE ADDRESS = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_PAGE_ADDRESS | 0);

	//SET COMMON OUTPUT SCAN DIRECTION = TOP -> BOTTOM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COMMON_SCAN_DIRECTION | 8);

	//_sh1106_i2c_send_byte_function(0x00);
	//_sh1106_i2c_send_byte_function(0x10);

	//SET DISPLAY START LINE = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_START_LINE | 0);

	//SET CONTRAST
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE);
	_sh1106_i2c_send_byte_function(0x7F);

	//SET SEGMENT REMAP
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_SEGMENT_REMAP | 1);

	//SET DISPLAY = NORMAL
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_NORMAL);

	//SET MULTIPLEX RATIO = 63 (ALL ROWS)
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_MULTIPLEX_RATIO);
	_sh1106_i2c_send_byte_function(0x3F);

	//SET ENTIRE DISPLAY = ON
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_ON);

	//SET DISPLAY OFFSET = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFFSET_MODE);
	_sh1106_i2c_send_byte_function(0x00);

	//SET DISPLAY OSCILLATOR FREQUENCY
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_OSCILLATOR_FREQUENCY);
	_sh1106_i2c_send_byte_function(0xF0);

	//SET DISCHARGE-PRECHARGE PERIOD
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISCHARGE_PRECHARGE);
	_sh1106_i2c_send_byte_function(0x22);

	//SET COMMON PADS HARDWARE CONFIG
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_COMMON_PADS_HARDWARE_CONFIG);
	_sh1106_i2c_send_byte_function(0x12);

	//SET COMMON PAD OUTPUT VOLTAGE
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_COMMON_PADS_OUTPUT_VOLTAGE);
	_sh1106_i2c_send_byte_function(0x20);

	_sh1106_i2c_send_byte_function(0x8D);
	_sh1106_i2c_send_byte_function(0x14);

	//SET DISPLAY ON
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_ON);

	_sh1106_i2c_send_stop_function();

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Frame buffer allocated\n");
		debug_printf("SH1106 : Display initialized\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayOnOff(uint8_t on)
{
	//TURN THE DISPLAY ON/OFF DEPENDING ON INPUT ARGUENT VALUE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	if(on)
	{
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_ON);
		if(_sh1106_i2c_debug)
		{
			debug_printf("SH1106 : Display turned ON\n");
		}
	}
	else
	{
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFF);
		if(_sh1106_i2c_debug)
		{
			debug_printf("SH1106 : Display turned OFF\n");
		}
	}
	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayContrast(uint8_t contrast_val)
{
	//SET THE CONTRAST OF THE DISPLAY (0 - 255)
	//HIGHER THE CONTRAST, HIGHER THE DISPLAY CURRENT CONSUMPTION

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE);
	_sh1106_i2c_send_byte_function(contrast_val);

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Contrast set to : %u\n", contrast_val);
	}
	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayNormal(void)
{
	//SET DISPLAY TO NORMAL MODE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_NORMAL);
	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display = Normal\n");
	}

	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayInverted(void)
{
	//SET DISPLAY TO INVERTED MODE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_REVERSED);
	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display = Inverted\n");
	}

	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_ResetAndClearScreen(const uint8_t* fill_pattern, uint8_t pattern_len)
{
	//RESET THE CURSOR TO COLUMN 0, PAGE 0
	//CLEAR THE SCREEN AND FILL WITH THE SPECIFIED PATTERN OF SPECIFIED BYTES (len)
	//PATTER WIDTH TO BE ASSUMED = 1 (8 BITS)
	//NOTE SINCE CLEAR SCREEN AND FILL WITH PATTERN WORKS AT THE MOST BASIC LEVEL, WE NEED TO BOTH SET AND CLEAR
	//THE PIXELS

	uint16_t x_offset = 0;
	uint16_t y_offset = 0;
	uint8_t byte;
	int16_t byte_counter;
	uint8_t i;
	uint8_t color;

	byte_counter = -1;

	while(y_offset < ((SH1106_I2C_OLED_MAX_PAGE + 1) * 8))
	{
		byte_counter++;
		if(byte_counter == pattern_len)
		{
			byte_counter = 0;
		}
		byte = fill_pattern[byte_counter];

		while(x_offset < 8)
		{
			//DRAW THE PIXEL IF REQUIRED
			if(byte & 0x80)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x40)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x20)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x10)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x08)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x04)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x02)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;

			if(byte & 0x01)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			for(i = 0; i < 16; i++)
			{
				SH1106_I2C_DrawPixel(x_offset + (8 * i), y_offset, color);
			}
			x_offset++;
		}
		//RESET x_offset
		x_offset = 0;
		y_offset++;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Screen reseted and cleared with specified pattern\n");
	}

}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_UpdateDisplay(void)
{
	//TRANSFER THE FRAMEBUFFER TO THE DISPLAY IN BULK

	uint16_t counter = 0;
	uint16_t x = 0;
	uint16_t y = 0;

	for(y = 0; y < (SH1106_I2C_OLED_MAX_PAGE + 1); y++)
	{
		//SET CURSOR TO THE BEGINNING OF THE CURRENT PAGE
		_sh1106_i2c_send_start_function();
		_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));
		_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

		//SET COLUMN TO 2
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_UPPER_4);
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_LOWER_4 | 0);

		//SET PAGE
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_PAGE_ADDRESS | y);
		_sh1106_i2c_send_stop_function();

		//SEND PAGE DATA
		_sh1106_i2c_send_start_function();
		_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));
		_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_DATA_STREAM);

		for(x = 0; x < (SH1106_I2C_OLED_MAX_COLUMN + 1); x++)
		{
			_sh1106_i2c_send_byte_function(_sh1106_framebuffer_pointer[counter]);
			counter++;
		}
		_sh1106_i2c_send_stop_function();
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display updated with frame buffer\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	//SET OR UNSET A PIXEL AT THE SPECIFIED X,Y LOCATION

	if((x > SH1106_I2C_OLED_MAX_COLUMN) || (y > (SH1106_I2C_OLED_MAX_PAGE * 8 + 7)))
	{
		//PIXEL OUT OF RANGE
		if(_sh1106_i2c_debug)
		{
			debug_printf("SH1106 : Drawpixel out of range\n");
		}
		return;
	}

	//PIXEL IN RANGE
	if(!color)
	{
		//CLEAR PIXEL
		_sh1106_framebuffer_pointer[((y/8) * 128) + x] &= ~(1 << (y%8));
	}
	else
	{
		//DRAW PIXEL
		_sh1106_framebuffer_pointer[((y/8) * 128) + x] |= (1 << (y%8));
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawLineVertical(uint8_t x, uint8_t y_start, uint8_t y_end, uint8_t color)
{
	//DRAW A VERTICAL LINE

	uint8_t counter = y_start;
	while(counter <= y_end)
	{
		SH1106_I2C_DrawPixel(x, counter, color);
		counter += 1;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawline vertical\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawLineHorizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t color)
{
	//DRAW A HORIZONTAL LINE

	uint8_t counter = x_start;
	while(counter <= x_end)
	{
		SH1106_I2C_DrawPixel(counter, y, color);
		counter += 1;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawline horizontal\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawBoxEmpty(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color)
{
	//DRAW EMPTY RECTANGLE BETWEEN THE SPECIFIED COORDINATES

	SH1106_I2C_DrawLineHorizontal(x_start, x_end, y_start, color);
	SH1106_I2C_DrawLineHorizontal(x_start, x_end, y_end, color);
	SH1106_I2C_DrawLineVertical(x_start, y_start, y_end, color);
	SH1106_I2C_DrawLineVertical(x_end, y_start, y_end, color);

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawbox empty\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawBoxFilled(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color)
{
	//DRAW FILLED RECTANGLE BETWEEN THE SPECIFIED COORDINATES

	uint8_t counter = y_start;

	while(counter <= y_end)
	{
		SH1106_I2C_DrawLineHorizontal(x_start, x_end, counter, color);
		counter += 1;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawbox filled\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawCircleEmpty(int8_t x, int8_t y, int8_t radius, uint8_t color)
{
	//DRAW CIRCLE OUTLINE AT SPECIFIED CORDINATE WITH SPECIFIED RADIUS

	int16_t curr_x, curr_y;
	int16_t compare = radius * radius;
	int16_t compare_1 = (radius + 1) * (radius + 1);

	for(curr_y = -radius; curr_y <= radius; curr_y++)
	{
		for(curr_x = -radius; curr_x <= radius; curr_x++)
		{
			if(((curr_x * curr_x) + (curr_y * curr_y)) < compare)
			{
				//DO NOTHING
			}
			else if(((curr_x * curr_x) + (curr_y * curr_y)) < compare_1)
			{
				SH1106_I2C_DrawPixel(x + curr_x, y + curr_y, color);
			}
		}
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawcircle empty\n");
	}
}
void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawCircleFilled(int8_t x, int8_t y, int8_t radius, uint8_t color)
{
	int16_t curr_x, curr_y;
	int16_t compare = radius * radius;

	for(curr_y = -radius; curr_y <= radius; curr_y++)
	{
		for(curr_x = -radius; curr_x <= radius; curr_x++)
		{
			if(((curr_x * curr_x) + (curr_y * curr_y)) <= compare)
			{
				SH1106_I2C_DrawPixel(x + curr_x, y + curr_y, color);
			}
		}
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Drawcircle filled\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawString(char* str, uint8_t x, uint8_t y, const FONT_INFO font, uint8_t color)
{
	//DRAW THE SPECIFIED TEXT STRING AT THE GIVEN LOCATION WITH THE SPECIFIED FONT AND COLOR

	uint16_t counter_char;
	uint16_t x_offset = x;
	uint16_t y_offset = y;
	uint16_t length = strlen(str);

	os_printf("string len = %u\n", length);

	uint16_t i;

	//GET FONT HEGHT
	//ONLY FONTS WITH COMMON HEIGHT SUPPORTED
	uint16_t font_height_bits = font.font_char_descriptors[0][1];
	uint16_t char_width_bytes;
	uint16_t current_char;

	for(counter_char = 0; counter_char < length; counter_char++)
	{
		y_offset = y;
		os_printf("current char = %c", str[counter_char]);
		os_printf(" pos = %u,%u\n", x_offset, y_offset);

		current_char = str[counter_char];

		if(current_char < font.start_char || current_char > font.end_char)
		{
			//CHARACTER IS NOT SUPPORTED BY SPECIFIED FONT
			//DRAW A BLOCK 8 BITS WIDE AND HEIGHT OF THE FONT IN CHARACTERS PLACE

			while(y_offset < font_height_bits)
			{
				SH1106_I2C_DrawLineHorizontal(x_offset, x_offset + 7, y_offset, color);
				y_offset += 1;
			}
			x_offset += 8;
		}
		else
		{
			//CHARACTER SUPPORTED BY THE FONT

			char_width_bytes = (font.font_char_descriptors[current_char - font.start_char][0]);

			while(y_offset < (y + font_height_bits))
			{
				for(i = 0; i < char_width_bytes; i++)
				{
					uint8_t byte = font.font_bitmap[font.font_char_descriptors[current_char - font.start_char][2] + (char_width_bytes * (y_offset - y)) + i];

					if(byte & 0x80)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x40)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x20)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x10)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x08)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x04)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x02)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
					if(byte & 0x01)
					{
						SH1106_I2C_DrawPixel(x_offset, y_offset, color);
					}
					x_offset++;
				}
				x_offset -= (char_width_bytes * 8);
				y_offset++;
			}
		}
		x_offset += (char_width_bytes * 8);
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : String written\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_DrawBitmap(uint8_t* bitmap, uint8_t x, uint8_t y, uint8_t x_len_bits, uint8_t y_len_bits, uint8_t color)
{
	//DRAW BITMAP OF THE SPECIFIED DIMENSIONS AT SPECIFIED X,Y CORDINATES IN THE SPECIFIED COLOR
	//BITMAP NEEDS TO BE IN ROW MAJOR FORMAT AND DIMENSIONS NEED TO BE MULTIPLE OF 8 BITS
	//WITH BITMAP WE WILL DO BOTH PIXEL SETTING AND CLEARNING

	uint16_t x_offset = x;
	uint16_t y_offset = y;
	uint8_t byte;
	uint8_t not_color;

	uint16_t row_size_bytes = (x_len_bits / 8);

	if(color == 0)
	{
		not_color = 1;
	}
	else
	{
		not_color = 0;
	}

	while(y_offset < (y + y_len_bits))
	{
		while(x_offset < (x + x_len_bits))
		{
			//GET A NEW BYTE FROM BUFFER IF BITS TRAVERSED IN CURRENT ROW EXCEEDS 8
			if(((x_offset - x) % 8) == 0)
			{
				byte = bitmap[(row_size_bytes * (y_offset - y)) + ((x_offset - x) / 8)];
				os_printf("byte = %x\n",byte);
			}

			//DRAW THE PIXEL IF REQUIRED
			if(byte & 0x80)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x40)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x20)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x10)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x08)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x04)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x02)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
			if(byte & 0x01)
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, color);
			}
			else
			{
				SH1106_I2C_DrawPixel(x_offset, y_offset, not_color);
			}
			x_offset++;
		}
		//RESET x_offset
		x_offset = x;
		y_offset++;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Bitmap written of size %u bits\n", (x_len_bits * y_len_bits));
	}
}
