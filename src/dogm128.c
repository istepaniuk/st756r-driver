#include "dogm128.h"
#include "delay.h"
#include "usart.h"
#include "font7x5.h"
#include "font48x30.h"

unsigned char dogm_page_buffer[LCD_PAGE_WIDTH];
  
void dogm_init(void){
  // Set CS and A0 pins to output
  TRIS_LCD_SS = 0;
  TRIS_LCD_A0 = 0;
  TRIS_LCD_RESET = 0;
  
  // Reset the LCD Controller and leave /RESET high.
  LCD_PIN_RESET = 1;
  dogm_delay(10);
  LCD_PIN_RESET = 0;
  dogm_delay(10);
  LCD_PIN_RESET = 1;
  dogm_delay(10);  
  
  dogm_spi_disable();
  dogm_delay(10);
  
  // Set the A0 pin to command mode (low)
  dogm_cmd_mode();
  // Set the /CS(/SS) pin to low.
  dogm_spi_enable();

  
  // Initialize the LCD
  dogm_spi_out(0x40); 		// Set display start line to 0
  #ifdef LCD_UPSIDE_DOWN
    dogm_spi_out(0xa0); 	// ADC set to 
  #else
    dogm_spi_out(0xa1); 	// ADC set to 
  #endif
  dogm_spi_out(0xc0); 		// common output mode
  dogm_spi_out(0xa6); 		// display normal, bit val 0: LCD pixel off
  dogm_spi_out(0xa2); 		// LCD bias 1/9
  dogm_spi_out(0x2f); 		// all power  control circuits on
  dogm_spi_out(0xf8); 		// set booster ratio to
  dogm_spi_out(0x00); 		// 4x
  dogm_spi_out(0x27); 		// set V0 voltage resistor ratio to large
  dogm_spi_out(0x81); 		// set contrast
  dogm_spi_out(0x16); 		// contrast value, EA default: 0x016
  dogm_spi_out(0xac); 		// indicator
  dogm_spi_out(0x00); 		// disable
  dogm_spi_out(0xaf); 		// display on
  
  dogm_spi_out(0xa4);	  	// normal display
  
  dogm_spi_disable();
  dogm_clear_screen();
  
  dogm_current_page=0;
}


void dogm_clear_screen(void){
  dogm_current_page = 8;
  while(dogm_current_page){
    dogm_current_page--;
    dogm_clear_page();
    dogm_transfer_page_all();
  }
}

// Page functions

void dogm_clear_page(void){
  unsigned char i = LCD_PAGE_WIDTH;
  while(i){
    i--;
    dogm_page_buffer[i] = 0x00;
  }
}

void dogm_next_page(void){
  dogm_current_page++;
  if (dogm_current_page > 7) dogm_current_page = 0;
}

void dogm_transfer_page(unsigned char from, unsigned char to){
  unsigned char column_adr;
 
  // Set write position
  dogm_cmd_mode();   
  dogm_spi_enable();
  #ifdef LCD_UPSIDE_DOWN
    dogm_spi_out(0xb0 | (7-dogm_current_page) );		// select current page
    
    // Leave 4 bits outside as the ST756R RAM is 132bits wide, not 128.
    // Note that if LCD_UPSIDE_DOWN is defined, ADC (column direction) is reversed
    column_adr = from+4;
  #else 
    dogm_spi_out(0xb0 | dogm_current_page );		// select current page
    column_adr = from;    
  #endif

  dogm_spi_out(0x10 | ((0xF0 & column_adr) >> 4));		// set upper 4 bit of the col adr to 0 
  dogm_spi_out(0x00 | ((0x0F & column_adr)     ));		// set lower 4 bit of the col adr to 0 
    
  dogm_spi_disable();
  
  // Send a complete page 
  dogm_data_mode();
  dogm_spi_enable();
  
  #ifdef LCD_UPSIDE_DOWN
  
  unsigned char idx;
  idx = from;
  while( idx <= to )
  {
    dogm_spi_out(dogm_page_buffer[idx]);    
    idx++;
  }
  #else
  
  unsigned char idx;
  idx = from;
  while( idx <= to )
  {
    // All fonts are upside down (MSB is on top)
    dogm_spi_out(reverse_byte(dogm_page_buffer[idx])); 
    idx++;
  }
  #endif
  
  dogm_spi_disable();
}

void dogm_draw_char_48x30(unsigned char c, unsigned char xpos){
  unsigned char i;
  unsigned char page = 6;
    
  dogm_current_page = dogm_current_page + 6;
  while(page){
    page--;
    i=30;
    while(i){
      i--;
      if(c<10)
        dogm_page_buffer[i + xpos] = font48x30[(c * 180) + (i * 6) + page];
      else
        dogm_page_buffer[i + xpos] = 0x00;
    }
    dogm_current_page--;
    dogm_transfer_page(xpos, xpos + 29);
  }
}
  
void dogm_draw_char_7x5(unsigned char c, unsigned char xpos){
  unsigned char i = 5;
  
  switch (c){
    case  'z': c = 13 * 5; break;
    case  '-': c = 14 * 5; break;
    case  ' ': c = 15 * 5; break; 
    case  '%': c = 16 * 5; break; 
    case   18: c = 18 * 5; break; 
    default:   c = (c - 46) * 5;
  } 

  while(i){
    i--;
    dogm_page_buffer[i + xpos] = font7x5_letters[c + i];    
  }  
  
  // Clear the 6th vertical line after the character.
  dogm_page_buffer[5 + xpos] = 0x00;
}


void dogm_draw_string_7x5(const char *s, unsigned char xpos){
  unsigned char i=0;
  while( *s != '\0' ) {
    dogm_draw_char_7x5((unsigned char)*s, xpos + i * 6);
    i++;
    s++;
  }
}

void dogm_draw_box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2){
  unsigned char y; 
  unsigned char i;
  unsigned char y1_page = y1 / 8;
  unsigned char y2_page = y2 / 8;
  
  // Upper horizontal line
  dogm_clear_page();
  y = 0x80 >> (y1%8);
  for(i=x1;i<x2;i++) dogm_page_buffer[i] = y;
  
  // Upper corners
  i = 8 - (y1%8);
  while(i){i--; y |= 1 << i; }
  dogm_page_buffer[x1] = y;
  dogm_page_buffer[x2] = y;
  dogm_current_page = y1_page;
  dogm_transfer_page(x1,x2);
  
  // Lower horizontal line
  dogm_clear_page();
  y = 0x80 >> y2%8;
  for(i=x1;i<x2;i++) dogm_page_buffer[i] = y;

  // Lower corners
  i = (y2 % 8);
  while(i){i--; y |= 0x80 >> i; }
  dogm_page_buffer[x1] = y;
  dogm_page_buffer[x2] = y;
  dogm_current_page = y2_page;
  dogm_transfer_page(x1,x2);
  
  // Adittional Vertical line sections (pages)
  dogm_clear_page();
  dogm_page_buffer[x1] = 0xFF;
  dogm_page_buffer[x2] = 0xFF;
  for( y = y1_page + 1; y < y2_page ; y++){
    dogm_current_page = y;
    dogm_transfer_page(x1, x2);
  } 
}

unsigned char reverse_byte(unsigned char v){
  // r will be reversed bits of v; first get LSB of v
  unsigned char r = v; 
  // Extra shift needed at end
  char s = 7; 
 
  // Stop when v is 0
  for (v >>= 1; v; v >>= 1) {
    r <<= 1;
    r |= v & 1;
    s--;
  }
  // Shift when v's highest bits are zero
  r <<= s; 
  
  return(r);
}
