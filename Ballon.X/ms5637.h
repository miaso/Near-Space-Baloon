/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_MS_H
#define	XC_MS_H

#include <xc.h> // include processor files - each processor file is guarded.  

void ms5637_restart_device();
void ms5637_read_calibration_data();
void ms5637_read_preassure_and_temperature_data();
void ms5637_do_mesurements();

#define MS5637_ADDRESS 0b11101100

#define COMMAND_Reset  0x1E
#define COMMAND_ConvertD1OSR256  0x40
#define COMMAND_ConvertD1OSR512  0x42
#define COMMAND_ConvertD1OSR1024 0x44
#define COMMAND_ConvertD1OSR2048 0x46
#define COMMAND_ConvertD1OSR4096 0x48
#define COMMAND_ConvertD1OSR8192 0x4A
#define COMMAND_ConvertD2OSR256  0x50
#define COMMAND_ConvertD2OSR512  0x52
#define COMMAND_ConvertD2OSR1024 0x54
#define COMMAND_ConvertD2OSR2048 0x56
#define COMMAND_ConvertD2OSR4096 0x58
#define COMMAND_ConvertD2OSR8192  0x5A
#define COMMAND_ADC_Read  0x00
#define COMMAND_PROM_Read 0xA0

#define COMMAND_PROM_Read_CRC_FACTORY   0b10100000 
#define COMMAND_PROM_Read_C1            0b10100010 //Pressure sensitivity 
#define COMMAND_PROM_Read_C2            0b10100100 //Pressure offset 
#define COMMAND_PROM_Read_C3            0b10100110 //Temperature coefficient of pressure sensitivity 
#define COMMAND_PROM_Read_C4            0b10101000 //Temperature coefficient of pressure offset
#define COMMAND_PROM_Read_C5            0b10101010 //Reference temperature
#define COMMAND_PROM_Read_C6            0b10101100 //Temperature coefficient of the temperature 



extern volatile float temperatureVAL, pressureVAL;

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

