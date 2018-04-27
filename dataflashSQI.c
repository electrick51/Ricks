/**
 * Bit-bang drivers for the Microchip SQI mode serial data flash chips
 * connected to port RG
 */

#include "twttsdef.h"
#include "exactint.h"
#include "dataflash.h"
#define Sio0Mask 0x0040 // Sio0 == RG6 
#define Sio1Mask 0x0080 // Sio1 == RG7 
#define Sio2Mask 0x0100 // Sio2 == RG8  
#define Sio3Mask 0x0200 // Sio3 == RG9 
#define SioMask 0x03c0  // RG9,RG8,RG7,RG6
#define SclkMask 0x0008 // Sclk == RG3
#define SceMask  0x0004 // Sce == RG2
#define Sio0BitNum 6
#define Sio1BitNum 7
#define Sio2BitNum 8
#define Sio3BitNum 9
 // entire flash chip write protected
 // except 002000-005fff     4 sectors
 //        700000-7fffff   256 sectors
// refer to dfformat.c for hardcoded flash addresses
 // block_protection_18[] sent to flash chip in order [17],[16],[15], ... [0]
 //                                       [0]    [1]                           //                                      b7-b0, b15-b8,                                                                                       b143-b136                         
const uint8_t block_protection_18[18] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x40, 0x41, 0x00};

static void LockBlockProtection(void);
static void WriteBlockProtection(void);

#if 0
static void CheckBlockProtection(void);
static char quickCheck(void);
static void flashTest(void);
#endif

static void sFlashWaitBusy(void);
static void sFlashWREN(void);
static void flashPortassign(void);
//static void ResetEn(void);
//static void Reset(void);
static void SPI_ResetEn(void);
static void SPI_Reset(void);
static void SPI_Reset_QIO(void);
//static void Reset_QIO();
static void En_QIO(void);
static void SendSQI_Byte(unsigned char out);
static void Send_Byte(unsigned char out);
static unsigned char Read_Status_Register(void);
static inline void SCK_High(void);
static inline void SCK_Low(void);
static inline void SetSIO0_Output(void);
static inline void SetSIO_Output(void);
static inline void SetSIO_Input(void);
static inline void CE_Low(void);
static inline void WP_High(void);
static inline void Reset_Hold_High(void);
static inline void init(void);
static inline void SIO0_High(void);
static inline void SIO0_Low(void);
static inline void SIO_High(void);
static inline void SIO_Low(void);
#if 0
// these two arrays simulate the two flash chip buffers that
// are on the Atmel type flash chip but are not on the Microchip flash
unsigned char fBuffer1[528], fBuffer2[528];
#endif
uint16_t sFlashPageSize;
#if 0
static void enable_sector_protection(void)
{
}

static void setup_sector_protection_reg(void)
{
}
#endif

/**
 * Initialize the DataFlash.
 */
void flash_setup(void)
{
    sFlashPageSize = SFLASHPAGESIZE;
    flashPortassign();
    CE_High();
    WP_High();
    Reset_Hold_High();
    init(); // set clock low
    // the next lines get the chip into a known state
    SPI_Reset_QIO(); // this should exit QIO mode if enabled
    SPI_ResetEn(); // reset the chip
    SPI_Reset();
    SPI_ResetEn(); // twice sayeth Microchip
    SPI_Reset();
    init(); // set clock low
    En_QIO(); // put serial flash chip qio mode
    WriteBlockProtection();
    LockBlockProtection();
    // comment out next three lines before release
#if 0
    CheckBlockProtection();    // note - never returns if test fails
    quickCheck();
    flashTest();               // note - never returns if test fails
#endif
}



//
// flash_page_update - write data to flash using page program cmd 
// number of bytes to write 0 < num <= 256 
// this function reads a page of data from the flash to a ram buffer
// merges new data into the ram buffer
// and writes the data back to the data flash chip
// for an address of aaaabb the page address is aaaa00
// the offset into the page is bb
// data written beyond the end of the page wraps around to the start of the page
// note: erases entire 4k sector containing address
void flash_page_update(unsigned long address, uint8_t *pData, unsigned num)
{
    unsigned int i, n,offset;
    unsigned char tmp[FLASH_PAGE_SIZE];
    
    if (num == 0)
    {
        return;
    }
    if (num > FLASH_PAGE_SIZE)
    {
        n = FLASH_PAGE_SIZE;
    }
    else
    {
        n = num;
    }   
    // read existing page to data ram buffer tmp
    flash_read(address & 0xffffff00L, tmp, FLASH_PAGE_SIZE);
    offset=(unsigned char)(address&0xffL);       
    for (i = 0; i < n; i++)
    {
        tmp[((offset+i)&0xff)]=*pData++;
    }         
    sFlashEraseSector(address & 0xffffff00L);         
    flash_page_write(address & 0xffffff00, tmp, FLASH_PAGE_SIZE);   
}

#if 1
//
// flashWwrite - write data to flash using page program cmd 
// number of bytes to write 0 < num <= 256 
// dstAddress is start of write block
// addresses to write must have been erased first

void flash_page_write(unsigned long address, uint8_t *pData, unsigned num)
{
    unsigned int i, n;
    if (num == 0)
    {
        return;
    }
    if (num > 256)
    {
        n = 256;
    }
    else
    {
        n = num;
    }
    
    sFlashWREN();
    CE_Low(); /* enable device */
    SendSQI_Byte(0x02); /* send Page Program command */
    SendSQI_Byte(((address & 0xFFFFFF) >> 16)); /* send 3 address bytes */
    SendSQI_Byte(((address & 0xFFFF) >> 8));
    SendSQI_Byte(address & 0xFF);
    for (i = 0; i < n; i++)
    {
        SendSQI_Byte(*pData++);
    }
    CE_High(); /* disable device */
    sFlashWaitBusy();
}
#endif


// flashRead reads an arbitrary size chunk of flash to
// an arbitrary ram address. num is number of bytes to read 0 < num < 65536
// read num bytes from flash starting at dstAddress to *pData
void flash_read(unsigned long dstAddress, unsigned char* pData, unsigned int num)
{
    unsigned int i;
    register uint8_t dataByte asm("w0");  //register selected must match return value of GetSQI_Byte_Macro
    
    CE_Low(); /* enable device */
    SendSQI_Byte(0x0B); /* hs read command */
    SendSQI_Byte(((dstAddress & 0xFFFFFF) >> 16)); /* send 3 address bytes */
    SendSQI_Byte(((dstAddress & 0xFFFF) >> 8));
    SendSQI_Byte(dstAddress & 0xFF);
    SendSQI_Byte(0xCF); /* dummy byte (does not set "mode") */
    SendSQI_Byte(0xFF); /*dummy byte*/
    SendSQI_Byte(0xFF); /*dummy byte*/
    //SetSIO_Input();
    for (i = 0; i < num; i++) /* read until no_bytes is reached */
    {
        GetSQI_Byte_Macro();//Assume GetSQI_BYTE_MACRO returns result in W0 GetSQI_Byte();        
        *pData++ = dataByte; //Note above macro returns result in W0 and dataByte is defined to be W0
    }
    CE_High();
}

/*
 * Sets up the flash for a sequential data read.
 * Caution: this leaves the FLASH chip selected!
 * return with rg9-rg6 input mode
 * CE and SCLK output low
 * use GetSQI_Byte_Macro to read the data from flash
 *
 */
 void flash_setup_read(unsigned long dstAddress)
{
    CE_Low(); /* enable device */
    SendSQI_Byte(0x0B); /* high speed read command */
    SendSQI_Byte(((dstAddress & 0xFFFFFF) >> 16)); /* send 3 address bytes */
    SendSQI_Byte(((dstAddress & 0xFFFF) >> 8));
    SendSQI_Byte(dstAddress & 0xFF);
    SendSQI_Byte(0xCF); /*dummy byte (does not set "mode") */
    SendSQI_Byte(0xFF); /*dummy byte*/
    SendSQI_Byte(0xFF); /*dummy byte*/
}

/************************************************************************/
/* PROCEDURE: flashWREN							*/
/*									*/
/* This procedure enables the Write Enable Latch.               	*/

/************************************************************************/
static void sFlashWREN(void)
{
    CE_High(); /* just in case */
    CE_Low(); /* enable device */
    SendSQI_Byte(0x06); /* send WREN command */
    CE_High(); /* disable device */
}
#if 0
/************************************************************************/
/* PROCEDURE:	Global Block Protection Unlock				*/
/*									*/
/* This procedure clears all block protection				*/

/************************************************************************/
static void flashGlobalBlockProtectionUnlock(void)
{
    sFlashWREN();
    CE_Low(); /* enable device */
    SendSQI_Byte(0x98); /* read command */
    CE_High(); /* disable device */
    sFlashWaitBusy();
}
#endif

/************************************************************************/
/* PROCEDURE: flashWaitBusy							*/
/*									*/
/* This procedure waits until device is no longer busy (can be used by	*/
/* Byte-Program, Page-Program, Sector-Erase, Block-Erase and Chip-Erase).*/

/************************************************************************/

static void sFlashWaitBusy(void)
{
    char Read_byte = 0x81;
    while (((Read_byte & 0x81) == 0x81) || ((Read_byte & 0x81) == 0x80) || ((Read_byte & 0x81) == 0x01)) // waste time until not busy
        Read_byte = Read_Status_Register();
}

/************************************************************************/
/* PROCEDURE: Chip_Erase						*/
/*									*/
/* This procedure erases the entire Chip.				*/
/************************************************************************/
#if 0
static void flashChipErase(void)
{
    flashGlobalBlockProtectionUnlock();
    sFlashWREN();
    CE_Low(); /* enable device */
    SendSQI_Byte(0xC7); /* send Chip Erase command (C7h) */
    CE_High(); /* disable device */
    sFlashWaitBusy();
}
#endif
/************************************************************************/
/* PROCEDURE: CE_High							*/
/*									*/
/* This procedure set CE = High.					*/
/************************************************************************/
// CE = RG2

inline void CE_High(void)
{
//    LATG = LATG | (SceMask); /* set CE high */
}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*									*/
/* This procedure drives the CE of the device to low.  			*/

/************************************************************************/
static inline void CE_Low(void)
{
//    LATG = LATG & ~(SceMask); /* clear CE low */
}

/************************************************************************/
// PROCEDURE: Port Assign						*/
//									*/
// This procedure sets the Port for SPI communication 	*/
// LATG is for bit banged SPI communication.		*/

/************************************************************************/
static void flashPortassign(void)
{

//    LATG = 0x0000;
//    TRISG = 0x0000; // Set everything to be output low

}


/************************************************************************/
/* PROCEDURE: WP_High()							*/
/*									*/
/* This procedure sets the WP pin to high.				*/

/************************************************************************/
static inline void WP_High(void)
{
//    LATG = LATG | (Sio2Mask); /* set WP pin */
}

/************************************************************************/
/* PROCEDURE: Reset_Hold_High()						*/
/*									*/
/* This procedure sets the Reset_Hold pin to high.			*/

/************************************************************************/
static inline void Reset_Hold_High(void)
{
//    LATG = LATG | (Sio3Mask); /* set Hold pin */
}




/************************************************************************/
/* PROCEDURE: ResetEn                                                   */
/*									*/
/* This procedure Enables acceptance of the RST (Reset) operation.	*/
/************************************************************************/
#if 0
static void ResetEn()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x66);
	CE_High();				/* disable device */
}
#endif
static void SPI_ResetEn()
{
	CE_Low();				/* enable device */
	Send_Byte(0x66);
	CE_High();				/* disable device */
}




/************************************************************************/
/* PROCEDURE: Reset                                     		*/
/*									*/
/* This procedure resets the device in to normal operating Ready mode.	*/
/*									*/
/************************************************************************/
#if 0
static void Reset()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x99);
	CE_High();				/* disable device */
}
#endif
static void SPI_Reset()
{
	CE_Low();				/* enable device */
	Send_Byte(0x99);
	CE_High();				/* disable device */
}



/************************************************************************/
/* PROCEDURE: En_QIO                                    		*/
/*									*/
/* This procedure enables quad I/O operation.           		*/

/************************************************************************/
static void En_QIO(void)
{
    CE_Low(); /* enable device */
    Send_Byte(0x38);
    CE_High(); /* disable device */
}


/************************************************************************/
/* PROCEDURE: Reset_QIO                                 		*/
/*									*/
/* This procedure resets the device to 1-bit SPI protocol operation.    */
/************************************************************************/
#if 0
static void Reset_QIO()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0xff);
	CE_High();				/* disable device */
}
#endif
static void SPI_Reset_QIO()
{
	CE_Low();				/* enable device */
	Send_Byte(0xff);
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE: SendSQI_Byte						*/
/*									*/
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI0, SIO1, SIO2, SIO2 pin (LSB 1st).			*/
/* returns with Sio pins inputs */

/************************************************************************/
static void SendSQI_Byte(unsigned char out)
{
    SetSIO_Output();
    SIO_Low();
//    LATG = LATG | (((unsigned int) (out & 0xf0)) << 2); // ms nibble
    SCK_High(); /* toggle clock high */
    SCK_Low(); /* toggle clock low */
    SIO_Low();
//    LATG = LATG | (((unsigned int) (out & 0x0f)) << 6); // ls nibble
    SCK_High(); /* toggle clock high */
    SIO_High();
    SetSIO_Input();
    SCK_Low(); /* toggle clock low */


}

/************************************************************************/
/* PROCEDURE: SIO0_High							*/
/*									*/
/* This procedure set SIO0 = High.					*/
/************************************************************************/
// SIO0 = RG6

static inline void SIO0_High(void)
{
//    LATG = LATG | (Sio0Mask); /* set SIO0 high */
}

/************************************************************************/
/* PROCEDURE: SIO0_Low							*/
/*									*/
/* This procedure drives the SIO0 of the device to low. 		*/

/************************************************************************/
static inline void SIO0_Low(void)
{
//    LATG = LATG & ~(Sio0Mask); /* clear SIO0 low */
}

/************************************************************************/
/* PROCEDURE: SIO_High							*/
/*									*/
/* This procedure set all 4 SIO = High.					*/

/************************************************************************/
static inline void SIO_High(void)
{
//    LATG = LATG | (SioMask); /* set SIO bits high */
}

/************************************************************************/
/* PROCEDURE: SIO_Low							*/
/*									*/
/* This procedure drives the SIO bits of the device to low. 		*/

/************************************************************************/
static inline void SIO_Low(void)
{
//    LATG = LATG & ~(SioMask); /* clear SIO bits low */
}

/************************************************************************/
/* PROCEDURE: init							*/
/*									*/
/* This procedure initializes the SCK to low. Must be called prior to 	*/
/* setting up mode 0.							*/

/************************************************************************/
static inline void init(void)
{
    SCK_Low(); /* set clock to low initial state */
}


/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*									*/
/* This procedure reads the status register and returns the byte.	*/

/************************************************************************/
static unsigned char Read_Status_Register(void)
{
    unsigned char byte = 0;
    register uint8_t dataByte asm("w0");  //register selected must match return value of GetSQI_Byte_Macro
    CE_Low(); /* enable device */
    SendSQI_Byte(0x05); /* send RDSR command */
    SendSQI_Byte(0xff); /* send RDSR command */
    //SetSIO_Input();
    GetSQI_Byte_Macro();
    byte = dataByte;
    CE_High(); /* disable device */
    return byte;
}
/************************************************************************/
/* PROCEDURE: SCK_High							*/
/*									*/
/* This procedure set SCK = High.					*/
/************************************************************************/
// sclk = RG3

static inline void SCK_High(void)
{
//    LATG = LATG | (SclkMask); /* set SCK high */
}

/************************************************************************/
/* PROCEDURE: SCK_Low							*/
/*                                                      		*/
/* This procedure drives the SCK of the device to low.  		*/

/************************************************************************/
static inline void SCK_Low(void)
{
//    LATG = LATG & ~(SclkMask); /* clear SCK low */
}
/************************************************************************/
/* PROCEDURE: SetSIO0_Output, SetSIO0_Input				*/
/*									*/
/* This procedure sets the Ports for SQI communicaiton          	*/
/************************************************************************/
// sio0 = RG6

static inline void SetSIO0_Output(void)
{
//    TRISG = 0;  //TRISG & ~(Sio0Mask);
}

// sio3-sio0 = RG9-RG6

static inline void SetSIO_Output(void)
{
//    TRISG = 0; // TRISG & ~(SioMask);
}

static inline void SetSIO_Input(void)
{
//    TRISG = SioMask; // TRISG | (SioMask);
}

/************************************************************************/
/* PROCEDURE: Send_Byte		(in SPI mode)            					*/
/*									                                    */
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI pin (SIO0 pin) LSB 1st.	             			*/

/************************************************************************/
static void Send_Byte(unsigned char out)
{

    unsigned char i = 0;
    SetSIO0_Output();

    for (i = 0; i < 8; i++)
    {

        if ((out & 0x80) == 0x80) /* check if MSB is high */
            SIO0_High();
        else
            SIO0_Low(); /* if not, set to low */
        SCK_High(); /* toggle clock high */
        out = (out << 1); /* shift 1 place for next bit */
        SCK_Low(); /* toggle clock low */
    }
}

#if 0
/* read & verify chip params
 *  and serial flash chip interface
 * return 1 if ok, return 0 if fails
 */
static char quickCheck(void)
{
    char temp;
    unsigned char ManufacturerID = 0;
    unsigned char DeviceTYPE = 0;
    unsigned char DeviceID = 0;
    register uint8_t dataByte asm("w0");  //register selected must match return value of GetSQI_Byte_Macro
    /* QuadJ_ID_Read(); */
    CE_Low(); /* enable device */
    SendSQI_Byte(0xAF); /* send read ID command  */
    SendSQI_Byte(0xFF);
    //SetSIO_Input();
    GetSQI_Byte_Macro(); /* receive byte */
    ManufacturerID = dataByte; 
    GetSQI_Byte_Macro(); /* receive byte */   
    DeviceTYPE = dataByte;
    GetSQI_Byte_Macro(); /* receive byte */
    DeviceID = dataByte;     
    CE_High();

    if ((ManufacturerID == 0xbf)&& (DeviceTYPE == 0x26) && (DeviceID == 0x43))
    {
        temp = 1;
    }
    else
    {
        temp = 0;
    } // should not get here
    return temp;
}
#endif
#if  0
/************************************************************************/
/* PROCEDURE: GetSQI_Byte						*/
/* inline version
/ * 									*/

/************************************************************************/
inline unsigned char GetSQI_Byte(void)
{
    unsigned int temp, in;
    //TRISG = SioMask; // SetSIO_Input();
    
   #if 0  
      //asm(" mov #0x3c0,w4
      //asm(" mov w4,TRISG");
      asm(" mov PORTG,w5");
      asm(" bset LATG,#3");
      asm(" bclr LATG,#3");
      asm(" lsr w5,#2,w5");
      asm(" mov PORTG,w4");
      asm(" bset LATG,#3");
      asm(" bclr LATG,#3");
      asm(" lsr w4,#6,w4");
      asm(" ior.b w4,w5,w4");
#endif 
    
#if 1 
    TRISG = SioMask; // SetSIO_Input();
    temp = (PORTG & SioMask);
    LATG = LATG | (SclkMask); // SCK_High();
    LATG = LATG & ~(SclkMask); //SCK_Low();
    temp = temp >> 2; // ms nibble
    // T data valid 5-8 ns from fall of SCK 
    in = (PORTG & SioMask);
    LATG = LATG | (SclkMask); // SCK_High();
    LATG = LATG & ~(SclkMask); //SCK_Low();
    in = in>>Sio0BitNum; // ls nibble
    in = in | temp;
    
    return ((unsigned char) in);
#endif
}
#endif


#if 0
// write sFlash chip buffer to main flash memory array
// with built in erase
// data should be in the flash chip ram buffer
// returns 0 if error, 1 if success
// write starts on page boundary
// buffers now in  data ram

uint8_t write_buffer_to_flash(uint16_t page, uint8_t buffer_num)
{
    unsigned long address;

    address = (unsigned long)page * (unsigned long)sFlashPageSize;
    // ignore attempts to write outside of flash
    if ((address + 256L) >= FLASH_SIZE)
    {
        return 0;
    }
    if (buffer_num == 1)
    {
        flash_write(address, &fBuffer1[0], 256);
    }
    else
    {
        flash_write(address, &fBuffer2[0], 256);
    }
    return 1;
}
#endif
// test functions for craig g
//
// flash_buffers_write - write data to the flash chip page buffer(s)
// assumes 528 byte page size for now
// write starts at offset (0,527)
// buffer_num = 1 or 2
#if 0
uint8_t write_to_flash_buffer(const unsigned char* pData, uint16_t num_bytes, uint16_t offset, uint8_t buffer_num)
{
    uint16_t num, i;
    // the Microchip flash chips do not have memory buffers
    // so replace these with two buffers in processor data memory
    if (num_bytes > 256)
    {
        num = 256;
    }
    else
    {
        num = num_bytes;
    }

    if (buffer_num == 1)
    {
        for (i = 0; i < num; ++i)
        {

            fBuffer1[offset + i] = *pData++;
        }
    }
    else
    {
        for (i = 0; i < num; ++i)
        {

            fBuffer2[offset + i] = *pData++;
        }

    }
    return 1;
}
#endif
#if 0
/* flash rewrite  
 * the sector or sectors to be written will be erased first.
 * all erase operations erase in multiple of 4096 bytes
 * write num bytes where 0 < num < 65536 
 * num bytes are written from *pData to flash starting at address
 */
void flash_rewrite(unsigned long address, uint8_t *pData, unsigned num)
{
    if (num == 0)
    {
        return;
    }
    sFlashEraseSectors(address,num);
    flash_write(address, pData, num);
}
#endif

/* erase n contiguous 4k sector(s) in serial flash
 * starting at the sector including address
 * and ending with the sector including address + num
 * where num is number of bytes 0 < num < 65536
 */
void sFlashEraseSectors(unsigned long address,unsigned int num)
{
    unsigned int i, n, r;
    unsigned long temp;
    if (num == 0)
    {
        return;
    }
    temp = address;   // address aaaxxx will erase aaa000 to aaafff, xxx are don't cares
    n = num >> 12;    // n = num / 4096;   // erase n sectors
    r = num & 0xfff;  // num % 4096;
    if (r != 0)
    {
        ++n;
    }
    for (i = 0; i < n; ++i)
    {
        sFlashEraseSector(temp);
        temp += FLASH_SECTOR_SIZE;
    }
}

#if 0
/*
* flash_write - write data to sFlash using page program cmd 
* number of bytes to write 0 < num < 65536
* address is start of write block
* assumes addresses to write have been erased previously
* num bytes are written from *pData to flash starting at address
*/ 
void flash_write(unsigned long address, const char* pData, unsigned int num)
{
    unsigned int i, n, r;
    unsigned long temp;
    char* tPtr;     //temp pointer
    
    if (num == 0)
    {
        return;
    }
    // ignore attempts to write outside of chip 
    if ((address + (long) num) > FLASH_SIZE)
    {
        return;
    }
    temp = address;
    tPtr=pData;
    n = num >> 8; // num / 256;
    r = num & 0xff; // num % 256;
    // write n full pages
    for (i = 0; i < n; ++i)
    {
        flash_page_write(temp, (char*) tPtr, 256);
        temp = temp + 256L;
        tPtr+=256;
    }

    // write 1 partial page of r bytes
    if (r != 0)
    {
        flash_page_write(temp, (char*) tPtr, r);
    }

}
#endif


/*
* flash_write - write data to sFlash using page program cmd 
* number of bytes to write 0 < num < 65536
* address is start of write block
* assumes addresses to write have been erased previously
* num bytes are written from *pData to flash starting at address
*/ 
void flash_write(unsigned long address, uint8_t *pData, unsigned int num)
{
    unsigned int n;
     //unsigned long temp;
     if (num == 0)
     {
         return;
     }
     // ignore attempts to write outside of flash chip
     if ((address + (unsigned long) num) >= FLASH_SIZE)
     {
         return;
     }
     n = 0;
     sFlashWREN();
     CE_Low();                                  // enable device
     SendSQI_Byte(0x02);                        // send Page Program command
     SendSQI_Byte(((address & 0xFFFFFF) >> 16));// send 3 address bytes
     SendSQI_Byte(((address & 0xFFFF) >> 8));
     SendSQI_Byte(address & 0xFF);
     while (n < num)
     {
         SendSQI_Byte(*pData++);
         ++n;
         ++address;
         if (((address & 0xffUL) == 0)&&(n < num))
         {
             // just crossed a page boundary
             // &  still have unwritten data
             // write page
             CE_High(); /* disable device */
             sFlashWaitBusy(); // wait for write
             // start next page
             sFlashWREN(); // set write enable
             CE_Low(); /* enable device */
             SendSQI_Byte(0x02); /* send Page Program command */
             SendSQI_Byte(((address & 0xFFFFFF) >> 16)); /* send 3 address bytes */
             SendSQI_Byte(((address & 0xFFFF) >> 8));
             SendSQI_Byte(address & 0xFF);
         }

     }
     // write last page
     CE_High();             //disable device
     sFlashWaitBusy();      // wait for write
}

/* erase a 4k sector, address can be any address in the sector
 * address = AAAXXX where AAA is 000-7FF and XXX is don't care
 * erases entire 4k sector from aaa000 to aaafff
 * the sector is assumed to be unlocked
 */
void sFlashEraseSector(unsigned long address)
{
    if (address < FLASH_SIZE)
    {
        sFlashWREN();
        CE_Low(); /* enable device */
        SendSQI_Byte(0x20); /* send Erase 4k Sector command */
        SendSQI_Byte(((address & 0xFFFFFF) >> 16)); /* send 3 address bytes */
        SendSQI_Byte(((address & 0xFFFF) >> 8));
        SendSQI_Byte(address & 0xFF);
        CE_High(); /* disable device */
        sFlashWaitBusy();
    }
}
#if 0
// quick test of the flash chip, flash chip i/o and this firmware
// does not return if test fails
// overwrites last sector in the flash chip 0x7ff000 to 0x7fffff

static void flashTest(void)
{
    unsigned long address;
    unsigned int good, ungood, i, j;
    unsigned char tData[256];
    good = 0;
    ungood = 0;
    address = 0x7ff000L;
    // Test 1 
    // erase last sector
    sFlashEraseSectors(address,1); // erase last sector in the flash chip
    // verify erase    
    for (j = 0; j < 16; ++j)
    {
        for (i = 0; i < 256; ++i)
        {
            tData[i] = 0;
        }
        flash_read(address, (unsigned char*) tData, 256);
        for (i = 0; i < 256; ++i)
        {
            if (tData[i] == 0xff)
            {
                ++good;
            }
            else
            {
                ++ungood;
            }
        }
        address += 256L;
    }
    // Test 2
    // write last sector 15 full pages
    address = 0x7ff000L;
    for (i = 0; i < 256; ++i)
    {
        tData[i] = i;
    }
    for (i = 0; i < 15; ++i)
    {
        flash_write(address, (uint8_t *)tData, 256);
        address += 256L;
    }
    // verify write 15 pages
    address = 0x7ff000L;
    for (j = 0; j < 15; ++j)
    {
        for (i = 0; i < 256; ++i)
        {
            tData[i] = 0;
        }
        flash_read(address, (uint8_t *)tData, 256);
        for (i = 0; i < 256; ++i)
        {
            if (tData[i] == i)
            {
                ++good;
            }
            else
            {
                ++ungood;
            }
        }
        address += 256L;
    }
    // Test 3    
    // write partial page
    for (i = 0; i < 256; ++i)
    {
        tData[i] = (255 - i);
    }
    flash_write(0x7fff00L, (uint8_t *)tData, 128);
    for (i = 0; i < 256; ++i)
    {
        tData[i] = 0;
    }
    flash_read(0x7fff00L, (uint8_t *) tData, 256);
    for (i = 0; i < 128; ++i)
    {
        if (tData[i] == (255 - i))
        {
            ++good;
        }
        else
        {
            ++ungood;
        }
    }
    // verify last 128 bytes still erased
    for (i = 128; i < 256; ++i)
    {
        if (tData[i] == 0xff)
        {
            ++good;
        }
        else
        {
            ++ungood;
        }
    }
  while ((good == 0)||(ungood != 0))
    {
    // loops here forever on failure
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    //return;
    }
}
#endif



/************************************************************************/
/* PROCEDURE:	WriteBlockProtection					*/
/*									*/
/* This procedure writes to block protection register			*/
/*									*/

/************************************************************************/
static void WriteBlockProtection(void)
{

    unsigned char i;
    sFlashWREN();

    CE_Low(); /* enable device */
    SendSQI_Byte(0x42); /* write block protection register command */

    for (i = 18; i > 0; i--)
    {
        SendSQI_Byte(block_protection_18[i - 1]);
    }
    CE_High(); /* disable device */
    sFlashWaitBusy();
}
#if 0
/************************************************************************/
/* PROCEDURE:	ReadBlockProtection			  		*/
/*									*/
/* This procedure verify block protection register			*/
/*									*/
/************************************************************************/
static void CheckBlockProtection(void)
{
    register uint8_t dataByte asm("w0"); // assign dataByte to w0
    unsigned int i;
    unsigned char good = 0;
    unsigned char ungood = 0;
    CE_Low(); /* enable device */
    SendSQI_Byte(0x72);   // read block protection reg command
    SendSQI_Byte(0xff);
    for (i = 18; i > 0; i--)
    {
        GetSQI_Byte_Macro();
        if (block_protection_18[i - 1] == dataByte)
        {
            ++good;
        }
        else
        {
            ++ungood;
        }
    }
    CE_High(); /* disable device */
    while ((good == 0)||(ungood != 0))
    {
    // loops here forever on failure
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    //return;
    }
}
#endif
/************************************************************************/
/* PROCEDURE:	LockBlockProtection					*/
/*									*/
/* This procedure locks the block protection register			*/
/* must power cycle the chip to unlock block protection register        */
/************************************************************************/

static void LockBlockProtection(void)
{
    sFlashWREN();
    CE_Low(); /* enable device */
    SendSQI_Byte(0x8d); /* lock down block protection register command */
    CE_High(); /* disable device */
    sFlashWaitBusy();
}
