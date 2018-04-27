#ifndef XPRINTF_H
#define XPRINTF_H

#include "queue.h"
//#include "voice.h"

uint8_t Itoa(uint8_t *dest,uint16_t nbr,uint8_t precision);
uint8_t copyN(uint8_t *src, uint8_t *dest, uint8_t n);
uint8_t copyNConst(const char *src, uint8_t *dest, uint8_t n);
uint8_t copyNConstToken(int token, uint8_t *dest, uint8_t n);
uint16_t xprintf(uint8_t dest[],uint8_t max, struct PHRASE_DATA *pData, uint8_t fontSize, uint8_t fontStyle, uint16_t maxPixelWidth);
void memFill(uint8_t dest[], uint8_t fill, uint16_t count);
uint8_t uint8toA(uint8_t dest[],uint8_t nbr);
uint16_t uint16toA(uint8_t dest[],uint16_t nbr);
uint8_t get_uistrings_text(uint8_t dest[],uint8_t max,uint16_t phraseN);

#endif /* XPRINTF_H */
