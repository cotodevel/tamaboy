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
	EXEC_MODE_PAUSE,
	EXEC_MODE_RUN,
	EXEC_MODE_STEP,
	EXEC_MODE_NEXT,
	EXEC_MODE_TO_CALL,
	EXEC_MODE_TO_RET,
} exec_mode_t;


#endif /* _TAMALIB_H_ */

#ifdef __cplusplus
extern "C" {
#endif

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
extern void tamalib_mainloop(void);

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

extern int main(int argc, char **argv);


#ifdef __cplusplus
}
#endif