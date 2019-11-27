#include "../main.h"
#include "app.h"
//#include <p24FJ128GA202.inc>
//extern atomic sfr sbit WKUP;

/** Wake_signal
* @note        assert wake signal on RE9, wait 1 ms, deassert
* @param
* @return
*/

//to do fix

//#define WKUP PORTBbits.RB4 

void Wake_signal()
{
   // WKUP = 0;                                             //assert wake signal
   // _Delay_ms(2);                                          //spec says 3µs assertion, let's use ms delay and wait ~2 ms
__delay_ms(2);
   // WKUP = 1;                                             //de-assert wake signal
}