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
#include <stdio.h>
#include <stdbool.h>

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
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void setPixel(int row, int col, u16 color) {
	TAMA_DRAW_BUFFER[OFFSET(row, col, SCREENWIDTH)] = color | PIXEL_ENABLE;
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
					SetPix(x + i, y + j, true);
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
uint8 SetPix(uint8 X, uint8 Y, bool isSelectedIcon){
	if(reEnableVblank == false){ //Startup code
		enableWaitForVblankC();
		playTamaIntro();
		reEnableVblank = true;
	}
	
	if(isSelectedIcon == true){
		setPixel((int)Y, (int)X, IconPixSelected);
	}
	else{
		setPixel((int)Y, (int)X, PixNorm);
	}
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
				SetPix(x + i, y + j, false);
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

/*****************************************************************************
 Convert an unsigned integer to a two-digit BCD number
 ****************************************************************************/
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void int2bcd(int i, char *bcd)
/* "i" is an unsigned integer, assumed to be less than 128 */

{
	int bit;								/* Loop variable - counts bits */
	int Carry;								/* Carry from BCD digit 0 to 1 */

	bcd[0] = 0;								/* Reset the BCD digits to 0 */
	bcd[1] = 0;

	for (bit = 0; bit < 8; bit++ ) {		/* Assume 8 bits */

		if ( bcd[0] < 5 ) {					/* Bottom BCD digit */
			bcd[0] *= 2;
			Carry = 0;
		}
		else {
			bcd[0] = (bcd[0] - 5) * 2;
			Carry = 1;
		}

		if ( i & 0x80 )						/* Add the top bit of i */
			bcd[0]++;						/* to the bottom BCD digit */
		i *= 2;								/* Shift i to the left */

		if ( bcd[1] < 5 ) {					/* Top BCD digit (no carry) */
			bcd[1] *= 2;
		}
		else {
			bcd[1] = (bcd[1] - 5) *2;
		}
		bcd[1] += Carry;

	}
}

//23 Jan 2022 
//Coto: added NDS RTC into Tamagotchi RTC
/*
Tama RTC clock bits

Hour mapping:
00[0xC] 00[0xD]: 12am
01[0xC] 00[0xD]: 1am
02[0xC] 00[0xD]: 2am
03[0xC] 00[0xD]: 3am
04[0xC] 00[0xD]: 4am
05[0xC] 00[0xD]: 5am
06[0xC] 00[0xD]: 6am
07[0xC] 00[0xD]: 7am
08[0xC] 00[0xD]: 8am
09[0xC] 00[0xD]: 9am
0A[0xC] 00[0xD]: 10am
0B[0xC] 00[0xD]: 11am
0C[0xC] 00[0xD]: 12pm
0D[0xC] 00[0xD]: 1pm
0E[0xC] 00[0xD]: 2pm
0F[0xC] 00[0xD]: 3pm
00[0xC] 01[0xD]: 4pm
01[0xC] 01[0xD]: 5pm
02[0xC] 01[0xD]: 6pm
03[0xC] 01[0xD]: 7pm
04[0xC] 01[0xD]: 8pm
05[0xC] 01[0xD]: 9pm
06[0xC] 01[0xD]: 10pm
07[0xC] 01[0xD]: 11pm

Min / Secs are integer -> BCD
*/

//NDS RTC: hh: 0-23 / mm: 0-59 / ss: 0-59
// unsigned char * tama_io_memorySource == &tama_io_memory[0x18]
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
bool convertHHMMSSDateTimeToTamaFormat(unsigned char hh, unsigned char mm, unsigned char ss, unsigned char * tama_io_memorySource){
    unsigned short hourDec = 0x0000; //12am default
    unsigned short minDec = 0x0000;
    unsigned short secDec = 0x0000;
	
	int2bcd((int)mm, (char*)&minDec);
	int2bcd((int)ss, (char*)&secDec);
	
    bool convertStatus = false;
    
    //12am
    if(hh == 0){
        hourDec = 0x0000;
        convertStatus = true;
    }
    //1am
    else if(hh == 1){
        hourDec = 0x0001;
        convertStatus = true;
    }
    //2am
    else if(hh == 2){
        hourDec = 0x0002; 
        convertStatus = true;
    }
    //3am
    else if(hh == 3){
        hourDec = 0x0002; 
        convertStatus = true;
    }
    //4am
    else if(hh == 4){
        hourDec = 0x0004; 
        convertStatus = true;
    }
    //5am
    else if(hh == 5){
        hourDec = 0x0005; 
        convertStatus = true;
    }
    //6am
    else if(hh == 6){
        hourDec = 0x0006; 
        convertStatus = true;
    }
    //7am
    else if(hh == 7){
        hourDec = 0x0007; 
        convertStatus = true;
    }
    //8am
    else if(hh == 8){
        hourDec = 0x0008; 
        convertStatus = true;
    }
    //9am
    else if(hh == 9){
        hourDec = 0x0009; 
        convertStatus = true;
    }
    //10am
    else if(hh == 10){
        hourDec = 0x000A; 
        convertStatus = true;
    }
    //11am
    else if(hh == 11){
        hourDec = 0x000B; 
        convertStatus = true;
    }
    //12pm
    else if(hh == 12){
        hourDec = 0x000C; 
        convertStatus = true;
    }
    //1pm
    else if(hh == 13){
        hourDec = 0x000D; 
        convertStatus = true;
    }
    //2pm
    else if(hh == 14){
        hourDec = 0x000E; 
        convertStatus = true;
    }
    //3pm
    else if(hh == 15){
        hourDec = 0x000F; 
        convertStatus = true;
    }
    //4pm
    else if(hh == 16){
        hourDec = 0x0100; 
        convertStatus = true;
    }
    //5pm
    else if(hh == 17){
        hourDec = 0x0101; 
        convertStatus = true;
    }
    //6pm
    else if(hh == 18){
        hourDec = 0x0102; 
        convertStatus = true;
    }
    //7pm
    else if(hh == 19){
        hourDec = 0x0103; 
        convertStatus = true;
    }
    //8pm
    else if(hh == 20){
        hourDec = 0x0104; 
        convertStatus = true;
    }
    //9pm
    else if(hh == 21){
        hourDec = 0x0105; 
        convertStatus = true;
    }
    //10pm
    else if(hh == 22){
        hourDec = 0x0106;
        convertStatus = true;
    }
    //11pm
    else if(hh == 23){
        hourDec = 0x0107; 
        convertStatus = true;
    }
    
    if(convertStatus == true){
        //tama_io_memorySource[0] == &tama_io_memory[16]
        tama_io_memorySource[0] = (secDec >> 0) & 0xF; //decimal: "s[s]" Decimal 0-9
        
        //tama_io_memorySource[1] == &tama_io_memory[17]
        tama_io_memorySource[1] = (secDec >> 8) & 0xF; //decimal: "[s]s" Decimal 0-9
        
        //tama_io_memorySource[2] == &tama_io_memory[18]
        tama_io_memorySource[2] = (minDec >> 0) & 0xF; //decimal: "m[m]" Decimal 0-9
        
        //tama_io_memorySource[3] == &tama_io_memory[19]
        tama_io_memorySource[3] = (minDec >> 8) & 0xF; //decimal: "[m]m" Decimal 0-5
        
        //tama_io_memorySource[4] == &tama_io_memory[20]
        tama_io_memorySource[4] =  (hourDec >> 0) & 0xF; //decimal: "h[h]" hex 0-F 
        
        //tama_io_memorySource[5] == &tama_io_memory[21]
        tama_io_memorySource[5] =  (hourDec >> 8) & 0xF; //decimal: "[h]h" hex 0-F
    }
    
    return convertStatus;
}