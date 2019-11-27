/*
 * File:   main.c
 * Author: T520
 *
 * Created on September 30, 2016, 2:58 PM
 */

#define _XTAL_FREQ  32000000UL
#define FCY 16000000UL



#define MCP90 0b10010000
#define MCP91 0b10010010
#define MCP92 0b10010100
#define MCP93 0b10010110
#define MCP94 0b10011000
#define MCP95 0b10011010
#define MCP96 0b10011100
#define MCP97 0b10011110


// CONFIG4
#pragma config DSWDTPS = DSWDTPS1F      // Deep Sleep Watchdog Timer Postscale Select bits (1:68719476736 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select (DSWDT uses LPRC as reference clock)
#pragma config DSBOREN = OFF            // Deep Sleep BOR Enable bit (DSBOR Disabled)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable (DSWDT Disabled)
#pragma config DSSWEN = OFF             // DSEN Bit Enable (Deep Sleep operation is always disabled)
#pragma config PLLDIV = PLL4X         // USB 96 MHz PLL Prescaler Select bits (PLL Disabled)
#pragma config I2C1SEL = ENABLE        // Alternate I2C1 enable bit (I2C1 uses aSCL1 and aSDA1 pins)
#pragma config IOL1WAY = ON             // PPS IOLOCK Set Only Once Enable bit (Once set, the IOLOCK bit cannot be cleared)

// CONFIG3
#pragma config WPFP = WPFP127           // Write Protection Flash Page Segment Boundary (Page 127 (0x1FC00))
#pragma config SOSCSEL = ON             // SOSC Selection bits (SOSC circuit selected)
#pragma config WDTWIN = PS25_0          // Window Mode Watchdog Timer Window Width Select (Watch Dog Timer Window Width is 25 percent)
#pragma config PLLSS = PLL_FRC          // PLL Secondary Selection Configuration bit (PLL is fed by the on-chip Fast RC (FRC) oscillator)
#pragma config BOREN = ON               // Brown-out Reset Enable (Brown-out Reset Enable)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Disabled)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMD = NONE            // Primary Oscillator Select (Primary Oscillator Disabled)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)
#pragma config OSCIOFCN = OFF           // OSCO Pin Configuration (OSCO/CLKO/RA3 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor Configuration bits (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRCPLL           // Initial Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config ALTCMPI = CxINC_RB       // Alternate Comparator Input bit (C1INC is on RB13, C2INC is on RB9 and C3INC is on RA0)
#pragma config WDTCMX = WDTCLK          // WDT Clock Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config IESO = ON                // Internal External Switchover (Enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler Select (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler Ratio Select (1:128)
#pragma config WINDIS = OFF             // Windowed WDT Disable (Standard Watchdog Timer)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC1/PGED1)
#pragma config LPCFG = OFF              // Low power regulator control (Disabled - regardless of RETEN)
#pragma config GWRP = OFF               // General Segment Write Protect (Write to program memory allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = ON              // JTAG Port Enable (Enabled)

#include "xc.h"
#include "main.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>


#include <libpic30.h>
#include "si7020.h"



#include "i2c1.h"
#include "ms5637.h"
#include "hdc1080.h"
#include "uart1.h"
#include "mcc_generated_files/ext_int.h"
#include "mcc_generated_files/pin_manager.h"

//#include "3D Motion Click Driver/app.h"

void PIN_MANAGER_Initialize(void) {
    


    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x000E;
    TRISB = 0x07CB;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0000;
    CNPD3 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x000C;
    ANSB = 0x7204;

    
    
    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

    RPINR18bits.U1CTSR = 0x0003; //RB3->UART1:U1CTS;
    RPOR6bits.RP12R = 0x0003; //RB12->UART1:U1TX;
    RPINR18bits.U1RXR = 0x000A; //RB10->UART1:U1RX;
   // RPOR1bits.RP2R = 0x0004; //RB2->UART1:U1RTS;
    
    
    RPOR7bits.RP14R = 0x0007;   //RB14->SPI1:SDO1;
    RPINR20bits.SDI1R = 0x000D;   //RB13->SPI1:SDI1;
    RPOR1bits.RP2R = 0x0008;   //RB2->SPI1:SCK1OUT;
 
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
}

void OSCILLATOR_Initialize(void) {

    // CF no clock failure; NOSC FRCPLL; SOSCEN disabled; POSCEN disabled; CLKLOCK unlocked; OSWEN Switch is Complete; IOLOCK not-active; 
    __builtin_write_OSCCONL((uint8_t) (0x0100 & 0x00FF));
    // PLLEN enabled; RCDIV FRC/2; DOZE 1:8; DOZEN disabled; ROI disabled; 
    //CLKDIV = 0x3120;
    
       // PLLEN enabled; RCDIV FRC/1; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3020;
    
    // STOR disabled; STORPOL Interrupt when STOR is 1; STSIDL disabled; STLPOL Interrupt when STLOCK is 1; STLOCK disabled; STSRC SOSC; STEN disabled; TUN Center frequency; 
    OSCTUN = 0x0000;
    // ROEN disabled; ROSEL FOSC; ROSIDL disabled; ROSWEN disabled; ROOUT disabled; ROSLP disabled; 
    REFOCONL = 0x0000;
    // RODIV 0;     REFOCONH = 0x0000;
    // ROTRIM 0; 
    REFOTRIML = 0x0000;
    // WDTO disabled; TRAPR disabled; SLEEP disabled; BOR disabled; DPSLP disabled; CM disabled; SWR disabled; SWDTEN disabled; EXTR disabled; POR disabled; IDLE disabled; IOPUWR disabled; VREGS disabled; 
    RCON = 0x0000;
}



/******************************************************************************/
// Trap: _OscillatorFail

/******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void) {
    //  TRAP_OSCFAIL_CLR;
    // LED_RED_ON;
    //#ifdef DEBUG_MODE

    // INTCON1bits.OSCFAIL = 0;

    while (1) {

    }
    //#endif    
    // __asm__ volatile ("reset");
}
/******************************************************************************/
// Trap: AddressError

/******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _AddressError(void) {
    //  TRAP_ADDRERR_CLR;
    // LED_RED_ON;
    //#ifdef DEBUG_MODE
    while (1) {

    }
    //#endif
    //  __asm__ volatile ("reset");
}
/******************************************************************************/
// Trap: StackError

/******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _StackError(void) {
    //  TRAP_STKERR_CLR;
    // LED_RED_ON;
    //#ifdef DEBUG_MODE
    while (1) {

    }
    //#endif
    //  __asm__ volatile ("reset");
}
/******************************************************************************/
// Trap: MathError

/******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _MathError(void) {
    //   TRAP_MATHERR_CLR;
    //LED_RED_ON;
    //#ifdef DEBUG_MODE
    while (1) {

    }
    //#endif
    //  __asm__ volatile ("reset");
}

void __attribute__((interrupt, no_auto_psv)) _DefaultInterrupt(void) {
    //  LED_RED_ON;


    while (1) {


    }


}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _INT0Interrupt(void)
{
    //***User Area Begin->code: INT0 - External Interrupt 0***
   // GPS_ON_OFF_SEQUENCE();
    //***User Area End->code: INT0 - External Interrupt 0***
    EX_INT0_InterruptFlagClear();
}

void EXT_INT_Initialize(void)
{
    /*******
     * INT0
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EX_INT0_InterruptFlagClear();   
    EX_INT0_PositiveEdgeSet();
    EX_INT0_InterruptEnable();
}

void GPS_ON_OFF_SEQUENCE(){
    GPS_ON_OFF_SetHigh();
    __delay_ms(100);
    GPS_ON_OFF_SetLow();
}


volatile uint8_t xujnia;

int main(void) {


    OSCILLATOR_Initialize();

    PIN_MANAGER_Initialize();

    I2C1_Initialize();

    UART1Init();
    
    EXT_INT_Initialize();
   
    GPS_RESET_SetHigh();
    __delay_ms(2000); //x 4
    
    GPS_ON_OFF_SEQUENCE();
    while (1) {


      //  hdc1080_Device_Measurement_Configuration();

       // LED5_Toggle();
        
         __delay_ms(250); //x 4
         UART1PutStr("suka");

    }


    return 0;
}
