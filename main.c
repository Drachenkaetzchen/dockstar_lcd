#include <avr/io.h>
#include "usbdrv/usbdrv.h"
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "doglcd.h"

uchar controller = 0;

#define VERSION_MAJOR 2
#define VERSION_MINOR 8
#define VERSION_STR "2.08"


USB_PUBLIC usbMsgLen_t  usbFunctionSetup(uchar data[8]) {
  static uchar replyBuf[4];
  usbMsgPtr = replyBuf;
  uchar len = (data[1] & 3)+1;       // 1 .. 4 bytes
  uchar target = (data[1] >> 3) & 3; // target 0 .. 3
  uchar i;

  // request byte:

  // 7 6 5 4 3 2 1 0
  // C C C T T R L L

  // TT = target bit map
  // R = reserved for future use, set to 0
  // LL = number of bytes in transfer - 1

  switch(data[1] >> 5) {

  case 0: // echo (for transfer reliability testing)
    replyBuf[0] = data[2];
    replyBuf[1] = data[3];
    return 2;
    break;

  case 1: // command
	  target &= controller;  // mask installed controllers

	      if(target) {
      for(i=0;i<len;i++) {
    	  lcd_write_command(data[2+i],1);

      }}
    break;

  case 2: // data
	  target &= controller;  // mask installed controllers

	      if(target) {
    for(i=0;i<len;i++) {
    	  lcd_write_char(data[2+i]);
      }}
    break;

  case 3: // set
    switch(target) {

    case 0:  // contrast
      lcd_set_contrast(data[2]/4);
      break;

    case 1:  // brightness
    	lcd_setbacklight(data[2]);
      break;

    default:
      // must not happen ...
      break;
    }
    break;

  case 4: // get
    switch(target) {
    case 0: // version
      replyBuf[0] = VERSION_MAJOR;
      replyBuf[1] = VERSION_MINOR;
      return 2;
      break;

    case 1: // keys
      replyBuf[0] = 0;
      replyBuf[1] = 0;
      return 2;
      break;

    case 2: // controller map
      replyBuf[0] = 1;
      replyBuf[1] = 0;
      return 2;
      break;

    default:
      // must not happen ...
      break;
    }
    break;

  default:
    // must not happen ...
    break;
  }

  return 0;  // reply len
}


int main(void)
{
	uchar   i;

	lcd_init();
	lcd_setbacklight(20);


	controller = 1;

    wdt_enable(WDTO_2S);

    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();

    lcd_clear();
    lcd_write_text("Waiting for lcd4linux...");

    _delay_ms(500);
    sei();


    for(;;){                /* main event loop */
        wdt_reset();
        usbPoll();

        /*if (dir == 0) {
        	if (c < 255) {
        		c++;
        	} else {
        		dir = 1;
        	}
        } else {
        	if (c > 30) {
        		c--;
        	} else {
        		dir = 0;
        	}
        }
        lcd_setbacklight(c);
        lcd_home();
        lcd_write_text("AFDH!");
        lcd_set_cursor(0,2);
        sprintf(value, "%d", c);
        lcd_write_text(value);
        _delay_ms(5);*/
    }


    return 0;
}
