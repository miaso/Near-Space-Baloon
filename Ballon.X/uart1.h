//prototypes
#ifndef UART1_H
#define UART1_H
//Initiation
extern void UART1Init();

//UART transmit function
extern void  UART1PutChar(char Ch);
extern void UART1PutStr(char *s);
    
//UART receive function
extern char UART1GetChar();

/*******************************/
/*  GPS defines  */
/*******************************/
#define GPS_STATE_IDLE                                   0x00
#define GPS_STATE_FRAME_DATA                             0x01
#define GPS_STATE_FRAME_CHECKSUM                         0x02

#define GPS_SOF                                          '$'
#define GPS_EOF                                          '*'
//extern volatile enum GPS_STATE CurrentUart1State;



/* serial Protocol frame status */
enum uart1FrameStat {
    conIdle1,
    conFrameReceived1,
    conFrameActive1

};
enum CONSTATEgps {
    stateSOFHunt1 = 0,
    stateData1 = 1,
    stateChecksum1 = 2,   
};

#include <stdio.h>
#include <stdlib.h>
//#include <inttypes.h>

#define _EMPTY 0x00
#define NMEA_GPRMC 0x01
#define NMEA_GPRMC_STR "$GPRMC"
#define NMEA_GPGGA 0x02
#define NMEA_GPGGA_STR "$GPGGA"
#define NMEA_UNKNOWN 0x00
#define _COMPLETED 0x03

#define NMEA_CHECKSUM_ERR 0x80
#define NMEA_MESSAGE_ERR 0xC0

struct gpgga {
    // Latitude eg: 4124.8963 (XXYY.ZZKK.. DEG, MIN, SEC.SS)
    double latitude;
    // Latitude eg: N
    char lat;
    // Longitude eg: 08151.6838 (XXXYY.ZZKK.. DEG, MIN, SEC.SS)
    double longitude;
    // Longitude eg: W
    char lon;
    // Quality 0, 1, 2
    uint8_t quality;
    // Number of satellites: 1,2,3,4,5...
    uint8_t satellites;
    // Altitude eg: 280.2 (Meters above mean sea level)
    double altitude;
};
typedef struct gpgga gpgga_t;

struct gprmc {
    double latitude;
    char lat;
    double longitude;
    char lon;
    double speed;
    double course;
};
typedef struct gprmc gprmc_t;

uint8_t nmea_get_message_type(const char *);
uint8_t nmea_valid_checksum(const char *);
void nmea_parse_gpgga(char *, gpgga_t *);
void nmea_parse_gprmc(char *, gprmc_t *);

#endif