#include "doglcd.h"
#include <avr/wdt.h>
#include <util/delay.h>

int contrast;
int displayMode;
int cursorMode;
int blinkMode;
int entryMode;

void lcd_setbacklight (int value) {
#if !defined(LCD_BACKLIGHT_PIN) && !defined(LCD_BACKLIGHT_PORT) && !defined(LCD_BACKLIGHT_PWM)
	return;
#endif

#ifdef LCD_BACKLIGHT_PWM
	OCR1A = value;
#else
	if (value > 0) {
		LCD_BACKLIGHT_PORT |= (1 << LCD_BACKLIGHT_PIN);
	} else {
		LCD_BACKLIGHT_PORT &= ~(1 << LCD_BACKLIGHT_PIN);
	}
#endif


}
void lcd_init (void) {

	// Set LCD ports to output
	LCD_PORTREG |= 1<<LCD_SI;
	LCD_PORT |= (1 << LCD_SI);

	LCD_PORTREG |= 1<<LCD_CLK;
	LCD_PORT |= (1 << LCD_CLK);

	LCD_PORTREG |= 1<<LCD_CSB;
	LCD_PORT |= (1 << LCD_CSB);

	LCD_PORTREG |= 1<<LCD_RS;
	LCD_PORT |= (1 << LCD_RS);

#if defined(LCD_BACKLIGHT_OWM) && !defined(LCD_BACKLIGHT_PIN) && !defined(LCD_BACKLIGHT_PORT)
#error If you wish to use PWM, you need to define the port and pin of your PWM output!
#endif

#ifdef LCD_BACKLIGHT_PIN
#if !defined(LCD_BACKLIGHT_PORT) && !defined(LCD_BACKLIGHT_PORT_REG)
#error If you specify a backlight pin, you also need to set LCD_BACKLIGHT_PORT
#endif
	LCD_BACKLIGHT_PORTREG |= (1 << LCD_BACKLIGHT_PIN);
#endif
	contrast = 0x20;

#ifdef LCD_BACKLIGHT_PWM
	TCCR1A = (1<<WGM10)|(1<<COM1A1);
	TCCR1B = (1<<CS10);
#endif

	lcd_reset();

}

void lcd_reset (void) {
	_delay_ms(50);
	//init data-size and lines, then change to instructionset 1
	 lcd_set_instruction_set(1);
	 lcd_write_command(0x1D,30);
	 lcd_set_contrast(contrast);

	 displayMode=0x04;
	 cursorMode=0x00;
	 blinkMode=0x00;
	 lcd_write_display_mode();
	 entryMode=0x04;
	 lcd_clear();
	 lcd_left_to_right();


}

void lcd_write_char(int value) {
	LCD_PORT |= (1 << LCD_RS);
    lcd_spi_transfer(value,2);
}

void lcd_clear() {
    lcd_write_command(0x01,1);
}

void lcd_home() {
    lcd_write_command(0x02,1);
}

void lcd_set_cursor(int col, int row) {
	int address = 0;

    if(col>=LCD_MEMSIZE || row>=LCD_ROWS) {
	//not a valid cursor position
	return;
    }

    switch (row) {
    	case 0:
    		address=(LCD_STARTADDR1+col) & 0x7F;
    		break;
    	case 1:
    		address=(LCD_STARTADDR2+col) & 0x7F;
    		break;
    	case 2:
    		address=(LCD_STARTADDR3+col) & 0x7F;
    		break;
    }

    lcd_write_command(0x80|address,1);
}

void lcd_no_display() {
    displayMode=0x00;
    lcd_write_display_mode();
}

void lcd_display() {
    displayMode=0x04;
    lcd_write_display_mode();
}

void lcd_no_cursor() {
    cursorMode=0x00;
    lcd_write_display_mode();
}

void lcd_cursor() {
    cursorMode=0x02;
    lcd_write_display_mode();
}

void lcd_no_blink() {
    blinkMode=0x00;
    lcd_write_display_mode();
}

void lcd_blink() {
    blinkMode=0x01;
    lcd_write_display_mode();
}

void scrollDisplayLeft(void) {
	lcd_set_instruction_set(0);
    lcd_write_command(0x18,30);
}

void scrollDisplayRight(void) {
	lcd_set_instruction_set(0);
    lcd_write_command(0x1C,30);
}

void lcd_left_to_right(void) {
    entryMode|=0x02;
    lcd_write_command(entryMode,30);
}

void lcd_right_to_left(void) {
    entryMode&=~0x02;
    lcd_write_command(entryMode,30);
}

void lcd_autoscroll(void) {
    entryMode|=0x01;
    lcd_write_command(entryMode,30);
}

void lcd_no_autocroll(void) {
    entryMode&=~0x01;
    lcd_write_command(entryMode,30);
}



void lcd_write_display_mode() {
    lcd_write_command((0x08 | displayMode | cursorMode | blinkMode),30);
}


void lcd_set_instruction_set (int is) {
	int _cmd = 0;

    if(is<0 || is>3) { return; }
    _cmd = LCD_INSTRUCTIONSETTEMPLATE | is;
    lcd_write_command(_cmd,30);
}

void lcd_write_command(int value,int executionTime) {
	LCD_PORT &= ~(1 << LCD_RS);

	lcd_spi_transfer(value,executionTime);
}

void lcd_spi_transfer(int value, int executionTime) {
	int i;

	LCD_PORT |= (1 << LCD_CLK);
	LCD_PORT &= ~(1 << LCD_CSB);

    for(i=7;i>=0;i--) {
    	if (value & ( 1 << i)) {
    		LCD_PORT |= (1 << LCD_SI);

    	} else {
    		LCD_PORT &= ~(1 << LCD_SI);
    	}
    	LCD_PORT &= ~(1 << LCD_CLK);
    	LCD_PORT |= (1 << LCD_CLK);
    }

    LCD_PORT |= (1 << LCD_CSB);

    _delay_ms(executionTime);
}

void lcd_set_contrast(int contrast) {
    if(contrast<0 || contrast>0x3F)
	return;
    if(LCD_VCC==DOG_LCD_VCC_5V) {
	/*
	  For 5v operation the booster must be off, which is on the
	  same command as the (2-bit) high-nibble of contrast
	*/
	lcd_write_command((0x50 | ((contrast>>4)&0x03)),30);
	/* Set amplification ratio for the follower control */
	lcd_write_command(0x69,30);
    }
    else {
	/*
	  For 3.3v operation the booster must be on, which is on the
	  same command as the (2-bit) high-nibble of contrast
	*/
	lcd_write_command((0x54 | ((contrast>>4)&0x03)),30);
	/*
	   Set amplification ratio for the follower control
	   this has to be higher it seems at 3.3V operation
	 */
	lcd_write_command(0x6B,30);
    }
    //set low-nibble of the contrast
    lcd_write_command((0x70 | (contrast & 0x0F)),30);
}

void lcd_write_text(const char *str)
{
  while (*str) {
	  lcd_write_char(*str++);
	  wdt_reset();
  }

}
