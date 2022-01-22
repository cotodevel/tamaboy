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
#include "soundTGDS.h"
#include "main.h"

u32 bankedButtons = 0;
u32* cycle_count=NULL;
u32 next_frame_count=0;
int next_frame_overflow=0;

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void tama_process(){
	
	// process buttons 
	int i = (~REG_KEYINPUT);
	if(bankedButtons > 0){ //trap keys this time
		i|= bankedButtons;
		bankedButtons = 0;
	}
	
	
	hw_set_button(BTN_LEFT, (i&(KEY_SELECT|KEY_LEFT))?1:0);
	hw_set_button(BTN_MIDDLE, (i&(KEY_A|KEY_UP|KEY_DOWN))?1:0);
	hw_set_button(BTN_RIGHT, (i&(KEY_B|KEY_RIGHT))?1:0);
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
void draw_icon(uint8 x, uint8 y, uint8 num, uint8 v)
{
	uint8 i, j;

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

#ifdef ARM9
__attribute__((section(".dtcm")))
#endif
bool reEnableVblank = false;

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
uint8 SetPix(uint8 X, uint8 Y){
	if(reEnableVblank == false){
		enableWaitForVblankC();
		playTamaIntro();
		reEnableVblank = true;
	}
	setPixel((int)Y, (int)X, PixNorm); //same as uint8 WritePix(int16_t X, int16_t Y, PixT V)
	return 0;	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
uint8 ClrPix(uint8 X, uint8 Y){
	if(reEnableVblank == true){
		setPixel((int)Y, (int)X, PixInv); //same as uint8 WritePix(int16_t X, int16_t Y, PixT V)
	}
	return 0;
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void draw_square(uint8 x, uint8 y, uint8 w, uint8 h, uint8 v)
{
	uint8 i, j;

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