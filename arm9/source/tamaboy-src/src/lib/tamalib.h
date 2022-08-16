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
#ifndef _TAMALIB_H_
#define _TAMALIB_H_

#ifdef ARM9
#include "typedefsTGDS.h"
#include "videoTGDS.h"
#endif
#include "cpu.h"
#include "hw.h"
#include "hal.h"

#define tamalib_set_button(btn, state)			hw_set_button(btn, state)
#define tamalib_set_speed(speed)			cpu_set_speed(speed)

#define tamalib_get_state()				cpu_get_state()
#define tamalib_refresh_hw()				cpu_refresh_hw()

#define tamalib_reset()					cpu_reset()

#define tamalib_add_bp(list, addr)			cpu_add_bp(list, addr)
#define tamalib_free_bp(list)				cpu_free_bp(list)

typedef enum {
	SPEED_UNLIMITED = 0,
	SPEED_1X = 1,
	SPEED_10X = 10,
} emulation_speed_t;

typedef enum {
	EXEC_MODE_PAUSE,
	EXEC_MODE_RUN,
	EXEC_MODE_STEP,
	EXEC_MODE_NEXT,
	EXEC_MODE_TO_CALL,
	EXEC_MODE_TO_RET,
} exec_mode_t;

#ifdef ARM9
//NDS Specific; scaling scheme: modified from StellaDS 
#define SCREENWIDTH  (256)
#define SCREENHEIGHT (192)
#define COLOR(r,g,b)  ((r) | (g)<<5 | (b)<<10)
#define OFFSET(r,c,w) ((r)*(w)+(c))
#define VRAM_BUFFER            ((u16*)0x06000000)
#define PIXEL_ENABLE (1<<15)
#define TAMA_DRAW_BUFFER ((u16*)0x06880000)

#define PixInv (u16)(RGB15(24,24,24))
#define PixNorm (u16)(RGB15(0,0,0))
#define IconPixSelected (u16)(RGB15(31,31,0))
#define screenScale (u16)(120)
#define xOffset (u16)(0)
#define yOffset (u16)(0)
#define stretch_x (u16)((( (LCD_WIDTH << 2) / 256) << 8) | ( (LCD_WIDTH << 2) % 256))
#define regBG3PD (u16)(((100 / screenScale)  << 8) | (100 % screenScale))
#define regBG3X (u16)((xOffset)<<8)
#define regBG3Y (u16)((yOffset)<<8)
#define PIXEL_SIZE					3
#define ICON_SIZE					8
#define ICON_STRIDE_X					24
#define ICON_STRIDE_Y					56
#define ICON_OFFSET_X					24
#define ICON_OFFSET_Y					0
#define LCD_OFFET_X					16
#define LCD_OFFET_Y					8

#define LCD_WIDTH			32
#define LCD_HEIGHT			16
#define ICON_NUM			8

#define ROM_PATH			"0:/rom.bin"
#define SAVE_PATH           "0:/ndstamaboy.sav"
#endif

//general defs
#define APP_NAME			"TamaTool"
#define APP_VERSION			"0.1" // Major, minor
#define COPYRIGHT_DATE			"2021"
#define AUTHOR_NAME			"Jean-Christophe Rona"
#define ROM_NOT_FOUND_TITLE		"Tamagotchi ROM not found"
#define ROM_NOT_FOUND_MSG		"You need to place a Tamagotchi P1 ROM called rom.bin inside TamaTool's folder/package first !"

#define REF_BACKGROUND_SIZE		345
#define REF_BACKGROUND_OFFSET_X		148
#define REF_BACKGROUND_OFFSET_Y		284

#define REF_SHELL_WIDTH			634
#define REF_SHELL_HEIGHT		816

#define REF_LCD_SIZE			321
#define REF_LCD_OFFSET_X		12
#define REF_LCD_OFFSET_Y		93

#define ICON_SRC_SIZE			64

#define REF_ICON_DEST_SIZE		64
#define REF_ICON_OFFSET_X		35
#define REF_ICON_OFFSET_Y		25
#define REF_ICON_STRIDE_X		71
#define REF_ICON_STRIDE_Y		242

#define REF_BUTTONS_X			182
#define REF_BUTTONS_Y			716
#define REF_BUTTONS_WIDTH		278
#define REF_BUTTONS_HEIGHT		88

#define REF_PIXEL_PADDING		1

#define DEFAULT_PIXEL_STRIDE		10

#define PIXEL_STRIDE_MIN		1
#define PIXEL_STRIDE_MAX		30

#define DEFAULT_LCD_ALPHA_ON		255
#define DEFAULT_LCD_ALPHA_OFF		20
#define RES_PATH			"../../res"
#define BACKGROUND_PATH			RES_PATH"/background.png"
#define SHELL_PATH			RES_PATH"/shell.png"
#define ICONS_PATH			RES_PATH"/icons.png"

#define AUDIO_FREQUENCY			48000
#define AUDIO_SAMPLES			480 // 10 ms @ 48000 Hz
#define AUDIO_VOLUME			0.2f

#define DEFAULT_FRAMERATE				30 // fps
#define MEM_FRAMERATE			DEFAULT_FRAMERATE // fps

/* Uncomment this line to be as close as possible
 * to a cycle-accurate emulation. The downside is that
 * the CPU load will be close to 100%.
 */
//#define NO_SLEEP

#endif /* _TAMALIB_H_ */

#ifdef __cplusplus
extern "C" {
#endif

extern breakpoint_t *g_breakpoints;
extern uint32_t g_program_size;
extern bool_t memory_editor_enable;

extern void tamalib_release(void);
extern bool_t tamalib_init(const u12_t *program, breakpoint_t *breakpoints, u32_t freq);

extern void tamalib_set_framerate(u8_t framerate);
extern u8_t tamalib_get_framerate(void);

extern void tamalib_register_hal(hal_t *hal);

extern void tamalib_set_exec_mode(exec_mode_t mode);

/* NOTE: Only one of these two functions must be used in the main application
 * (tamalib_step() should be used only if tamalib_mainloop() does not fit the
 * main application execution flow).
 */
extern void tamalib_step(void);
#ifdef WIN32
extern void tamalib_mainloop(void);
extern int main(int argc, char **argv);
#endif

extern hal_t hal;
extern void * hal_malloc(u32_t size);
extern void hal_free(void *ptr);
extern void hal_halt(void);
extern bool_t hal_is_log_enabled(log_level_t level);
extern void hal_log(log_level_t level, char *buff, ...);
extern void hal_sleep_until(timestamp_t ts);
extern timestamp_t hal_get_timestamp(void);
extern void hal_update_screen(void);
extern void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val);
extern void hal_set_lcd_icon(u8_t icon, bool_t val);
extern void hal_set_frequency(u32_t freq);
extern void hal_play_frequency(bool_t en);
extern int hal_handler(void);
extern u32_t ts_freq;
extern void compute_layout(void);
extern u8_t log_levels;
extern timestamp_t screen_ts;
extern u8_t g_framerate;
extern exec_mode_t exec_mode;
extern u32_t step_depth;

#ifdef ARM9
extern int tamaprocess(int argc, char **argv);
extern bool reEnableVblank;
extern uint8 SetPix(uint8 X, uint8 Y, bool isSelectedIcon);
extern uint8 ClrPix(uint8 X, uint8 Y);
extern void setPixel(int row, int col, u16 color);
extern void tama_process();
extern u32 bankedButtons;
extern u32* cycle_count;
extern u32 next_frame_count;
extern int next_frame_overflow;
extern void ClrBuf();
extern void draw_square(uint8 x, uint8 y, uint8 w, uint8 h, uint8 v);
extern void draw_icon(uint8 x, uint8 y, uint8 num, uint8 v);
extern const bool_t icons[ICON_NUM][ICON_SIZE][ICON_SIZE];
extern void do_nothing(void);
#endif


#ifdef __cplusplus
}
#endif