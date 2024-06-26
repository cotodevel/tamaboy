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

#include "ipcfifoTGDSUser.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "InterruptsARMCores_h.h"
#include "interrupts.h"
#include "wifi_arm7.h"
#include "main.h"

#include "ipcfifoTGDS.h"
#include "spitscTGDS.h"
#include <stdbool.h>

#include "biosTGDS.h"
#include "spiTGDS.h"
#include "clockTGDS.h"

//TGDS-MB v3 bootloader
void bootfile(){
}

//User Handler Definitions

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void IpcSynchandlerUser(uint8 ipcByte){
	switch(ipcByte){
		default:{
			//ipcByte should be the byte you sent from external ARM Core through sendByteIPC(ipcByte);
		}
		break;
	}
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer0handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer1handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer2handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer3handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HblankUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void VcounterUser(){
	struct sIPCSharedTGDS * sIPCSharedTGDSInst = (struct sIPCSharedTGDS *)TGDSIPCStartAddress;
	//Convert NDS RTC -> RTC BCD format
	sIPCSharedTGDSInst->ndsRTCSeconds = nds_get_time7();
	unsigned char hh = ((unsigned char)sIPCSharedTGDSInst->tmInst.tm_hour);
    unsigned char mm = ((unsigned char)sIPCSharedTGDSInst->tmInst.tm_min);
    unsigned char ss = ((unsigned char)sIPCSharedTGDSInst->tmInst.tm_sec);
    struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = getsIPCSharedTGDSSpecific();
	convertHHMMSSDateTimeToTamaFormat(hh, mm, ss, (unsigned char *)&sIPCSharedTGDSSpecificInst->tama_clock_io_arm7[0]);
}

//Note: this event is hardware triggered from ARM7, on ARM9 a signal is raised through the FIFO hardware
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void screenLidHasOpenedhandlerUser(){
	TurnOnScreens();
}

//Note: this event is hardware triggered from ARM7, on ARM9 a signal is raised through the FIFO hardware
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void screenLidHasClosedhandlerUser(){
	TurnOffScreens();
}