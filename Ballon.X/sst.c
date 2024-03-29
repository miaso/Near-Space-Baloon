
#include "sst.h"
#include "xc.h"

#define INTERRUPT_PRIORITY_MAX                          7
#define INTERRUPT_PRIORITY_EXT                          6
#define INTERRUPT_PRIORITY_RTC                          5
#define INTERRUPT_PRIORITY_TIMER                        4
#define INTERRUPT_PRIORITY_UART                         3
#define INTERRUPT_PRIORITY_ADC                          2
#define INTERRUPT_PRIORITY_DEFAULT                      1
#define INTERRUPT_PRIORITY_DISABLED                     0
/****************/
/* SPI1 defines */
/****************/
//#define SPI1_ENABLE                                     SPI1STATbits.SPIEN = 1
//#define SPI1_DISABLE                                    SPI1STATbits.SPIEN = 0
//#define SPI1_OVERFLOW_FLAG                              SPI1STATbits.SPIROV
//#define SPI1_OVERFLOW_CLEAR                             SPI1STATbits.SPIROV = 0
//#define SPI1_RX_BUFFER_FULL                             SPI1STATbits.SPIRBF
//#define SPI1_TX_BUFFER_FULL                             SPI1STATbits.SPITBF
//#define SPI1_INTERRUPT_FLAG                             IFS0bits.SPI1IF
//#define SPI1_INTERRUPT_CLEAR                            IFS0bits.SPI1IF = 0
//#define SPI1_FAULT_FLAG                                 IFS0bits.SPF1IF
//#define SPI1_FAULT_CLEAR                                IFS0bits.SPF1IF = 0
//#define SPI1_INTERRUPT_PRIORITY                         IPC2bits.SPI1IP
//#define SPI1_ENABLED                                    (SPI1STAT & 0x8000)

static void SPI_Init(void)
{
	CS_PORT = 1;
	SCK_TRIS = 0;
	SDO_TRIS = 0;
	CS_TRIS	 = 0;

   // AUDEN disabled; FRMEN disabled; AUDMOD I2S; FRMSYPW One clock wide; AUDMONO stereo; FRMCNT 0; MSSEN disabled; FRMPOL disabled; IGNROV disabled; SPISGNEXT not sign-extended; FRMSYNC disabled; URDTEN disabled; IGNTUR disabled; 
    SPI1CON1H = 0x0000;
    // WLENGTH 0; 
    SPI1CON2L = 0x0000;
    // SPIROV disabled; FRMERR disabled; 
    SPI1STATL = 0x0000;
    // SPI1BRGL 1; 
    SPI1BRGL = 0x0001;
    // SPITBFEN disabled; SPITUREN disabled; FRMERREN disabled; SRMTEN disabled; SPIRBEN disabled; BUSYEN disabled; SPITBEN disabled; SPIROVEN disabled; SPIRBFEN disabled; 
    SPI1IMSKL = 0x0000;
    // RXMSK 0; TXWIEN disabled; TXMSK 0; RXWIEN disabled; 
    SPI1IMSKH = 0x0000;
    // SPI1URDTL 0; 
    SPI1URDTL = 0x0000;
    // SPI1URDTH 0; 
    SPI1URDTH = 0x0000;
    // SPIEN enabled; DISSDO disabled; MCLKEN FOSC/2; CKP Idle:Low, Active:High; SSEN disabled; MSTEN Master; MODE16 disabled; SMP Middle; DISSCK disabled; SPIFE Frame Sync pulse precedes; CKE Active to Idle; MODE32 disabled; SPISIDL disabled; ENHBUF enabled; DISSDI disabled; 
    SPI1CON1L = 0x8121;

    
//	SSPSTAT = 0x0;
//	SSPCON1 = 0x01;
//
//	SSPSTATbits.SMP = 0;
//	SSPSTATbits.CKE = 1;
//	SSPCON1bits.CKP = 0;
//	SSPCON1bits.SSPEN = 1;
    
    
    
}


static unsigned char SPI_Send_Byte(unsigned char data)
{
	unsigned char res;
    
	CS_LOW
    
      while( SPI1STATLbits.SPITBF == 1 )
    {

    }
    
     SPI1BUFL = ((unsigned char)data);
     
      while ( SPI1STATLbits.SPIRBE == 1)
    {
               
    }
    res  = SPI1BUFL;
     
//	SSPBUF = data;
//	while(!SSPSTATbits.BF);
//	res = SSPBUF;
    
	CS_HIGH
            
	return res;
}


static unsigned char SPI_Send_Cont_Byte(unsigned char data)
{
	unsigned char res;
    
          while( SPI1STATLbits.SPITBF == 1 )
    {

    }
    
     SPI1BUFL = ((unsigned char)data);
     
      while ( SPI1STATLbits.SPIRBE == 1)
    {
               
    }
    res  = SPI1BUFL;
    
//	SSPBUF = data;
//	while(!SSPSTATbits.BF);
//	res = SSPBUF;
    
	return res;
}


/*************************************************
 * Brief: Initialize SPI module and required IO pins.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Init(void)
{
// CON1 = 0x07;
	SDO_PORT = SDI_PORT = 0;
	CS_PORT = 1;
	SPI_Init();
}


/*************************************************
 * Brief: Reads _len_ bytes from address _addr_ and
 *	writes to _*buf_. The read instruction supports up to
 *	25MHz read.
 *
 * Parameters:
 *	unsigned char *buf - buffer to receive data
 *	long addr - valid address in range 0x0 - 0x3fffff
 *	int len - number of bytes to read
 *
 * Return: none
 *************************************************/
void SST_Read(unsigned char *buf, long addr, int len)
{
	CS_LOW
	SPI_Send_Cont_Byte(COM_READ);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x00ff00) >> 8);
	SPI_Send_Cont_Byte(addr & 0x0000ff);
	while(len-- > 0)
		*buf++ = SPI_Send_Cont_Byte(0);
	CS_HIGH
}


/*************************************************
 * Brief: Reads _len_ bytes from address _addr_ and
 *	writes to _*buf_. Supports up to 80MHz read.
 *
 * Parameters:
 *	unsigned char *buf - buffer to receive data
 *	long addr - valid address in range 0x0 - 0x3fffff
 *	int len - number of bytes to read
 *
 * Return: none
 *************************************************/
void SST_High_Speed_Read(unsigned char *buf, long addr, int len)
{
	CS_LOW
	SPI_Send_Cont_Byte(COM_HI_SPEED_READ);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x00ff00) >> 8);
	SPI_Send_Cont_Byte(addr & 0x0000ff);
	SPI_Send_Cont_Byte(0);	//dummy
	while(len-- > 0)
		*buf++ = SPI_Send_Cont_Byte(0);
	CS_HIGH
}


/*************************************************
 * Brief: Allows reading of the Status register.
 *
 * Parameters: none
 *
 * Return:
 *	unsigned char res - Status register
 *************************************************/
unsigned char SST_Read_Status_Register(void)
{
	unsigned char res;
	CS_LOW
	SPI_Send_Cont_Byte(COM_READ_STATUS_REG);
	res = SPI_Send_Cont_Byte(0);
	CS_HIGH
	return res;
}


/*************************************************
 * Brief: Identifies the device as SST25VF032B and
 * manufacturer as SST.
 *
 * Parameters:
 *	unsigned char *buf - buffer to receive data
 *
 * Return: none
 *************************************************/
void SST_Read_ID(unsigned char *buf)
{
	CS_LOW
	SPI_Send_Cont_Byte(COM_READ_ID);
	SPI_Send_Cont_Byte(0);
	SPI_Send_Cont_Byte(0);
	SPI_Send_Cont_Byte(0);
	*buf++ = SPI_Send_Cont_Byte(0);
	*buf = SPI_Send_Cont_Byte(0);
	CS_HIGH
}


/*************************************************
 * Brief: Sets the Write-Enable-Latch bit in the Status register
 * to '1' allowing Write operations to occur.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Write_Enable(void)
{
	SPI_Send_Byte(COM_WRITE_ENABLE);
}


/*************************************************
 * Brief: Resets the Write-Enable-Latch bit and AAI bit to '0',
 * therefore, preventing any new Write operations.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Write_Disable(void)
{
	SPI_Send_Byte(COM_WRITE_DISABLE);
}


/*************************************************
 * Brief: Opens the status register for alteration.
 * The Write-Status-Register instruction must be executed
 * immediately after the execution of the
 * Enable-Write-Status-Register instruction.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Enable_Write_Status_Register(void)
{
	SPI_Send_Byte(COM_ENABLE_WRITE_STATUS_REG);
}


/*************************************************
 * Brief: Programs the bits _b_ in the selected by _addr_ byte
 * to the desired data. The selected byte must be in the erased
 * state (FFH) when initiating a Program operation.
 *
 * Parameters:
 *	unsigned char b - byte to be programmed
 *	long addr - valid address in range 0x0 - 0x3fffff
 *
 * Return: none
 *************************************************/
void SST_Byte_Program(unsigned char b, long addr)
{
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_BYTE_PROGRAM);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x00ff00) >> 8);
	SPI_Send_Cont_Byte(addr & 0x0000ff);
	SPI_Send_Cont_Byte(b);	//DIN
	CS_HIGH
	SST_Write_Disable();
}


/*************************************************
 * Brief: Resets BP2, BP1 and BP0.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Clear_Write_Protection(void)
{
	unsigned char temp;
	temp = SST_Read_Status_Register();
	temp &= 0xe3;
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_WRITE_STATUS_REG);
	SPI_Send_Cont_Byte(temp);
	CS_HIGH
	SST_Write_Disable();
}


/*************************************************
 * Brief: Clears all bits in the device to 0xff.
 *
 * Parameters: none
 *
 * Return: none
 *************************************************/
void SST_Chip_Erase(void)
{
	SST_Write_Enable();
	SPI_Send_Byte(COM_CHIP_ERASE);
	//pool busy
	while((SST_Read_Status_Register() & 0x01) == 1);
	SST_Write_Disable();
}


/*************************************************
 * Brief: Clears all bits in the selected 4 KByte sector to 0xff.
 *
 * Parameters:
 *	long addr - valid address in range 0x0 - 0x3ff8xx
 *
 * Return: none
 *************************************************/
void SST_Sector_Erase(long addr)
{
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_4KB_SECOTR_ERASE);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x00ff00) >> 8);
	SPI_Send_Cont_Byte(0);
	CS_HIGH
	//pool busy
	while((SST_Read_Status_Register() & 0x01) == 1);
	SST_Write_Disable();
}


/*************************************************
 * Brief: Clears all bits in the selected 32 KByte block to 0xff.
 *
 * Parameters:
 *	long addr - valid address in range 0x0 - 0x3f8xxx
 *
 * Return: none
 *************************************************/
void SST_32KB_Block_Erase(long addr)
{
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_32KB_BLOCK_ERASE);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x008000) >> 16);
	SPI_Send_Cont_Byte(0);
	CS_HIGH
	//pool busy
	while((SST_Read_Status_Register() & 0x01) == 1);
	SST_Write_Disable();
}


/*************************************************
 * Brief: Clears all bits in the selected 64 KByte block to 0xff.
 *
 * Parameters:
 *	long addr - valid address in range 0x0 - 0x3fxxxx
 *
 * Return: none
 *************************************************/
void SST_64KB_Block_Erase(long addr)
{
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_64KB_BLOCK_ERASE);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte(0);
	SPI_Send_Cont_Byte(0);
	CS_HIGH
	//pool busy
	while((SST_Read_Status_Register() & 0x01) == 1);
	SST_Write_Disable();
}


/*************************************************
 * Brief: Writes new values to the BP3, BP2, BP1, BP0,
 * and BPL bits of the status register.
 *
 * Parameters:
 *	unsigned char val - new value to be written
 *
 * Return: none
 *************************************************/
void SST_Write_Status_Register(unsigned char val)
{
	SST_Enable_Write_Status_Register();
	CS_LOW
	SPI_Send_Cont_Byte(COM_WRITE_STATUS_REG);
	SPI_Send_Cont_Byte(val);
	CS_HIGH
}


/*************************************************
 * Brief: Identifies the device as SST25VF032B and
 * manufacturer as SST.
 *
 * Parameters:
 *	unsigned char *buf - buffer to receive data
 *
 * Return: none
 *************************************************/
void SST_Read_Jedec_ID(unsigned char *buf)
{
	CS_LOW
	SPI_Send_Cont_Byte(COM_READ_JEDEC_ID);
	*buf++ = SPI_Send_Cont_Byte(0);
	*buf++ = SPI_Send_Cont_Byte(0);
	*buf = SPI_Send_Cont_Byte(0);
	CS_HIGH
}


/*************************************************
 * Brief: The AAI program instruction allows multiple bytes
 * of data to be programmed without re-issuing the
 * next sequential address location.
 *
 * Parameters:
 *	unsigned char *buf - buffer containing data to be send
 *	long addr - starting address
 *	int len - number of bytes to be written. No less than 2 bytes
 *
 * Return: none
 *************************************************/
void SST_AAI_Soft_EoW(unsigned char *buf, long addr, int len)
{
	int i;
	SST_Write_Enable();
	CS_LOW
	SPI_Send_Cont_Byte(COM_AAI_WORD_PROGRAM);
	SPI_Send_Cont_Byte((addr & 0xff0000) >> 16);
	SPI_Send_Cont_Byte((addr & 0x00ff00) >> 8);
	SPI_Send_Cont_Byte(addr & 0x0000ff);
	SPI_Send_Cont_Byte(*buf);	//D0
	buf++;
	SPI_Send_Cont_Byte(*buf);	//D1
	buf++;
	len -= 2;
	CS_HIGH
	if(len > 0)
	{
		if(len % 2 == 0)
		{
			while(len > 0)
			{
				CS_LOW
				SPI_Send_Cont_Byte(COM_AAI_WORD_PROGRAM);
				SPI_Send_Cont_Byte(*buf);
				buf++;
				SPI_Send_Cont_Byte(*buf);
				buf++;
				len -= 2;
				CS_HIGH
				//pool busy
				while((SST_Read_Status_Register() & 0x01) == 1);
			}
		}
		else
		{
			while(len > 0)
			{
				CS_LOW
				SPI_Send_Cont_Byte(COM_AAI_WORD_PROGRAM);
				SPI_Send_Cont_Byte(*buf);
				buf++;
				len--;
				if(len == 0)
				{
					SPI_Send_Cont_Byte(0);
					break;
				}
				SPI_Send_Cont_Byte(*buf);
				buf++;
				len--;
				CS_HIGH
				//pool busy
				while((SST_Read_Status_Register() & 0x01) == 1);
			}
		}
	}
	CS_HIGH
	SST_Write_Disable();
}