#include <avr/io.h>

#ifndef DOGLCD_H_
#define DOGLCD_H_

/* This section defines the ports and pins you connected your DOG LCD
 * display to. Make sure you use the same port (e.g. D) to connect
 * your LCD display - cross-port pins are not supported (yet).
 */
#define LCD_SI			0
#define LCD_CLK 		1
#define LCD_CSB			2
#define LCD_RS			4
#define LCD_PORT 		PORTC	// Port
#define LCD_PORTREG 	DDRC	// LCD Port Register

/* Define the port , register and pin of your backlight control. If it is not
 * connected, uncomment the following three lines. */
#define LCD_BACKLIGHT_PORT PORTB
#define LCD_BACKLIGHT_PORTREG DDRB
#define LCD_BACKLIGHT_PIN DDB1

/* Define the type of your display. */
#define DOGM163
//#define DOGM162
//#define DOGM081

/* Uncomment the following line if you don't want PWM backlight control. */
#define LCD_BACKLIGHT_PWM OCR1A

/* Voltage definitions - don't touch them. */
#define DOG_LCD_VCC_5V 5
#define DOG_LCD_VCC_3V 3

/* Define the voltage of your display. You can power the display with 3.3 and 5V,
 * so you use either DOG_LCD_VCC_5V or DOG_LCD_VCC_3V.
 */
#define LCD_VCC DOG_LCD_VCC_5V


#ifdef DOGM081
	#define LCD_ROWS		1
	#define LCD_COLS		8
	#define	LCD_MEMSIZE		80
	#define LCD_STARTADDR1	0
	#define LCD_STARTADDR2	-1
	#define LCD_STARTADDR3	-1
	#define LCD_INSTRUCTIONSETTEMPLATE 0x30
#endif

#ifdef DOGM162
	#define LCD_ROWS		2
	#define LCD_COLS		16
	#define	LCD_MEMSIZE		40
	#define LCD_STARTADDR1	0
	#define LCD_STARTADDR2	0x40
	#define LCD_STARTADDR3	-1
	#define LCD_INSTRUCTIONSETTEMPLATE 0x38
#endif

#ifdef DOGM163
	#define LCD_ROWS		3
	#define LCD_COLS		16
	#define	LCD_MEMSIZE		16
	#define LCD_STARTADDR1	0
	#define LCD_STARTADDR2	0x10
	#define LCD_STARTADDR3	0x20
	#define LCD_INSTRUCTIONSETTEMPLATE 0x38
#endif

#if ! defined(DOGM081) && !defined(DOGM162) && !defined(DOGM163)
	#error You MUST specify your DOGM model within the source code!
#endif


void lcd_init (void);
void lcd_reset (void);
void lcd_write_display_mode();
void lcd_set_instruction_set (int is);
void lcd_write_command(int value,int executionTime);
void lcd_spi_transfer (int value, int executionTime);
void lcd_set_contrast (int contrast);
void lcd_no_autoscroll(void);
void lcd_autoscroll(void);
void lcd_right_to_left(void);
void lcd_left_to_right(void);
void lcd_scroll_display_left(void);
void lcd_scroll_display_right (void);
void lcd_blink();
void lcd_no_blink();
void lcd_cursor();
void lcd_no_cursor();
void lcd_display();
void lcd_no_display();
void lcd_set_cursor(int col, int row);
void lcd_home();
void lcd_clear();
void lcd_write_char(int value);
void lcd_setbacklight (int value);
void lcd_write_text(const char *str);
#endif
