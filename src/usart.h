#ifndef _SERIAL_H_
#define _SERIAL_H_

// baud rate and FOSC speed
#define BAUD 115200
#define FOSC 32000000L
// 9 bit comunication
#define NINE 0

#define DIVIDER ((int)(FOSC / (16UL * BAUD) -1))
#define HIGH_SPEED 1

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif


#define RX_PIN TRISC7
#define TX_PIN TRISC6


// serial initialization
#define usart_setup()\
	RX_PIN = 1;	\
	TX_PIN = 1;		  \
	SPBRG = DIVIDER;     	\
	RCSTA = (NINE_BITS|0x90);	\
	TXSTA = (SPEED|NINE_BITS|0x20)

void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);

#endif
