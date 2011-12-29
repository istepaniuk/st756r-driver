#ifndef _WDT_H_
#define _WDT_H_

// Turns off the Watchdog timer
#define wdt_off() SWDTEN = 0

// Turns on the Watchdog timer
#define wdt_on() SWDTEN = 1

// Clears the Watchdog Timer
#define wdt_restart() CLRWDT()

// Setups the WDTCON Register and starts the WDT
#define wdt_init() \
  SWDTEN = 1;\
  WDTPS0 = 0;\
  WDTPS1 = 0;\
  WDTPS2 = 1;\
  WDTPS3 = 0;\
  WDTPS4 = 0
  //0b00100 (1:512 prescaler) - 16ms

#endif
