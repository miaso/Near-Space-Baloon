/*
 * File:   MCP9000.c
 * Author: T520
 *
 * Created on 31. oktober 2016, 00:37
 */
#include <stdint.h>
#include "MCP9800.h"
#define FCY     16000000ULL //TO DO
#include <libpic30.h>


#include "xc.h"

#include <stdio.h>

#include "extralDefines.h"

#include "i2c1.h"
//#include "ms5637.h"



volatile uint16_v tempCode, tpCd;
float xaxa;

//int16_t FCV_TEMP,FCV_TEMP2,FCV_DP,FCV_DDP; 

//Variable declarations
volatile uint8_t FCV_TEMP_L;
volatile uint8_t FCV_THERM_R;
volatile int16_t FCV_TEMP;
volatile uint8_t FCV_ACK;
volatile uint8_t FCV_THERM;
volatile uint8_t FCV_THERM_W;
volatile int16_t FCV_TEMP2;
volatile int16_t FCV_DP;
volatile int16_t FCV_DDP;
volatile uint8_t FCV_TEMP_H;



/*
 * Function:void mcp9800_set_resolution(uint8_t adr)
 * --------------------
 * IN:uint8_t adr  - The device address
 * Sets appropriate resolution of the sensor
 */
void mcp9800_set_resolution(uint8_t adr) {
    i2c_start();
    send_i2c_byte(adr); // address +0 write
    i2c_ack();
    send_i2c_byte(resReg);
    i2c_ack();
    send_i2c_byte(0x60);
    i2c_ack();
    i2c_stop();
}

/*
 * Function:void mcp9800_measure_temperature(uint8_t adr) 
 * --------------------
 * IN:uint8_t adr  - The device address
 * Sequence to obtain the Temperature values
 * and calculate the result
 */
void mcp9800_measure_temperature(uint8_t adr) {
    i2c_start();
    send_i2c_byte(adr); // address +0 write
    i2c_ack();

    send_i2c_byte(tempReg); //0xE3 =0b11100011
    i2c_ack();
    i2c_repeatedStart();
    //i2c_start(); 
    send_i2c_byte(adr + 1); //adresss +1 read
    i2c_ack();
    //i2c_mIdleI2C1();
    FCV_TEMP_H = i2c_read_ack();
    i2c_ack();
    //   i2c_mIdleI2C1();
    //   i2c_ack();
    FCV_TEMP_L = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
    // 
    tempCode.b[0] = FCV_TEMP_L;
    tempCode.b[1] = FCV_TEMP_H;

    if ((FCV_TEMP_H & 0x80) == 128) {
 

        FCV_TEMP_L = FCV_TEMP_L & 0xF0;
        FCV_TEMP = (FCV_TEMP_H << 8) | FCV_TEMP_L;
        FCV_TEMP = (~FCV_TEMP) + 1;
        FCV_TEMP = (FCV_TEMP >> 4);
        FCV_TEMP2 = FCV_TEMP / 16;
        FCV_DP = FCV_TEMP % 16;
        FCV_DP = (FCV_DP * 100) / 16;
        FCV_DDP = FCV_DP % 10;
        FCV_DP = FCV_DP / 10;

    } else {
    
        FCV_TEMP = (FCV_TEMP_H << 8) | FCV_TEMP_L;
        FCV_TEMP = (FCV_TEMP >> 4);
        FCV_TEMP2 = FCV_TEMP / 16;
        FCV_DP = FCV_TEMP % 16;
        FCV_DP = (FCV_DP * 100) / 16;
        FCV_DDP = FCV_DP % 10;
        FCV_DP = FCV_DP / 10;

    }

}