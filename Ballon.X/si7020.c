/*
 * File:   si7020.c
 * Author: Arturas
 *
 * 
 * v.1
 */
#define FCY     16000000ULL //TO DO
#include <libpic30.h>
#include "xc.h"

//#include "i2c1.h"
//#include "uart1.h"


//#include "extralDefines.h"
//#include "oled_draw.h"
#include <stdio.h>
//#include "oled.h"
#include "si7020.h"
#include "main.h"
#include "i2c1.h"





volatile char ttLSB, ttMSB, hhLSB, hhMSB;
volatile uint16_v temperatureCode;
volatile uint16_v humidityCode;
volatile int temperatureValue;
volatile int humidityValue;

volatile uint16_t tempo;

int calculateTemperature(uint16_v code) {

    uint16_t temp;
    // Calculate the temperature using the formula from the datasheet

    //temp = ((((int)17572 * code.w) + 0x8000) >> 16) - 4685;
    //temp= temp/100;

    temp = 175.72 * code.w / 65536 - 46.85;
    // Return value is to be scaled by 10
    //*tempCx10 = (temp + 5) / 10;
    tempo = temp;
    return temp;
}

int calculateHumidity(uint16_v humcode) {
    uint32_t hum;
    hum = humcode.w;
    // Calculate the humidity using the formula from the datasheet
    hum = 125 * hum / 65536 - 6;
    return hum;
}

/*
 * Function:  getLastTemp()
 * --------------------
 * Used to retrieve last temperature measurement, idea is to save some time on 
 * conversion-> because SI7020 does relative humidity(RH) measurement it also measures 
 * temperature. 
 */
void getLastTemp() {
    i2c_start();
    send_i2c_byte(SI7020ADR_W); // address +0 write
    send_i2c_byte(READ_LAST_TEMP_CMD);
    i2c_repeatedStart();
    //i2c_start();  
    send_i2c_byte(SI7020ADR_R); //adresss +1 read
    i2c_mIdleI2C1();
    ttLSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    ttMSB = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();

    temperatureCode.b[0] = ttLSB;
    temperatureCode.b[1] = ttMSB;
}

/*
 * Function:  measureT()
 * --------------------
 * Used to send the temperature measurement command and read the result 
 */
void measureT() {
    i2c_start();
    send_i2c_byte(SI7020ADR_W); // address +0 write
    send_i2c_byte(MEASURE_TEMP_HOLD_CMD); //0xE3 =0b11100011
    i2c_repeatedStart();
    //i2c_start(); 
    send_i2c_byte(SI7020ADR_R); //adresss +1 read
    i2c_mIdleI2C1();
    ttLSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    ttMSB = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
 
    temperatureCode.b[0] = ttLSB;
    temperatureCode.b[1] = ttMSB;
}

/*
 * Function:  measureRH()
 * --------------------
 * Used to send the RH measurement command and read the result 
 */
void measureRH() {
    i2c_start();
    send_i2c_byte(SI7020ADR_W); // address +0 write
    send_i2c_byte(MEASURE_HUM_HOLD_CMD); //0xE5 =0b11100101 
    i2c_repeatedStart();
    send_i2c_byte(SI7020ADR_R); //adresss +1 read
    i2c_mIdleI2C1();
    hhLSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hhMSB = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();

    humidityCode.b[0] = hhLSB;
    humidityCode.b[1] = hhMSB;
    
}


/*
 * Function:makeHTmeasurements(void)
 * --------------------
 *  Function used to run the sequence of measurements:
 *  1.Read RH 
 *  2.Calculate Humidity value 
 *  3.Get last temperature measurement after RH measurement was done
 *  4.Calculate temperature value 
 *  5.Update the display with new values.
 */
void makeHTmeasurements(void) {
  reset_i2c_bus();
   
    measureRH();
    humidityValue = calculateHumidity(humidityCode);
    getLastTemp();
    temperatureValue = calculateTemperature(temperatureCode);
    
    
    char str[10];
    char humm[10];

    int debugf=0;
    debugf=4+debugf;
    sprintf(str, "%d", temperatureValue);
 //   oled_prints(30, 6, str);
    sprintf(humm, "%d", humidityValue);
 //   oled_prints(45, 8, humm);
  //  oled_render();
  //  UART1PutStr(str);
}

/*
 * Function: firmwareTest()
 * --------------------
 *  
 * Requests the firmare version from si7020. Function used to test i2c communication.
 *  
 */
void firmwareTest() {
    i2c_start();
    send_i2c_byte(0x80); // address +0 write
    send_i2c_byte(0x84); /// FIRMWARE!!!!!!!!!
    send_i2c_byte(0xB8); /// FIRMWARE!!!!!!!!!
    //0x84 =0b10000100
    //0xB8 =0b10111000
    i2c_start(); /// FIRMWARE!!!!!!!!!
    send_i2c_byte(0x81); /// FIRMWARE!!!!!!!!!
    uint8_t cc3=i2c_read();

    // i2c_stop();
}



