#include <htc.h>
#include "watchdog.h"
#include "delay.h"
#define _XTAL_FREQ 32000000

//delays 'time' milliseconds, restarting WDT
void delay_ms_wdt(unsigned int time){
  while(time > 0){
    wdt_restart();
    time--;
    __delay_ms(1);
  }
}
