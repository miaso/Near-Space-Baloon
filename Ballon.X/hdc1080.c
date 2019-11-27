/*
 * File:   hdc1080.c
 * Author: T520
 *
 * Created on 5. november 2016, 16:40
 */


/*
 * File:   hdc1080.c
 * Author: T520
 *
 * Created on 8. november 2016, 15:17
 */

#define FCY 16000000UL

#include <libpic30.h>

#include "xc.h"
#include "main.h"
#include "i2c1.h"
#include "hdc1080.h"
#include "si7020.h"



uint8_t hdcMSB, hdcLSB,hdcMSB2, hdcLSB2;
uint16_v hdc1080_tempReg, hdc1080_humReg;

float hdc1080_TEMP, hdc1080_HUM;




//When reading from the HDC1080, the last value stored in the pointer by a write operation is used to determine
//which register is accessed by a read operation. To change the pointer register for a read operation, a new value
//must be written to the pointer register. This transaction is accomplished by issuing the slave address byte with
//the R/W bit low, followed by the pointer byte. 

uint16_t hdc1080_Read_Register(uint8_t RegAddr) {
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(RegAddr);
    //ACK FROM SLAVE  ?
    //i2c_stop(); //meh ? maybe needed
   //  i2c_stop();
    i2c_start();
   // i2c_start();
    //i2c_repeatedStart();
    send_i2c_byte(HDC1080_ADDRESS + 1);
    i2c_mIdleI2C1();
    hdcMSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdcLSB = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
    
   // /jajajajaj//
    return (hdcMSB << 8) | hdcLSB;
}

void hdc1080_HUMIDITYANDTEMPERATUREMEASUREMENTS() {
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS+1); 

    i2c_mIdleI2C1();
    hdc1080_tempReg.b[1] = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdc1080_tempReg.b[0]= i2c_read_ack();
    
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdc1080_humReg.b[1]= i2c_read_ack();
    
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdc1080_humReg.b[0]= i2c_read_ack();
    
    
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
    
   // /jajajajaj//
  //  return (hdcMSB << 8) | hdcLSB;
}

//To access a particular register on the HDC1080, write the desired register address value to the Pointer Register.
//The pointer value is the first byte transferred after the slave address byte with the R/W bit low. Every write
//operation to the HDC1080 requires a value for the pointer register 

void hdc1080_Write_Register(uint8_t RegAddr, uint8_t MSBb, uint8_t LSBb) {
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(RegAddr);
    //ACK FROM SLAVE  ?
    send_i2c_byte(MSBb);
    //ACK FROM SLAVE  ?
    send_i2c_byte(LSBb);
    //ACK FROM SLAVE b ?   
    i2c_stop();
}

void hdc1080_Write_Register1byte(uint8_t RegAddr) {
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(RegAddr);
    //ACK FROM SLAVE  ?
   

    //probablyi2c_stop();
}
void hdc1080_triger(uint8_t data) {
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(data);
}



//#ifndef uint16_t
//typedef unsigned int uint16_t;
//#define uint16_t uint16_t
//#define UINT16_MAX (65535U)
//#endif
//typedef unsigned long           uint32_t;
//typedef union
//{
//    uint32_t dw;
//    uint16_t w[2];
//    uint8_t  b[4];
//} uint32_v;


double  tempds;
void hdc1080_Calculate_Temperature() {
    hdc1080_TEMP = (((int32_t)hdc1080_tempReg.w /(int32_t) 65536)*(int32_t)165) -(int32_t) 40;
tempds =((double)hdc1080_tempReg.w / (double)65536)*(double)165 - (double)40;

}
double humds;
void hdc1080_Calculate_Humidity() {
    hdc1080_HUM = ((uint16_t)hdc1080_humReg.w / 65536)*100;
    humds = ((double)hdc1080_humReg.w /(double) 65536)*(double)100;
}


uint8_t hdcnr1,hdcnr2,hdcnr3,nr4;
void hdc1080_Trigger_Humidity_Temperature_Measurement(){
     i2c_start();
    send_i2c_byte(HDC1080_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(HDC1080_TEMPERATURE_REGITSTER);
    //ACK FROM SLAVE  ?
    
    /////////////////// problem block
      send_i2c_byte(HDC1080_ADDRESS+1); // address +0 write  
    // while no ack by slave  ???? 
    /////////////////////
      
      
    i2c_mIdleI2C1();
    hdcnr1 = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdcnr2 = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    
    hdcnr3 = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    
    
    
    
    nr4 = i2c_read_ack();
    //
    //i2c_mIdleI2C1(); /////////////////////// maybe?
    //  i2c_nack();
    i2c_stop();
    
    
    
    /// dva int16 sdelannyje iz dvux
    hdc1080_HUM=(hdcnr1 << 8) | hdcnr2;
    hdc1080_TEMP=(hdcnr3 << 8) | nr4;
            
            
      
    
//    send_i2c_byte(MSBb);
//    //ACK FROM SLAVE  ?
//    send_i2c_byte(LSBb);
//    //ACK FROM SLAVE b ?   

}



void hdc1080_Read_HUM_TEMP_MEASUREMENTS(){
    i2c_start();
    send_i2c_byte(HDC1080_ADDRESS+1); // address +0 write  
    i2c_mIdleI2C1();
    hdcMSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdcLSB = i2c_read_ack();
    
        i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdcLSB = i2c_read_ack();
    
        i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    hdcMSB = i2c_read_ack();
    
    
    
    
    
    
    
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
    
   // /jajajajaj//
//    return (hdcMSB << 8) | hdcLSB;
}

void hdc1080_Setup(){
    
}
void hdc1080_Do_Measuremetns(){
    
}
void hdc1080_Device_Measurement_Configuration() {
    // Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1
    //1. Configure the acquisition parameters in register address 0x02:
    //(a) Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1.
    //(b) Set the desired temperature measurement resolution:
    //? Set Bit[10] to 0 for 14 bit resolution.
    //? Set Bit[10] to 1 for 11 bit resolution.
    //(c) Set the desired humidity measurement resolution:
    //? Set Bit[9:8] to 00 for 14 bit resolution.
    //? Set Bit[9:8] to 01 for 11 bit resolution.
    //? Set Bit[9:8] to 10 for 8 bit resolution
    hdc1080_Write_Register(HDC1080_CONFIGURATION_REGISTER,0b00010100,0b00000000); //Set Bit[9:8] to 00 for 14 bit resolution. // Set Bit[10] to 0 for 14 bit resolution.
    
    hdc1080_triger(0x00);
     __delay_ms(7);
    hdc1080_HUMIDITYANDTEMPERATUREMEASUREMENTS();
    hdc1080_Calculate_Temperature();
    hdc1080_Calculate_Humidity();
    
}