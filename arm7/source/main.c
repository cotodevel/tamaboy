/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA
*/

#include <string.h>
#include "main.h"
#include "InterruptsARMCores_h.h"
#include "interrupts.h"

#include "ipcfifoTGDSUser.h"
#include "wifi_arm7.h"
#include "usrsettingsTGDS.h"
#include "timerTGDS.h"
#include "biosTGDS.h"

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	/*			TGDS 1.6 Standard ARM7 Init code start	*/
	
	//wait for VRAM D to be assigned from ARM9->ARM7 (ARM7 has load/store on byte/half/words on VRAM)
	while (!(*((vuint8*)0x04000240) & 0x2));
	
	installWifiFIFO();		
	
	/*			TGDS 1.6 Standard ARM7 Init code end	*/
	
    while (1) {
		handleARM7SVC();	/* Do not remove, handles TGDS services */
		HaltUntilIRQ();
	}
	return 0;
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
        hourDec = 0x0003; 
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