#include <htc.h>
#include "usart.h"

void putch(unsigned char byte) 
{
    // output one byte
	while(!TXIF)
		continue;
	TXREG = byte;
}

unsigned char getch() {
	// retrieve one byte
	while(!RCIF)
		continue;
	return RCREG;	
}

unsigned char getche(void)
{
	unsigned char c;
	putch(c = getch());
	return c;
}
