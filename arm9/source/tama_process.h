#ifndef __tama_process_h__
#define __tama_process_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "fatfslayerTGDS.h"
#include "utilsTGDS.h"

#define SCREENWIDTH  (256)
#define SCREENHEIGHT (192)
#define COLOR(r,g,b)  ((r) | (g)<<5 | (b)<<10)
#define OFFSET(r,c,w) ((r)*(w)+(c))
#define VRAM_BUFFER            ((u16*)0x06000000)
#define PIXEL_ENABLE (1<<15)

#define PixInv (u16)(RGB15(31,31,31))
#define PixNorm (u16)(RGB15(0,0,0))

//scaling scheme: modified from StellaDS 
#define screenScale (u16)(120)
#define xOffset (u16)(0)
#define yOffset (u16)(0)
#define stretch_x (u16)((( (LCD_WIDTH << 2) / 256) << 8) | ( (LCD_WIDTH << 2) % 256))
#define regBG3PD (u16)(((100 / screenScale)  << 8) | (100 % screenScale))
#define regBG3X (u16)((xOffset)<<8)
#define regBG3Y (u16)((yOffset)<<8)

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void tama_process();
extern u32* cycle_count;
extern u32 next_frame_count;
extern int next_frame_overflow;
extern void setPixel(int row, int col, u16 color);
extern void draw_icon(uint8_t x, uint8_t y, uint8_t num, uint8_t v);
extern void draw_square(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t v);
extern uint8_t SetPix(uint8_t X, uint8_t Y);
extern uint8_t ClrPix(uint8_t X, uint8_t Y);

#ifdef __cplusplus
}
#endif