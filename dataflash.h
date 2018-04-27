#ifndef DATAFLASH_H
#define DATAFLASH_H
#include "exactint.h"
// for Atmel/Adesto data flash chips 
// change below to #if 0  and
// remove "dataflashSQI.c" and add "dataflash.c to project"
#if 1
// for 64 Meg Microchip Data Flash SST26V064B 
// change above to #if 1 and
// remove "dataflash.c" and add "dataflashSQI.c" to project
#define SQI_FLASH
#define FLASH_SIZE 0x800000L     // 8388608 bytes
#define FLASH_NUM_PAGES 32768U   
#define FLASH_PAGE_SIZE 256U
#define FLASH_NUMBER_OF_SECTORS 2048U   
#define FLASH_SECTOR_SIZE 4096UL
#define SFLASHPAGESIZE 256U
extern uint16_t sFlashPageSize;
                        


#define GetSQI_Byte_Macro()\
{ /* rg9-rg6 sb in input mode */\
__asm__ volatile (" mov PORTG,w5");  /* rg9-rg6=data,rg15-rg10,rg5-rg0=0 */\
__asm__ volatile (" bset LATG,#3");  /* clock pulse */\
__asm__ volatile (" bclr LATG,#3");\
__asm__ volatile (" lsr w5,#2,w5"); /* high nibble */\
__asm__ volatile (" mov PORTG,w4");\
__asm__ volatile (" bset LATG,#3"); /* clock pulse */\
__asm__ volatile (" bclr LATG,#3");\
__asm__ volatile (" lsr w4,#6,w4");  /* low nibble */\
__asm__ volatile (" ior.b w4,w5,w0");/* store result in W0 */\
}
// use the following syntax to access the result of the macro:
// register uint8_t dataByte asm("w0"); // assign dataByte to w0 
#if 0
uint8_t write_buffer_to_flash( uint16_t page, uint8_t buffer_num );
uint8_t write_to_flash_buffer( const unsigned char* pData, uint16_t num_bytes, uint16_t offset, uint8_t buffer_num );
#endif
// initialize flash chip and flash chip i/o
void flash_setup(void);

// erase n sectors starting at the sector containing address
// and ending with the sector containing address+num
void sFlashEraseSectors(unsigned long address,unsigned int num);

// erase the single sector containing address 
void sFlashEraseSector(unsigned long address);
#if 0
// write num bytes to flash starting at address
// affected sectors are erased first, 0 < num < 65536
void flash_rewrite(unsigned long address, uint8_t *pData, unsigned num);
#endif
// updates a flash page by reading a flash page, merging in new data and writing
// the page with the new data
void flash_page_update(unsigned long address, uint8_t *pData, unsigned num);

// write num bytes starting at address, assumes all byte locations are in erased state 
// 0 < num < 65536
void flash_write(unsigned long address, uint8_t *pData, unsigned int num);

// write num bytes starting at address, assumes all byte locations are in erased state 
// 0 < num <= 256
void flash_page_write(unsigned long address, uint8_t*pData, unsigned num);

// read num bytes starting at address, 0 < num < 65536
void flash_read(unsigned long address, uint8_t *pData, unsigned int num);

// setup flash in read mode starting at address
// flash remains in read mode with flash CE line low until flash CE line goes high
void flash_setup_read(unsigned long address);

// deselect flash chip
inline void CE_High(void);      /* note: compiler may not honor "inline" */

#else
/* Atmel/Adesto Data Flash Chip */ 
/* remove "dataflashSQI.c" and add "dataflashSQI.c" to project */


// DataFlash chip select port pin
#define SELECT_FLASH()   L_FLASH_CS = 0
#define DESELECT_FLASH() L_FLASH_CS = 1


void enable_sector_protection( void );
void disable_sector_protection( void );
void setup_sector_protection_reg( void );

// temporary
void flash_setup( void );
unsigned char write_SPI( unsigned char data );

/////////////////////////////////////////////////////////////////////////////////////
// Drivers for the Atmel AT45DB041D series serial data flash chips
// part #      page size pages sec size page   offset
// at45db041d  264 bytes  2048 256 pages 11 bit  9 bit xxxxpppp,pppppppo,oooooooo
// at45db081d  264 bytes  4096 256 pages 12 bit  9 bit xxxppppp,pppppppo,oooooooo
// at45db161d  528 bytes  4096 256 pages 12 bit 10 bit xxpppppp,ppppppoo,oooooooo
// at45db321d  528 bytes  8192 128 pages 13 bit 10 bit xppppppp,ppppppoo,oooooooo
// at45db642d 1056 bytes  8192 256 pages 13 bit 11 bit pppppppp,pppppooo,oooooooo
//
// note - should also work with at45db041b chips
// #define FLASH_CHIP AT45DB041B
////////////////////////////////////////////////////////////////////////////////////

void (void);
void flash_read(unsigned long address, unsigned char * pData, unsigned num);
#if defined NC30
#pragma parameter flash_read(R3R1, R0, A0)
#pragma parameter flash_rewrite(R3R1, R2R0, A0)
#endif
void flash_write(unsigned long address, const char * pData, unsigned num);
void flash_rewrite(unsigned long address, const char * pData, unsigned num);
//void flash_maintenance(void);
//void flash_page_erase(unsigned long address);
void flash_write_cached(unsigned long address, const char * pData, unsigned num);
void flash_write_flush(void);
void flash_test(void);
void flash_page_erase(unsigned long address);
uint8_t write_to_flash_buffer( const unsigned char* pData, uint16_t num_bytes, uint16_t offset, uint8_t buffer_num);
uint8_t write_buffer_to_flash(uint16_t page, uint8_t buffer_num );


/*
 MWL
 Sets up the flash for a sequential data read.
 Caution: this leaves the FLASH chip selected!
*/
extern void flash_setup_read(unsigned long address);


// note - physical flash page size (sFlashPageSize)
//        is a multiple of logical flash
//        page size (FLASH_PAGE_SIZE)
//
#define FLASH_PAGE_SIZE     264    // note - actual flash chip may now have
                                   // 264, 528 or 1056 bytes per page
#define FLASH_NUM_PAGES     4096
#define FLASH_SIZE  (FLASH_PAGE_SIZE * FLASH_NUM_PAGES)

//
//Global status register flags

#define T1_OVF  0x01
#define CLEARED 0x02

// buffer 1
#define BUFFER_1 0x00

// buffer 2
#define BUFFER_2 0x01


// defines for all opcodes

// buffer 1 write
#define BUFFER_1_WRITE 0x84

// buffer 2 write
#define BUFFER_2_WRITE 0x87

// buffer 1 read
//#define BUFFER_1_READ 0x54    // legacy
#define BUFFER_1_READ_LS 0xd1 // low speed mode
#define BUFFER_1_READ_HS 0xd4 // high speed mode

// buffer 2 read
//#define BUFFER_2_READ 0x56    // legacy
#define BUFFER_2_READ_LS 0xd3 // low speed mode d series chips
#define BUFFER_2_READ_HS 0xd6 // high speed mode d series chips

// continuous array read
//#define CONT_ARRAY_READ 0x68    // DO not use legacy comands .. 16mbit part will not operate correctly at data rates above 125Khz
#define CONT_ARRAY_READ_LS 0x03 // low speed mode
#define CONT_ARRAY_READ_HS 0x0b // high speed mode

// buffer 1 to main memory page program with built-in erase
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83

// buffer 2 to main memory page program with built-in erase
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86

// buffer 1 to main memory page program without built-in erase
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x88

// buffer 2 to main memory page program without built-in erase
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x89

// main memory page program through buffer 1
#define MM_PAGE_PROG_THROUGH_B1 0x82

// main memory page program through buffer 2
#define MM_PAGE_PROG_THROUGH_B2 0x85

// auto page rewrite through buffer 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58

// auto page rewrite through buffer 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59

// main memory page compare to buffer 1
#define MM_PAGE_TO_B1_COMP 0x60

// main memory page compare to buffer 2
#define MM_PAGE_TO_B2_COMP 0x61

// main memory page to buffer 1 transfer
#define MM_PAGE_TO_B1_XFER 0x53

// main memory page to buffer 2 transfer
#define MM_PAGE_TO_B2_XFER 0x55

// erase a page
#define PAGE_ERASE 	0x81
// main memory page to buffer 2 transfer
#define BLOCK_ERASE 0x50

// DataFlash status register for reading density, compare status,
// and ready/busy status
#define STATUS_REGISTER 0xD7
#define STATUS_BUSY(status) (((status) & 0x80) == 0)

// main memory page read
//#define MAIN_MEMORY_PAGE_READ 0x52    // legacy
//#define MAIN_MEMORY_PAGE_READ 0xd2    // d series chips

// erase a page
//#define PAGE_ERASE  0x81
// erase a block
//#define BLOCK_ERASE 0x50

// erase a sector
//#define SECTOR_ERASE 0x7c

// erase entire chip (d series chips)
#define CHIP_ERASE1 0xc7
#define CHIP_ERASE2 0x94
#define CHIP_ERASE3 0x80
#define CHIP_ERASE4 0x9a


#define PAGE_UNKNOWN 0xffff
#endif

#endif 

