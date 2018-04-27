#include "util.h"
#include <limits.h>

/*******************************************************************************
 * This module defines general-purpose utility functions.
 *
 * It is also a useful place to temporarily make functions for testing compiler
 * issues and "side" items.
 */

/**
 * Force a restart using the watchdog.
 */
void force_restart(void)
{
   for ( ;; )
   {
      /* Wait for watchdog timer to reset device. */
   }
}

/**
 * Calculate an exponential moving average to filter incoming data values.
 * This version uses an alpha of 1/32, scaled to the maximum integral values.
 * @param average  the current moving average
 * @param value    new value to present to filter
 * @return the filtered result
 * @see http://en.wikipedia.org/wiki/Exponential_moving_average#Exponential_moving_average
 */
#define EMA_ALPHA            (UINT_MAX / 32UL)      /* 65536 /   32 = 2048 */
#define EMA_ONE_MINUS_ALPHA  (UINT_MAX - EMA_ALPHA) /* 65536 - 2048 = 63488 */
uint16_t ema_filter(uint16_t average, uint16_t value)
{
   return (EMA_ONE_MINUS_ALPHA * average + EMA_ALPHA * value) / UINT_MAX;
}

#if 0

/*******************************************************************************
 * The following were comparisons of various ways of performing a bit shift over
 * an array.
 */

/**
 * Bit array shift left.
 * Receive a bit to be placed in the LS bit of the LS byte of an array of bits.
 * Shift remaining bits 1 to the "left".
 *
 * @param byte  pointer to the first (LS) byte of the bit array
 * @param size  the number of bytes in the array
 * @param carry boolean value for the incoming bit
 */
void shiftleft1(unsigned char *byte, uint8_t size, unsigned char carry)
{
   unsigned char *a, *b;
   /**
    * This method uses two pointers to wander from the MS byte of the array on
    * down. The 'a' pointer is the byte being modified, the 'b' pointer is the
    * next LS byte down in the array.
    */
   for ( a = byte + size - 1, b = a - 1; b > byte; --a, --b )
   {
      *a = (*a << 1) | (*b >> 7);
   }
   /** Special-case the last byte. */
   *b = (*b << 1) | carry;
}

/**
 * Bit array shift left.
 * Receive a bit to be placed in the LS bit of the LS byte of an array of bits.
 * Shift remaining bits 1 to the "left".
 *
 * @param byte  pointer to the first (LS) byte of the bit array
 * @param size  the number of bytes in the array
 * @param carry boolean value for the incoming bit
 */
void shiftleft2(unsigned char *byte, uint8_t size, unsigned char carry)
{
   /**
    * This was originally done as a for() loop, but to try to match C with its
    * generated assembly I've changed to a while() to separate statements.
    */
   unsigned char *end = byte + size;
   while ( byte < end )
   {
      unsigned char nextcarry = *byte >> 7;
      *byte = (*byte << 1) | carry;
      carry = nextcarry;
      ++byte;
   }
}

/**
 * Bit array shift left.
 * Receive a bit to be placed in the LS bit of the LS byte of an array of bits.
 * Shift remaining bits 1 to the "left".
 * Hard-coded to a 9-byte array.
 *
 * @param byte  pointer to the first (LS) byte of the bit array
 * @param carry boolean value for the incoming bit
 */
void shiftleft3(unsigned char *byte, unsigned char carry)
{
   unsigned char *end = byte + 9;
   while ( byte < end )
   {
      unsigned char nextcarry = *byte >> 7;
      *byte = (*byte << 1) | carry;
      carry = nextcarry;
      ++byte;
   }
}

/**
 * Bit array shift left.
 * Receive a bit to be placed in the LS bit of the LS byte of an array of bits.
 * Shift remaining bits 1 to the "left".
 * Hard-coded to a 9-byte array.
 *
 * @param byte  pointer to the first (LS) byte of the bit array
 * @param carry boolean value for the incoming bit
 */
void shiftleft4(unsigned char *byte, unsigned char carry)
{
   unsigned char *end = byte + 9;
   while ( byte < end )
   {
      unsigned char nextcarry = *byte >> 7;
      *byte = (*byte << 1) | carry;
      carry = nextcarry;
      ++byte;
   }
}

#endif
