#include "typedefsTGDS.h"
#include "posixHandleTGDS.h"
#include "keypadTGDS.h"
#include "gui_console_connector.h"
#include "TGDSLogoLZSSCompressed.h"
#include "VideoGL.h"
#include "videoTGDS.h"
#include "tamalib/tamalib.h"
#include "tamalib/cpu.h"
#include "hal.h"
#include "rom.h"
#include "dsregs.h"
#include "tama_process.h"

u32* cycle_count=NULL;
u32 next_frame_count=0;
int next_frame_overflow=0;

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void setup_vram(void) {
    int i;
	
    
    /* set up sound */
	//todo: sound
	/*
    REG_SOUNDCNT_X = 0x80;
    REG_SOUNDCNT_L=0x0077;
    REG_SOUNDCNT_H = 2;
    REG_SOUND1CNT_L=0x0;
    REG_SOUND1CNT_H= 2<<6 | 15<<12;
    REG_SOUND1CNT_X= 1<<15;
	*/

    /* set up lcd icon variables */
    show_overlay = 0;
    lcd_icon_state = 0;

    
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void tama_process(){
	// wait for vblank 
	IRQWait(0, IRQ_VBLANK);	
	
	// process buttons 
	int i = ~REG_KEYINPUT;
	hw_set_button(BTN_LEFT, (i&(KEY_SELECT|KEY_LEFT))?1:0);
	hw_set_button(BTN_MIDDLE, (i&(KEY_A|KEY_UP|KEY_DOWN))?1:0);
	hw_set_button(BTN_RIGHT, (i&(KEY_B|KEY_RIGHT))?1:0);
	if (i & KEY_START)
		show_overlay = 1;
	// set number of cycles to next frame 
	next_frame_count += 546;
	next_frame_overflow += 0xa;
	if (next_frame_overflow >= 0x10) {
		next_frame_overflow &= 0xf;
		next_frame_count++;
	}
	// do some processor stuff 
	while (*cycle_count < next_frame_count) {
		tamalib_step();
		tamalib_step();
		tamalib_step();
		tamalib_step();
	}
	
	hal_update_screen();
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void setPixel(int row, int col, u16 color) {
	VRAM_BUFFER[OFFSET(row, col, SCREENWIDTH)] = color | PIXEL_ENABLE;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void draw_icon(uint8_t x, uint8_t y, uint8_t num, uint8_t v)
{
	uint8_t i, j;

	if (v) {
		for (j = 0; j < ICON_SIZE; j++) {
			for (i = 0; i < ICON_SIZE; i++) {
				if(icons[num][j][i]) {
					SetPix(x + i, y + j);
				}
			}
		}
	} else {
		for (j = 0; j < ICON_SIZE; j++) {
			for (i = 0; i < ICON_SIZE; i++) {
				if(icons[num][j][i]) {
					ClrPix(x + i, y + j);
				}
			}
		}
	}
}


uint8_t SetPix(uint8_t X, uint8_t Y){
	setPixel((int)Y, (int)X, PixNorm); //same as uint8_t WritePix(int16_t X, int16_t Y, PixT V)
	return 0;	
}

uint8_t ClrPix(uint8_t X, uint8_t Y){
	setPixel((int)Y, (int)X, PixInv); //same as uint8_t WritePix(int16_t X, int16_t Y, PixT V)
	return 0;
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void draw_square(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t v)
{
	uint8_t i, j;

	if (v) {
		for (j = 0; j < h; j++) {
			for (i = 0; i < w; i++) {
				SetPix(x + i, y + j);
			}
		}
	} else {
		for (j = 0; j < h; j++) {
			for (i = 0; i < w; i++) {
				ClrPix(x + i, y + j);
			}
		}
	}
}