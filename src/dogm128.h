#ifndef _DOGM128_H_
#define _DOGM128_H_

#include "spi.h"
#include <htc.h>

#define LCD_PIN_SS RB2
#define LCD_PIN_A0 RA5
#define LCD_PIN_RESET RB3

#define LCD_UPSIDE_DOWN

#define TRIS_LCD_SS TRISB2
#define TRIS_LCD_A0 TRISA5
#define TRIS_LCD_RESET TRISB3

#define LCD_WIDTH 128
#define LCD_HEIGHT 64

#define LCD_PAGE_WIDTH 128
#define LCD_PAGE_CNT 8
#define LCD_PAGE_HEIGHT 8

#define dogm_spi_out spi_write
#define dogm_spi_enable()  LCD_PIN_SS=0
#define dogm_spi_disable() LCD_PIN_SS=1
#define dogm_cmd_mode()  LCD_PIN_A0=0;delay_cycles(5)
#define dogm_data_mode() LCD_PIN_A0=1;delay_cycles(5)
#define dogm_delay(x) delay_cycles(x)

unsigned char dogm_current_page;

extern void dogm_init(void);

extern void dogm_transfer_page(unsigned char from, unsigned char to);
#define dogm_transfer_page_all() dogm_transfer_page(0,127)

extern void dogm_clear_page(void);
extern void dogm_next_page(void);
extern void dogm_clear_screen(void);
extern void dogm_draw_char_7x5(unsigned char c, unsigned char xpos);
extern void dogm_draw_string_7x5(const char *s, unsigned char xpos);
extern void dogm_draw_char_48x30(unsigned char c, unsigned char xpos);
extern void dogm_draw_box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);

extern unsigned char reverse_byte(unsigned char v);

#endif
