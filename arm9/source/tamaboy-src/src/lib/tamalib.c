#ifdef _MSC_VER
#include <windows.h>
#endif


/*
 * TamaLIB - A hardware agnostic Tamagotchi P1 emulation library
 *
 * Copyright (C) 2021 Jean-Christophe Rona <jc@rona.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "tamalib.h"
#include "hw.h"
#include "cpu.h"
#include "hal.h"
#ifdef ARM9
#include "debugNocash.h"
#endif

exec_mode_t exec_mode = EXEC_MODE_RUN;
u32_t step_depth = 0;
timestamp_t screen_ts = 0;
u32_t ts_freq = 0;
u8_t g_framerate = DEFAULT_FRAMERATE;

#ifdef ARM9
const bool_t icons[ICON_NUM][ICON_SIZE][ICON_SIZE] = {
	{
		{1, 0, 1, 0, 1, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 1, 1, 1, 1, 0, 1, 1},
		{0, 1, 1, 1, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 0, 1, 1},
		{0, 0, 1, 0, 0, 0, 0, 1},
		{0, 0, 1, 0, 0, 0, 0, 1},
	},
	{
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{1, 0, 1, 0, 0, 1, 0, 1},
		{0, 0, 1, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	},
	{
		{1, 1, 1, 0, 0, 1, 1, 1},
		{1, 0, 1, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 1, 1, 1, 1},
		{0, 0, 0, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
	},
	{
		{0, 0, 0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 1, 1, 0},
		{0, 0, 0, 1, 1, 1, 1, 1},
		{0, 0, 1, 1, 0, 1, 1, 1},
		{0, 1, 1, 0, 1, 1, 0, 1},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
	},
	{
		{0, 1, 1, 0, 0, 0, 0, 0},
		{1, 0, 0, 1, 0, 0, 1, 1},
		{1, 0, 0, 1, 1, 1, 0, 1},
		{0, 1, 0, 1, 0, 0, 1, 1},
		{0, 1, 0, 0, 1, 1, 0, 1},
		{0, 1, 0, 0, 0, 0, 0, 1},
		{0, 1, 1, 0, 0, 0, 1, 1},
		{0, 0, 1, 1, 1, 1, 1, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 0, 0, 1, 0, 0, 0, 1},
		{0, 1, 0, 0, 1, 0, 1, 0},
		{0, 0, 1, 1, 1, 1, 0, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 1, 0, 1},
		{0, 1, 1, 1, 0, 1, 0, 1},
		{1, 1, 1, 0, 0, 1, 0, 1},
		{1, 1, 0, 0, 1, 0, 1, 0},
		{1, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0, 0, 0},
		{1, 1, 0, 1, 1, 1, 1, 0},
		{1, 0, 1, 0, 0, 1, 0, 1},
		{0, 1, 1, 1, 1, 0, 1, 1},
		{0, 0, 0, 1, 0, 0, 0, 1},
		{0, 0, 0, 0, 1, 1, 1, 0},
	},
};
#endif

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool_t tamalib_init(const u12_t *program, breakpoint_t *breakpoints, u32_t freq){
	bool_t res = 0;

	hal.malloc = &hal_malloc;
	hal.free = &hal_free;
	hal.halt = &hal_halt;
	hal.is_log_enabled = &hal_is_log_enabled;
	hal.log = &hal_log;
	
	hal.get_timestamp = &hal_get_timestamp;
	#ifdef ARM9
	hal.sleep_until = do_nothing;
	hal.update_screen = do_nothing;
	hal.handler = do_nothing;
	#endif
	
	#ifdef WIN32
	hal.sleep_until = &hal_sleep_until;
	hal.update_screen = &hal_update_screen;
	hal.handler = &hal_handler;
	#endif
	hal.set_lcd_matrix = &hal_set_lcd_matrix;
	hal.set_lcd_icon = &hal_set_lcd_icon;
	hal.set_frequency = &hal_set_frequency;
	hal.play_frequency = &hal_play_frequency;

	res |= cpu_init(program, breakpoints, freq);
	res |= hw_init();

	ts_freq = freq;

	return res;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void tamalib_release(void)
{
	hw_release();
	cpu_release();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void tamalib_set_framerate(u8_t framerate)
{
	g_framerate = framerate;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u8_t tamalib_get_framerate(void)
{
	return g_framerate;
}

void tamalib_register_hal(hal_t *hal)
{
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void tamalib_set_exec_mode(exec_mode_t mode)
{
	exec_mode = mode;
	step_depth = cpu_get_depth();
	cpu_sync_ref_timestamp();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void tamalib_step(void)
{
	if (exec_mode == EXEC_MODE_PAUSE) {
		return;
	}

	if (cpu_step()) {
		exec_mode = EXEC_MODE_PAUSE;
		step_depth = cpu_get_depth();
	} else {
		switch (exec_mode) {
			case EXEC_MODE_PAUSE:
			case EXEC_MODE_RUN:
				break;

			case EXEC_MODE_STEP:
				exec_mode = EXEC_MODE_PAUSE;
				break;

			case EXEC_MODE_NEXT:
				if (cpu_get_depth() <= step_depth) {
					exec_mode = EXEC_MODE_PAUSE;
					step_depth = cpu_get_depth();
				}
				break;

			case EXEC_MODE_TO_CALL:
				if (cpu_get_depth() > step_depth) {
					exec_mode = EXEC_MODE_PAUSE;
					step_depth = cpu_get_depth();
				}
				break;

			case EXEC_MODE_TO_RET:
				if (cpu_get_depth() < step_depth) {
					exec_mode = EXEC_MODE_PAUSE;
					step_depth = cpu_get_depth();
				}
				break;
		}
	}
}

//VS2012 specific start
void tamalib_mainloop(void)
{
	timestamp_t ts;

	while (!hal.handler()) {
		tamalib_step();

		/* Update the screen @ g_framerate fps */
		ts = hal.get_timestamp();
		if (ts - screen_ts >= ts_freq/g_framerate) {
			screen_ts = ts;
			hal.update_screen();
		}
	}
}
//VS2012 specific end

//NDS specific start
#ifdef ARM9

static u16 thisFreq = 0;
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void hal_set_frequency(u32_t freq)
{
	short const freq_table[] = {(2076<<4),2068<<4,2060<<4,2830<<5,2044<<4,2028<<4,2011<<4,1996<<4};
    int n = freq_table[freq] + 9800*2;
	thisFreq = (u16)n;
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void hal_play_frequency(bool_t en)
{
	int channel = 9; //PSG
	//Bit24-26  Wave Duty    (0..7) ;HIGH=(N+1)*12.5%, LOW=(7-N)*12.5% (PSG only)
	u8 wavDuty = 6;
	u32 cnt   = SOUND_ONE_SHOT | SOUND_VOL(2) | SOUND_PAN(48) | (3 << 29) | (wavDuty << 24); //3=PSG/Noise)
	if (en){
		cnt = (SCHANNEL_ENABLE | cnt);
	}
	writeARM7SoundChannel(channel, cnt, thisFreq);
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
		nocashMessage("re-enabling VBLANK!");
		enableWaitForVblankC();
		playTamaIntro();
		//Sync to NDS RTC
		//struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = getsIPCSharedTGDSSpecific();
		//memcpy((unsigned char *)&tama_io_memory[16], (unsigned char *)&sIPCSharedTGDSSpecificInst->tama_clock_io_arm7[0], (int)6);	
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
void setPixel(int row, int col, u16 color) {
	TAMA_DRAW_BUFFER[OFFSET(row, col, SCREENWIDTH)] = color | PIXEL_ENABLE;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void ClrBuf(){
	/* Dot matrix */
	int j = 0;
	int i = 0;
	for (j = 0; j < LCD_HEIGHT; j++) {
		for (i = 0; i < LCD_WIDTH; i++) {
			draw_square(i * PIXEL_SIZE + LCD_OFFET_X, j * PIXEL_SIZE + LCD_OFFET_Y, PIXEL_SIZE, PIXEL_SIZE, 0);
		}
	}

	/* Icons */
	for (i = 0; i < ICON_NUM; i++) {
		draw_icon((i % 4) * ICON_STRIDE_X + ICON_OFFSET_X, (i / 4) * ICON_STRIDE_Y + ICON_OFFSET_Y, i, 0);
	}
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

__attribute__((section(".dtcm")))
u32 bankedButtons=0;

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Os")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
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
	
	u32 loops = 101; //(66mhz/32k)/20 =  1/20th~ of CPU threshold per frame
	// do some processor stuff 
	while (loops > 0) {
		tamalib_step();
		loops--;
	}	
}

#endif
//NDS specific end