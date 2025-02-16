#ifdef ARM9
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

#include "main.h"
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dswnifi_lib.h"
#include "keypadTGDS.h"
#include "fileBrowse.h"	//generic template functions from TGDS: maintain 1 source, whose changes are globally accepted by all TGDS Projects.
#include "biosTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "dldi.h"
#include "global_settings.h"
#include "posixHandleTGDS.h"
#include "TGDSMemoryAllocator.h"
#include "consoleTGDS.h"
#include "soundTGDS.h"
#include "nds_cp15_misc.h"
#include "fatfslayerTGDS.h"
#include "utilsTGDS.h"
#include "ima_adpcm.h"
#include "tamalib.h"
#include "mem_edit.h"
#include "powerTGDS.h"
#include "loader.h"
#include "TGDS_threads.h"

// Includes
#include "WoopsiTemplate.h"

__attribute__((section(".dtcm")))
struct task_Context * internalTGDSThreads = NULL;

//TGDS Soundstreaming API
int internalCodecType = SRC_NONE; //Returns current sound stream format: WAV, ADPCM or NONE
struct fd * _FileHandleVideo = NULL; 
struct fd * _FileHandleAudio = NULL;

bool stopSoundStreamUser(){
	return stopSoundStream(_FileHandleVideo, _FileHandleAudio, &internalCodecType);
}

void closeSoundUser(){
	//Stubbed. Gets called when closing an audiostream of a custom audio decoder
}

//because virtual pet needs drums to start properly
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void playTamaIntro(){
	playSoundStream("0:/drumSample.ima", _FileHandleVideo, _FileHandleAudio, TGDS_ARM7_AUDIOBUFFER_STREAM);
}

static inline void menuShow(){
	clrscr();
	printf("     ");
	printf("     ");
	printf("toolchaingenericds-foobilliard: ");
	printf("(Select): This menu. ");
	printf("(Start): FileBrowser : (A) Play WAV/IMA-ADPCM (Intel) strm ");
	printf("(D-PAD:UP/DOWN): Volume + / - ");
	printf("(D-PAD:LEFT): GDB Debugging. >%d", TGDSPrintfColor_Green);
	printf("(D-PAD:RIGHT): Demo Sound. >%d", TGDSPrintfColor_Yellow);
	printf("(B): Stop WAV/IMA-ADPCM file. ");
	printf("Current Volume: %d", (int)getVolume());
	if(internalCodecType == SRC_WAVADPCM){
		printf("ADPCM Play: >%d", TGDSPrintfColor_Red);
	}
	else if(internalCodecType == SRC_WAV){	
		printf("WAVPCM Play: >%d", TGDSPrintfColor_Green);
	}
	else{
		printf("Player Inactive");
	}
	printf("Available heap memory: %d >%d", getMaxRam(), TGDSPrintfColor_Cyan);
}

//ToolchainGenericDS-LinkedModule User implementation: Called if TGDS-LinkedModule fails to reload ARM9.bin from DLDI.
int TGDSProjectReturnFromLinkedModule()  {
	return -1;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int main(int argc, char **argv)   {
	/*			TGDS 1.6 Standard ARM9 Init code start (custom VRAM + Woopsi SDK)	*/
	bool isTGDSCustomConsole = false;	//set default console or custom console: custom console
	GUI_init(isTGDSCustomConsole);
	GUI_clear();

	bool isCustomTGDSMalloc = true;
	setTGDSMemoryAllocator(getProjectSpecificMemoryAllocatorSetup(isCustomTGDSMalloc));
	
	int ret=FS_init();
	if (ret == 0)
	{
		
	}
	else{
		
	}
	
	asm("mcr	p15, 0, r0, c7, c10, 4");
	flush_icache_all();
	flush_dcache_all();
	internalTGDSThreads = getTGDSThreadSystem();
	/*			TGDS 1.6 Standard ARM9 Init code end (custom VRAM + Woopsi SDK)	*/
	
	REG_IME = 0;
	setupDisabledExceptionHandler();
	set0xFFFF0000FastMPUSettings();
	//TGDS-Projects -> legacy NTR TSC compatibility
	if(__dsimode == true){
		TWLSetTouchscreenTWLMode();
	}
	REG_IME = 1;
	
	int taskATimeMS = 1; //Task execution requires at least 1ms: Timeout led backlight.
	if(registerThread(internalTGDSThreads, (TaskFn)&taskA, (u32*)NULL, taskATimeMS, (TaskFn)&onThreadOverflowUserCode, tUnitsMilliseconds) != THREAD_OVERFLOW){
        
    }

	powerOFF3DEngine(); //Power off ARM9 3D Engine to save power
	setBacklight(POWMAN_BACKLIGHT_BOTTOM_BIT);
	bottomScreenIsLit = true;

	// Create Woopsi UI
	WoopsiTemplate WoopsiTemplateApp;
	WoopsiTemplateProc = &WoopsiTemplateApp;
	WoopsiTemplateApp.main(argc, argv);
	
	//tama process end
	if (memory_editor_enable) {
		mem_edit_reset_terminal();
	}

	tamalib_release();

	#ifdef WIN32
	sdl_release();
	#endif
	
	TGDSARM9Free(g_program);
	tamalib_free_bp(&g_breakpoints);
	
	while(1) {
		bool waitForVblank = false;
		int threadsRan = runThreads(internalTGDSThreads, waitForVblank);
	}
	
	return 0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void taskA(u32 * args){
	handleTurnOnTurnOffScreenTimeout();
}

bool iconCallbackWakeup = false;	//Allow the set icon callback to timeout the screen backlight
bool bottomScreenIsLit = false;
int millisecondsElapsed = 0;	
static bool toggleScreen = false;
int iconCallbackWakeupAcknowledgeTimeout = 0;
bool triggerSpecialEffect = false;

//called 50 times per second
void handleTurnOnTurnOffScreenTimeout(){

	//special effects start	
	if( (iconCallbackWakeup == true) && (iconCallbackWakeupAcknowledgeTimeout < 16) ){
		bottomScreenIsLit = true; //special effect is treated as screen lit
		if (  (iconCallbackWakeupAcknowledgeTimeout % 4 ) ){
			if(toggleScreen == false){
				if(GUI.GBAMacroMode == true){
					setBacklight(POWMAN_BACKLIGHT_BOTTOM_BIT);
				}
				else{
					setBacklight(POWMAN_BACKLIGHT_TOP_BIT|POWMAN_BACKLIGHT_BOTTOM_BIT);
				}
			}
			else{
				setBacklight(0);	
			}
			toggleScreen = !toggleScreen;
		}
		iconCallbackWakeupAcknowledgeTimeout++;
	}
	//special effects end
	
	else{
		millisecondsElapsed ++;
		if (  millisecondsElapsed >= 500 ){
			setBacklight(0);
			millisecondsElapsed = 0;

			if(triggerSpecialEffect == true){
				iconCallbackWakeupAcknowledgeTimeout = 0;
				triggerSpecialEffect = false;
			}
			
		}
		//turn on bottom screen if input event
		if(bottomScreenIsLit == true){
			if(GUI.GBAMacroMode == true){
				setBacklight(POWMAN_BACKLIGHT_BOTTOM_BIT);
			}
			else{
				setBacklight(POWMAN_BACKLIGHT_TOP_BIT|POWMAN_BACKLIGHT_BOTTOM_BIT);
			}
			bottomScreenIsLit = false;
			millisecondsElapsed = 0;
		}	
		else{
			iconCallbackWakeup = false; //Re-enable the action event (8th icon) trigger, only when screen is turned off.
		}
	}

}

//////////////////////////////////////////////////////// Threading User code start : TGDS Project specific ////////////////////////////////////////////////////////
//User callback when Task Overflows. Intended for debugging purposes only, as normal user code tasks won't overflow if a task is implemented properly.
//	u32 * args = This Task context
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void onThreadOverflowUserCode(u32 * args){
	struct task_def * thisTask = (struct task_def *)args;
	struct task_Context * parentTaskCtx = thisTask->parentTaskCtx;	//get parent Task Context node 
	char threadStatus[64];
	switch(thisTask->taskStatus){
		case(INVAL_THREAD):{
			strcpy(threadStatus, "INVAL_THREAD");
		}break;
		
		case(THREAD_OVERFLOW):{
			strcpy(threadStatus, "THREAD_OVERFLOW");
		}break;
		
		case(THREAD_EXECUTE_OK_WAIT_FOR_SLEEP):{
			strcpy(threadStatus, "THREAD_EXECUTE_OK_WAIT_FOR_SLEEP");
		}break;
		
		case(THREAD_EXECUTE_OK_WAKEUP_FROM_SLEEP_GO_IDLE):{
			strcpy(threadStatus, "THREAD_EXECUTE_OK_WAKEUP_FROM_SLEEP_GO_IDLE");
		}break;
	}
	
	char debOut2[256];
	char timerUnitsMeasurement[32];
	if( thisTask->taskStatus == THREAD_OVERFLOW){
		if(thisTask->timerFormat == tUnitsMilliseconds){
			strcpy(timerUnitsMeasurement, "ms");
		}
		else if(thisTask->timerFormat == tUnitsMicroseconds){
			strcpy(timerUnitsMeasurement, "us");
		} 
		else{
			strcpy(timerUnitsMeasurement, "-");
		}
		sprintf(debOut2, "[%s]. Thread requires at least (%d) %s. ", threadStatus, thisTask->remainingThreadTime, timerUnitsMeasurement);
	}
	else{
		sprintf(debOut2, "[%s]. ", threadStatus);
	}
	
	int TGDSDebuggerStage = 10;
	u8 fwNo = *(u8*)(0x027FF000 + 0x5D);
	handleDSInitOutputMessage((char*)debOut2);
	handleDSInitError(TGDSDebuggerStage, (u32)fwNo);
	
	while(1==1){
		HaltUntilIRQ();
	}
}
//////////////////////////////////////////////////////////////////////// Threading User code end /////////////////////////////////////////////////////////////////////////////

#endif