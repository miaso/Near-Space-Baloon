/*
 * File:   uart1.c
 * Author: T520
 *
 * Created on January 13, 2017, 3:11 PM
 */


#include "xc.h"
#include "main.h"
#include "uart1.h"


void UART1Init() {
    //Set up registers
        // BaudRate = 4800; Frequency = 16000000 Hz; U1BRG 207; 
    U1BRG =0x00CF;// 0x0340; 
    U1MODE = 0x8000; //turn on module
  //  u1
    U1STA = 0x8400; //set interrupts
    /*Enable UART interrupts*/
    IEC0bits.U1RXIE = 1; // interrupt on reception allowed
    // IEC4bits.U1ERIE = 1; // interrupts on errors allowed
    IPC2bits.U1RXIP = 5; // interrupt level of reception
    IPC16bits.U1ERIP = 6; // interrupt level on errors
    U1STAbits.URXISEL = 0; // interrupt when any character is received
    //reset RX interrupt flag
    U1STAbits.OERR = 0;
    IFS0bits.U1RXIF = 0;
    U1STAbits.UTXEN = 1;
}




volatile unsigned char UART_RX_BUFFER[128] = "";
uint8_t rxActive;
volatile int indxRX = 0;
volatile enum CONSTATEgps con_state_gps = stateSOFHunt1;

//volatile enum uart1FrameStat CurrentUart1State = conIdle1;

/*RET conState - See above */
enum uart1FrameStat GPS_Con_Update()  {
    char c;
    enum uart1FrameStat retVal = conIdle1;


    if (U1STAbits.URXDA) {
        do {
            c = U1RXREG;
            switch (CONSTATEgps) {
                case stateSOFHunt1:
                    if (c == GPS_SOF) // IF START OF FRAME DETECTED 
                    {
                
                       rxActive = 0; // used to capture last 2  CRC bytes
                        indxRX = 0;  
                        CONSTATEgps++;          
                    } else {
                               // retVal = con_state_gps;
                    }
                    break;
                case stateData1:
                  //  bChecksum_RX1 ^= c;
                    if (indxRX < 60 && c!=GPS_EOF) {
                        UART_RX_BUFFER[indxRX++] = c;         
                        
                    } else {
                        CONSTATEgps++;
                    }
                    break;
                case stateChecksum1:
                    rxActive++;
                    UART_RX_BUFFER[indxRX++] = c;  

                    if(rxActive >= 0){
               //     if (CRC_CHECK_TRUE) {
                        
                       rxActive = 0; 
                         retVal = conFrameReceived1; // Tell THE world that we got a packet
                       //  NMEA_PARSE();
                    } else {
                        
                    }
                    CONSTATEgps=stateSOFHunt1;
                    //}
                    break;
                default:
                    CONSTATEgps = stateSOFHunt1; // Restart looking for SOF
                    rxActive = 0; // Not really active now...
                    break;
            }
        } while ((retVal == conIdle1) && U1STAbits.URXDA);
    }
    return retVal;
}

#define UART1_RX_INTERRUPT_CLEAR                        IFS0bits.U1RXIF = 0
#define UART1_OERR_CLEAR                                U1STAbits.OERR = 0

volatile uint8_t RxCounter=0;
volatile uint8_t uartBuffer[256];
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
 
  //  GPS_state=GPS_Con_Update();
    
    
    
    
    
//
                     uartBuffer[RxCounter++]= U1RXREG ;
         //   serBuf4[serBufLen4++] = U1RXREG;
    
   // CurrentUart1State = Uart1ConUpdate(1);
    
    UART1_RX_INTERRUPT_CLEAR;
    UART1_OERR_CLEAR;
}




//UART transmit function, parameter Ch is the character to send
void UART1PutChar(char Ch)
{
   //transmit ONLY if TX buffer is empty
   while(U1STAbits.UTXBF == 1);
   U1TXREG = Ch;
}

void UART1PutStr(char* s){
    	char c = 0;
        for (;(( c=*s)!=0);s++){
		UART1PutChar(*s); 
	}
}

//UART receive function, returns the value received.
char UART1GetChar()
{
   char Temp;
   //wait for buffer to fill up, wait for interrupt
   while(IFS0bits.U1RXIF == 0);
   Temp = U1RXREG;
   //reset interrupt
   IFS0bits.U1RXIF = 0;
   //return my received byte
   return Temp;
}

void nmea_parse_gpgga(char *nmea, gpgga_t *loc)
{
    char *p = nmea;

    p = strchr(p, ',')+1; //skip time

    p = strchr(p, ',')+1;
    loc->latitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'N':
            loc->lat = 'N';
            break;
        case 'S':
            loc->lat = 'S';
            break;
        case ',':
            loc->lat = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->longitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'W':
            loc->lon = 'W';
            break;
        case 'E':
            loc->lon = 'E';
            break;
        case ',':
            loc->lon = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->quality = (uint8_t)atoi(p);

    p = strchr(p, ',')+1;
    loc->satellites = (uint8_t)atoi(p);

    p = strchr(p, ',')+1;

    p = strchr(p, ',')+1;
    loc->altitude = atof(p);
}

void nmea_parse_gprmc(char *nmea, gprmc_t *loc)
{
    char *p = nmea;

    p = strchr(p, ',')+1; //skip time
    p = strchr(p, ',')+1; //skip status

    p = strchr(p, ',')+1;
    loc->latitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'N':
            loc->lat = 'N';
            break;
        case 'S':
            loc->lat = 'S';
            break;
        case ',':
            loc->lat = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->longitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'W':
            loc->lon = 'W';
            break;
        case 'E':
            loc->lon = 'E';
            break;
        case ',':
            loc->lon = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->speed = atof(p);

    p = strchr(p, ',')+1;
    loc->course = atof(p);
}

/**
 * Get the message type (GPGGA, GPRMC, etc..)
 *
 * This function filters out also wrong packages (invalid checksum)
 *
 * @param message The NMEA message
 * @return The type of message if it is valid
 */
uint8_t nmea_get_message_type(const char *message)
{
    uint8_t checksum = 0;
    if ((checksum = nmea_valid_checksum(message)) != _EMPTY) {
        return checksum;
    }

    if (strstr(message, NMEA_GPGGA_STR) != NULL) {
        return NMEA_GPGGA;
    }

    if (strstr(message, NMEA_GPRMC_STR) != NULL) {
        return NMEA_GPRMC;
    }

    return NMEA_UNKNOWN;
}

uint8_t nmea_valid_checksum(const char *message) {
    uint8_t checksum= (uint8_t)strtol(strchr(message, '*')+1, NULL, 16);

    char p;
    uint8_t sum = 0;
    ++message;
    while ((p = *message++) != '*') {
        sum ^= p;
    }

    if (sum != checksum) {
        return NMEA_CHECKSUM_ERR;
    }

    return _EMPTY;
}
