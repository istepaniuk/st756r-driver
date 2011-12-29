#ifndef _DELAY_H_
#define _DELAY_H_

//oscillator freq in Hertz, to calibrate __delay_ms
#define _XTAL_FREQ 32000000

//delays for x milliseconds
#define delay_ms(x) __delay_ms(x)

//delays for x microseconds
#define delay_us(x) __delay_us(x)

//delays for x cycles
#define delay_cycles(x) _delay(x)

//delays 'time' milliseconds, restarting WDT
void delay_ms_wdt(unsigned int);

#endif
