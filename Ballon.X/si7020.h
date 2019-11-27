/*
 * File:   si7020.h
 * Author: Arturas
 *
 * 
 * v.1
 */


#include "xc.h"
#include "main.h"

#define SI7020ADR  0x40
#define SI7020ADR_R  0x81
#define SI7020ADR_W  0x80
#define MEASURE_HUM_HOLD_CMD  0xE5
#define MEASURE_HUM_NOHOLD_CMD  0xF5
#define MEASURE_TEMP_HOLD_CMD  0xE3
#define MEASURE_TEMP_NOHOLD_CMD  0xF3
#define READ_LAST_TEMP_CMD  0xE0
#define RESET_SI7020  0XFE
#define WRITE_RHT_USERREG_CMD  0xE6
#define READ_RHT_USERREG_CMD  0xE7
#define WRITE_HEATERCONTROLREGISTER_CMD 0x51
#define READ_HEATERCONTROLREGISTER_CMD  0x11
 

//#ifndef uint8_t
//typedef unsigned char uint8_t;
//#define uint8_t uint8_t
//#define UINT8_MAX (255)
//#endif
//
//#ifndef uint16_t
//typedef unsigned int uint16_t;
//#define uint16_t uint16_t
//#define UINT16_MAX (65535U)
//#endif
//
//#ifndef uint32_t
//typedef unsigned long int uint32_t;
//#define uint32_t uint32_t
//#define UINT32_MAX (4294967295UL)
//#endif
//
//typedef union
//{
//    uint16_t w;
//    uint8_t  b[2];
//}uint16_v;
// 

void INITIALIZE_SI7020(void);


int calculateTemperature(uint16_v code);
int calculateHumidity(uint16_v humcode);
void getLastTemp();
void getLastTemp();
void measureT();
void measureRH() ;
void makeHTmeasurements(void);
void firmwareTest() ;
 
