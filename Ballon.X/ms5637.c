/*
 * File:   ms5637.c
 * Author: T520
 *
 * Created on 30. oktober 2016, 16:03
 */
#define FCY     16000000ULL //TO DO
#include <libpic30.h>


#include "xc.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>

//#include "extralDefines.h"

#include "i2c1.h"
#include "ms5637.h"


uint16_t C1, C2, C3, C4, C5, C6, crctest;
uint32_t D1, D2;
int32_t dT, P;
int64_t OFF, SENS, OFF2, SENS2, T2, TEMP, P2;
uint8_t MSB, LSB;
uint8_t nr1, nr2, nr3;
//long long T2;
volatile float temperatureVAL, pressureVAL;

/*
 * Function: void ms5637_reset() 
 * --------------------
 * RESET SEQUENCE
 * The Reset sequence shall be sent once after power-on to make sure that the calibration PROM gets loaded into
 * the internal register. It can be also used to reset the device PROM from an unknown condition.
 * The reset can be sent at any time. In the event that there is not a successful power on reset this may be caused by
 * the SDA being blocked by the module in the acknowledge state. The only way to get the MS5637 to function is to
 * send several SCLs followed by a reset sequence or to repeat power on reset.
 */
void ms5637_reset() {
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS); // address +0 write  
    //ACK FROM SLAVE ???
    send_i2c_byte(COMMAND_Reset);
    i2c_stop();
}

/*
 * Function: uint16_t ms5637_read_reg_PROM(uint8_t reg) 
 * --------------------
 * IN:uint8_t reg REGISTRY address to access
 * OUT:uint16_t   addressed REGISTRY contents  
 * PROM READ SEQUENCE
 * The read command for PROM shall be executed once after reset by the user to read the content of the calibration
 * PROM and to calculate the calibration coefficients. There are in total 7 addresses resulting in a total memory of 112
 * bit. Addresses contains factory data and the setup, calibration coefficients, the serial code and CRC. The command
 * sequence is 8 bits long with a 16 bit result which is clocked with the MSB first. The PROM Read command consists
 * of two parts. First command sets up the system into PROM read mode. The second part gets the data from the
 * system
 */
uint16_t ms5637_read_reg_PROM(uint8_t reg) {
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS); // address +0 write  
    //ACK FROM SLAVE  ?
    send_i2c_byte(reg);
    //ACK FROM SLAVE  ?
    i2c_stop();
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS + 1);
    i2c_mIdleI2C1();
    MSB = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    LSB = i2c_read_ack();
    i2c_mIdleI2C1(); /////////////////////// maybe?
    i2c_nack();
    i2c_stop();
    return (MSB << 8) | LSB;
}

/*
 * Function: void ms5637_read_calibration_data()  
 * --------------------
 * Read calibration values sequence.
 * Coefficient 1 is stored in C1 , n is stored in Cn ...
 */
void ms5637_read_calibration_data() {
    // CRC and factory
    crctest = ms5637_read_reg_PROM(COMMAND_PROM_Read_CRC_FACTORY);
    C1 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C1);
    C2 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C2);
    C3 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C3);
    C4 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C4);
    C5 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C5);
    C6 = ms5637_read_reg_PROM(COMMAND_PROM_Read_C6);

    //crc check, not implemented
    uint8_t n_prom[8];
    n_prom[0] = crctest;
    n_prom[1] = C1;
    n_prom[2] = C2;
    n_prom[3] = C3;
    n_prom[4] = C4;
    n_prom[5] = C5;
    n_prom[6] = C6; // 
}

/*
 * Function:  ms5637_restart_device()
 * --------------------
 * Used to restart the device and retrieve callibration values/ coefficients 
 */
void ms5637_restart_device() {
    ms5637_reset();
    ms5637_read_calibration_data();
}

/*
 * Function: ms5637_conversion_serquence(uint8_t convType)
 * --------------------
 * IN: uint8_t convType
 * OUT:uint32_t Digital Pressure / Temperature value
 * The conversion command is used to initiate uncompensated pressure (D1) or uncompensated temperature (D2)
 * conversion. After the conversion, using ADC read command the result is clocked out with the MSB first. If the
 * conversion is not executed before the ADC read command, or the ADC read command is repeated, it will give 0 as
 * the output result. If the ADC read command is sent during conversion the result will be 0, the conversion will not
 * stop and the final result will be wrong. Conversion sequence sent during the already started conversion process will
 * yield incorrect result as well. A conversion can be started by sending the command to MS5637. When command is
 * sent to the system it stays busy until conversion is done. When conversion is finished the data can be accessed by
 * sending a Read command, when an acknowledge is sent from the MS5637, 24 SCL cycles may be sent to receive
 * all result bits. Every 8 bits the system waits for an acknowledge sig
 */
uint32_t ms5637_conversion_serquence(uint8_t convType) {
    /////////////////////////////////////////////////////   
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS); // address write
    //ACK FROM SLAVE  ?
    send_i2c_byte(convType);
    //ACK FROM SLAVE  ?
    i2c_stop();

    __delay_ms(20);
    //////////////////////////////////////////////////////    
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS); // address write
    //ACK FROM SLAVE  ? 
    send_i2c_byte(COMMAND_ADC_Read);
    i2c_stop();
    //////////////////////////////////////////////////////    
    i2c_start();
    send_i2c_byte(MS5637_ADDRESS + 1);
    i2c_mIdleI2C1();
    nr1 = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    nr2 = i2c_read_ack();
    i2c_ack();
    i2c_mIdleI2C1();
    i2c_ack();
    nr3 = i2c_read_ack();
    //
    //i2c_mIdleI2C1(); /////////////////////// maybe?
    //  i2c_nack();
    i2c_stop();

    uint32_v fss;
    fss.b[0] = nr3;
    fss.b[1] = nr2;
    fss.b[2] = nr1;
    fss.b[3] = 0;
    return fss.dw;

}

unsigned char crc4(unsigned int n_prom[]) // n_prom defined as 8x unsigned int (n_prom[8])
{
    int cnt; // simple counter
    unsigned int n_rem = 0; // crc reminder
    unsigned char n_bit;
    n_prom[0] = ((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
    n_prom[7] = 0; // Subsidiary value, set to 0
    for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
    { // choose LSB or MSB
        if (cnt % 2 == 1) n_rem ^= (unsigned short) ((n_prom[cnt >> 1]) & 0x00FF);
        else n_rem ^= (unsigned short) (n_prom[cnt >> 1] >> 8);
        for (n_bit = 8; n_bit > 0; n_bit--) {
            if (n_rem & (0x8000)) n_rem = (n_rem << 1) ^ 0x3000;
            else n_rem = (n_rem << 1);
        }
    }
    n_rem = ((n_rem >> 12) & 0x000F); // final 4-bit reminder is CRC code
    return (n_rem ^ 0x00);
}

/*
 * Function: void ms5637_read_preassure_and_temperature_data()
 * --------------------
 * Sequence to read  bot Digital Pressure and Temperature value
 */
void ms5637_read_preassure_and_temperature_data() {
    D1 = ms5637_conversion_serquence(COMMAND_ConvertD1OSR256);
    D2 = ms5637_conversion_serquence(COMMAND_ConvertD2OSR256);
}

/*
 * Function:void ms5637_calculate_temperature()
 * --------------------
 * Sequence to calculate the temperature value and modify some offsets if needed
 */
void ms5637_calculate_temperature() {
    dT = (int32_t) D2 - ((int32_t) C5 * (int32_t) 256); //// BAD? ??? ? 
    TEMP = (int64_t) 2000 + ((int64_t) dT * (int64_t) C6 / (int64_t) 8388608); //7286016
    // SECOND ORDER TEMPERATURE COMPENSATION
    //In order to obtain best accuracy over temperature range, particularly at low temperature, it is recommended to
    //compensate the non-linearity over the temperature. This can be achieved by correcting the calculated temperature,
    //offset and sensitivity by a second-order correction factor. The second-order factors are calculated as follows:
    if (TEMP < 2000) {
        T2 = (int64_t) 3 * ((int64_t) dT * (int64_t) dT) / (1LL << 33);
        OFF2 = (int64_t) 61 * (((int64_t) TEMP - (int64_t) 2000)*((int64_t) TEMP - (int64_t) 2000)) / (int64_t) 16;
        SENS2 = (int64_t) 29 * (((int64_t) TEMP - (int64_t) 2000)*((int64_t) TEMP - (int64_t) 2000)) / (int64_t) 16;
        if (TEMP<-1500) {
            OFF2 = (int64_t) OFF2 + (int64_t) 17 * (((int64_t) TEMP - (int64_t) 1500)*((int64_t) TEMP - (int64_t) 1500));
            SENS2 = (int64_t) SENS2 + (int64_t) 9 * (((int64_t) TEMP - (int64_t) 1500)*((int64_t) TEMP - (int64_t) 1500));
        }
    } else {
        T2 = (int64_t) 5 * ((int64_t) dT * (int64_t) dT) / (1LL << 38);
    }
    OFF = (int64_t) C2 * (int64_t) 131072 + ((int64_t) C4 * (int64_t) dT) / (int64_t) 64;
    SENS = (int64_t) C1 * (int64_t) 65536 + ((int64_t) C3 * (int64_t) dT) / (int64_t) 128;

    TEMP = (int64_t) TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    temperatureVAL = TEMP / 100.0; // ACTUAL TEMEPRATURE RESULT IN FLOAT 
}

/*
 * Function:void ms5637_calculate_compensated_pressure()
 * --------------------
 * Sequence to calculate the pressure
 */
void ms5637_calculate_compensated_pressure() {

    //    OFF=C2*131072+(C4*dT)/64;
    //    SENS=C1*65536+(C3*dT)/128;

    P = (int32_t) ((int32_t) D1 * (int32_t) SENS / (int32_t) 2097152 - (int32_t) OFF) / (int32_t) 32768;
    P2 = (int64_t) ((int64_t) D1 * (int64_t) SENS / (int64_t) 2097152 - (int64_t) OFF) / (int64_t) 32768;
    pressureVAL = P2 / 100.0; // ACTUAL PREASSURE RESULT IN FLOAT 
}
/*
 * Function:void ms5637_do_mesurements()
 * --------------------
 * Full sequence of obtaining the required data and calculating the results.
 * Should be called from main loop, after executing the method
 * the measurements are stored in ==>> float temperatureVAL, pressureVAL;
 */
void ms5637_do_mesurements() {
    ms5637_read_preassure_and_temperature_data();
    ms5637_calculate_temperature();
    ms5637_calculate_compensated_pressure();

}

