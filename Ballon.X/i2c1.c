/**
  I2C1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c1.c

  @Summary
    This is the generated header file for the I2C1 driver using MPLAB(c) Code Configurator

  @Description
    This header file provides APIs for driver for I2C1.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - v3.00
        Device            :  PIC24FJ128GA306
        Driver Version    :  1.0
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.26
        MPLAB 	          :  MPLAB X 3.20
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */

#include "i2c1.h"
//#include "si7020.h"
///#include "uart1.h"
//#include "extralDefines.h"
#define FCY     16000000ULL //TO DO

#include <libpic30.h>
/**
 Section: Data Types
 */

/**
  I2C Driver Queue Status Type

  @Summary
    Defines the type used for the transaction queue status.

  @Description
    This defines type used to keep track of the queue status.
 */

typedef union {

    struct {
        uint8_t full : 1;
        uint8_t empty : 1;
        uint8_t reserved : 6;
    } s;
    uint8_t status;
} I2C_TR_QUEUE_STATUS;

/**
  I2C Driver Queue Entry Type

  @Summary
    Defines the object used for an entry in the i2c queue items.

  @Description
    This defines the object in the i2c queue. Each entry is a composed
    of a list of TRBs, the number of the TRBs and the status of the
    currently processed TRB.
 */
typedef struct {
    uint8_t count; // a count of trb's in the trb list
    I2C1_TRANSACTION_REQUEST_BLOCK *ptrb_list; // pointer to the trb list
    I2C1_MESSAGE_STATUS *pTrFlag; // set with the error of the last trb sent.
    // if all trb's are sent successfully,
    // then this is I2C1_MESSAGE_COMPLETE
} I2C_TR_QUEUE_ENTRY;

/**
  I2C Master Driver Object Type

  @Summary
    Defines the object that manages the i2c master.

  @Description
    This defines the object that manages the sending and receiving of
    i2c master transactions.
 */

typedef struct {
    /* Read/Write Queue */
    I2C_TR_QUEUE_ENTRY *pTrTail; // tail of the queue
    I2C_TR_QUEUE_ENTRY *pTrHead; // head of the queue
    I2C_TR_QUEUE_STATUS trStatus; // status of the last transaction
    uint8_t i2cDoneFlag; // flag to indicate the current
    // transaction is done
    uint8_t i2cErrors; // keeps track of errors


} I2C_OBJECT;

/**
  I2C Master Driver State Enumeration

  @Summary
    Defines the different states of the i2c master.

  @Description
    This defines the different states that the i2c master
    used to process transactions on the i2c bus.
 */

typedef enum {
    S_MASTER_IDLE,
    S_MASTER_RESTART,
    S_MASTER_SEND_ADDR,
    S_MASTER_SEND_DATA,
    S_MASTER_SEND_STOP,
    S_MASTER_ACK_ADDR,
    S_MASTER_RCV_DATA,
    S_MASTER_RCV_STOP,
    S_MASTER_ACK_RCV_DATA,
    S_MASTER_NOACK_STOP,
    S_MASTER_SEND_ADDR_10BIT_LSB,
    S_MASTER_10BIT_RESTART,

} I2C_MASTER_STATES;

/**
 Section: Macro Definitions
 */

/* defined for I2C1 */


#ifndef I2C1_CONFIG_TR_QUEUE_LENGTH
#define I2C1_CONFIG_TR_QUEUE_LENGTH 1
#endif

#define I2C1_TRANSMIT_REG                       I2C1TRN                 // Defines the transmit register used to send data.
#define I2C1_RECEIVE_REG                        I2C1RCV                 // Defines the receive register used to receive data.

// The following control bits are used in the I2C state machine to manage
// the I2C module and determine next states.
#define I2C1_WRITE_COLLISION_STATUS_BIT         I2C1STATbits.IWCOL      // Defines the write collision status bit.
#define I2C1_ACKNOWLEDGE_STATUS_BIT             I2C1STATbits.ACKSTAT    // I2C ACK status bit.

#define I2C1_START_CONDITION_ENABLE_BIT         I2C1CONbits.SEN         // I2C START control bit.
#define I2C1_REPEAT_START_CONDITION_ENABLE_BIT  I2C1CONbits.RSEN        // I2C Repeated START control bit.
#define I2C1_RECEIVE_ENABLE_BIT                 I2C1CONbits.RCEN        // I2C Receive enable control bit.
#define I2C1_STOP_CONDITION_ENABLE_BIT          I2C1CONbits.PEN         // I2C STOP control bit.
#define I2C1_ACKNOWLEDGE_ENABLE_BIT             I2C1CONbits.ACKEN       // I2C ACK start control bit.
#define I2C1_ACKNOWLEDGE_DATA_BIT               I2C1CONbits.ACKDT       // I2C ACK data control bit.

/**
 Section: Local Functions
 */




/**
  Section: Driver Interface
 */

void I2C1_Initialize(void) {

//    // initialize the hardware
//    // Baud Rate Generator Value: I2CBRG 2;   
//    // Calculated Frequency: 100/404.0 kHz
//    I2C1BRG = 0x9D; //9D;  -100 kHz //0x25;-404
//    // ACKEN disabled; STREN disabled; GCEN disabled; SMEN disabled; DISSLW disabled; I2CSIDL disabled; ACKDT Sends ACK; SCLREL Holds; RSEN disabled; IPMIEN disabled; A10M 7 Bit; PEN disabled; RCEN disabled; SEN disabled; I2CEN enabled; 
//    I2C1CON1 = 0x8200;
//    // P disabled; S disabled; BCL disabled; I2COV disabled; IWCOL disabled; 
//    I2C1STAT = 0x0000;
//
//    /* MI2C1 - I2C1 Master Events */
//    // clear the master interrupt flag
// //   IFS1bits.MI2C1IF = 0;
//    // enable the master interrupt
//  //  IEC1bits.MI2C1IE = 1;
   // initialize the hardware
    
    
    
    
    // Baud Rate Generator Value: I2CBRG 78;   
    I2C1BRG = 0x004E;
    // ACKEN disabled; STRICT disabled; STREN disabled; GCEN disabled; SMEN disabled; DISSLW disabled; I2CSIDL disabled; ACKDT Sends ACK; SCLREL Holds; RSEN disabled; A10M 7 Bit; PEN disabled; RCEN disabled; SEN disabled; I2CEN enabled; 
    I2C1CON1 = 0x8200;
    // R_nW disabled; P disabled; S disabled; BCL disabled; I2COV disabled; D_nA disabled; IWCOL disabled; 
    I2C1STAT = 0x0000;

    /* MI2C1 - I2C1 Master Events */
 //    clear the master interrupt flag
    IFS1bits.MI2C1IF = 0;
}

void DRV_I2C1_Initialize(void) {
    I2C1_Initialize();
}




void i2c_start(void) {
    int x = 0;
    I2C1CON1bits.ACKDT = 0; //Reset any previous Ack
    __delay_us(10)

    I2C1CON1bits.SEN = 1; //Initiate Start condition
    Nop();

    //the hardware will automatically clear Start Bit
    //wait for automatic clear before proceding
    while (I2C1CON1bits.SEN) {
        __delay_us(1);
        x++;
        if (x > 20)
            break;
    }
    __delay_us(2);
}

void i2c_stop(void) {
    int x = 0;
    I2C1CON1bits.ACKDT = 0; //Reset any previous Ack
    __delay_us(10)

    I2C1CON1bits.PEN = 1; // Stop Condition Enable bit
    Nop();

    //the hardware will automatically clear stop Bit
    //wait for automatic clear before proceding
    while (I2C1CON1bits.PEN) {
        __delay_us(1);
        x++;
       // UART1PutChar(x);
        if (x > 20)
            break;
    }
    //   I2C1CONbits.PEN = 0; // Stop Condition Enable bit
    __delay_us(2);
}

void i2c_repeatedStart(void) {
    int x = 0;
    I2C1CON1bits.ACKDT = 0; //Reset any previous Ack
    __delay_us(10)

    I2C1CON1bits.RSEN = 1; //Initiate Start condition
    Nop();

    //the hardware will automatically clear Start Bit
    //wait for automatic clear before proceding
    while (I2C1CON1bits.RSEN) {
        __delay_us(1);
        x++;
        if (x > 20)
            break;
    }
    __delay_us(2);
}

void i2c_restart(void) {
    int x = 0;

    I2C1CON1bits.RSEN = 1; //Initiate restart condition
    Nop();

    //the hardware will automatically clear restart bit
    //wait for automatic clear before proceding
    while (I2C1CON1bits.RSEN) {
        __delay_us(1);
        x++;
        if (x > 20) break;
    }

    __delay_us(2);
}
//Resets the I2C bus to Idle

void reset_i2c_bus(void) {
    int x = 0;

    //initiate stop bit
    I2C1CON1bits.PEN = 1;

    //wait for hardware clear of stop bit
    while (I2C1CON1bits.PEN) {
        __delay_us(2);
        x++;
        if (x > 20) break;
    }
    I2C1CON1bits.RCEN = 0;
    IFS1bits.MI2C1IF = 0; // Clear Interrupt
    I2C1STATbits.IWCOL = 0;
    I2C1STATbits.BCL = 0;
    __delay_us(10);
}

//basic I2C byte send

char send_i2c_byte(int data) {
    int i;

    while (I2C1STATbits.TBF) {
    }
    IFS1bits.MI2C1IF = 0; // Clear Interrupt
    I2C1TRN = data; // load the outgoing data byte

    // wait for transmission
    for (i = 0; i < 500; i++) {
        if (!I2C1STATbits.TRSTAT) break;
        __delay_us(1);

    }
    if (i == 500) {
        return (1);
    }

    // Check for NO_ACK from slave, abort if not found
    if (I2C1STATbits.ACKSTAT == 1) {
       // UART1PutStr("no ack-reset");
        reset_i2c_bus();
        return (1);
    }

    __delay_us(2);
    return (0);
}


//function reads data, returns the read data, no ack

char i2c_read(void) {
    int i = 0;
    char data = 0;

    //set I2C module to receive
    I2C1CON1bits.RCEN = 1;

    //if no response, break
    while (!I2C1STATbits.RBF) {
        i++;
        if (i > 2000) break;
    }

    //get data from I2CRCV register
    data = I2C1RCV;

    //return data
    return data;
}

//function reads data, returns the read data, with ack

char i2c_read_ack(void) //does not reset bus!!!
{
    int i = 0;
    char data = 0;

    //set I2C module to receive
    I2C1CON1bits.RCEN = 1;

    //if no response, break
    while (!I2C1STATbits.RBF) {
        i++;
        if (i > 2000) break;
        // UART1PutChar('R');
    }
    i = 0;
    //get data from I2CRCV register
    data = I2C1RCV;

    __delay_us(2);

    //i2c_ack();
    __delay_us(2);

    return data;

}

void i2c_mIdleI2C1(void) {

    while ((I2C1CON1 & 0x001F) != 0) {
      //  UART1PutChar('d');
    }//
    //Wait for Acken, Rcen, Pen, Rsen and Sen to clear


}

void i2c_ack(void) {
    I2C1CON1bits.ACKDT = 0;
    Nop();
    //I2C1CONbits.ACKEN=0;
    I2C1CON1bits.RCEN = 0;
    I2C1CON1bits.PEN = 0;
    I2C1CON1bits.RSEN = 0;
    I2C1CON1bits.SEN = 0;
    //__delay_us(1);       
    I2C1CON1bits.ACKEN = 1;
    while (I2C1CON1bits.ACKEN == 1) {
        //UART1PutChar('A');
    };
}

void i2c_nack(void) {
    I2C1CON1bits.ACKDT = 1;
    Nop();
    //I2C1CONbits.ACKEN=0;
    I2C1CON1bits.RCEN = 0;
    I2C1CON1bits.PEN = 0;
    I2C1CON1bits.RSEN = 0;
    I2C1CON1bits.SEN = 0;
    //__delay_us(1);       
    I2C1CON1bits.ACKEN = 1;
    while (I2C1CON1bits.ACKEN == 1) {
    //    UART1PutChar('N');
    };
}

void si7020test() {
    //i2c_start();  


    send_i2c_byte(0x81);

    send_i2c_byte(0xE3);

    // char cc=i2c_read_ack();




}
