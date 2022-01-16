/*
 * TamaTool - A cross-platform Tamagotchi P1 explorer
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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "tamalib.h"
#include "state.h"
#include "fatfslayerTGDS.h"
#include "debugNocash.h"
#include "WoopsiTemplate.h"
#include "ipcfifoTGDS.h"

#define STATE_FILE_MAGIC				"TLST"
#define STATE_FILE_VERSION				2

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
static uint32_t find_next_slot(void)
{
	char path[256];
	uint32_t i = 0;

	for (i = 0;; i++) {
		sprintf(path, STATE_TEMPLATE, i);
		if (FileExists(path) == FT_FILE) {
			nocashMessage("find_next_slot(): found!");
			break;
		}
	}

	return i;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void state_find_next_name(char *path)
{
	sprintf(path, STATE_TEMPLATE, find_next_slot());
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void state_find_last_name(char *path)
{
	uint32_t num = find_next_slot();

	if (num > 0) {
		sprintf(path, STATE_TEMPLATE, num - 1);
	} else {
		path[0] = '\0';
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void state_save(char *path)
{
	state_t *state;
	int bufOffset = 0;
	u8 * buf = (u8*)TGDSARM9Malloc(8192);
	
	uint32_t num = 0;
	uint32_t i;
	char dbgBuf[128];
	
	state = tamalib_get_state();
	
	/* First the magic, then the version, and finally the fields of
	 * the state_t struct written as u8, u16 little-endian or u32
	 * little-endian following the struct order
	 */
	buf[bufOffset] = (uint8_t) STATE_FILE_MAGIC[0];
	bufOffset++;
	buf[bufOffset] = (uint8_t) STATE_FILE_MAGIC[1];
	bufOffset++;
	buf[bufOffset] = (uint8_t) STATE_FILE_MAGIC[2];
	bufOffset++;
	buf[bufOffset] = (uint8_t) STATE_FILE_MAGIC[3];
	bufOffset++;
	
	buf[bufOffset] = STATE_FILE_VERSION & 0xFF;
	bufOffset++;

	buf[bufOffset] = *(state->pc) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->pc) >> 8) & 0x1F;
	bufOffset++;

	buf[bufOffset] = *(state->x) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->x) >> 8) & 0xF;
	bufOffset++;
	
	buf[bufOffset] = *(state->y) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->y) >> 8) & 0xF;
	bufOffset++;
	
	buf[bufOffset] = *(state->a) & 0xF;
	bufOffset++;
	
	buf[bufOffset] = *(state->b) & 0xF;
	bufOffset++;
	
	buf[bufOffset] = *(state->np) & 0x1F;
	bufOffset++;

	buf[bufOffset] = *(state->sp) & 0xFF;
	bufOffset++;

	buf[bufOffset] = *(state->flags) & 0xF;
	bufOffset++;
	
	buf[bufOffset] = *(state->tick_counter) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = (*(state->tick_counter) >> 8) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = (*(state->tick_counter) >> 16) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->tick_counter) >> 24) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = *(state->clk_timer_timestamp) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->clk_timer_timestamp) >> 8) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->clk_timer_timestamp) >> 16) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->clk_timer_timestamp) >> 24) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = *(state->prog_timer_timestamp) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->prog_timer_timestamp) >> 8) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->prog_timer_timestamp) >> 16) & 0xFF;
	bufOffset++;
	buf[bufOffset] = (*(state->prog_timer_timestamp) >> 24) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = *(state->prog_timer_enabled) & 0x1;
	bufOffset++;
	
	buf[bufOffset] = *(state->prog_timer_data) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = *(state->prog_timer_rld) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = *(state->call_depth) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = (*(state->call_depth) >> 8) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = (*(state->call_depth) >> 16) & 0xFF;
	bufOffset++;
	
	buf[bufOffset] = (*(state->call_depth) >> 24) & 0xFF;
	bufOffset++;
	
	for (i = 0; i < INT_SLOT_NUM; i++) {
		buf[bufOffset] = state->interrupts[i].factor_flag_reg & 0xF;
		bufOffset++;
		
		buf[bufOffset] = state->interrupts[i].mask_reg & 0xF;
		bufOffset++;
		
		buf[bufOffset] = state->interrupts[i].triggered & 0x1;
		bufOffset++;
	}

	/* First 640 half bytes correspond to the RAM */
	for (i = 0; i < MEM_RAM_SIZE; i++) {
		buf[bufOffset] = state->memory[i + MEM_RAM_ADDR] & 0xF;
		bufOffset++;
	}

	/* I/Os are from 0xF00 to 0xF7F */
	for (i = 0; i < MEM_IO_SIZE; i++) {
		buf[bufOffset] = state->memory[i + MEM_IO_ADDR] & 0xF;
		bufOffset++;
	}
	
	FS_saveFile(path, (char *)buf, 8192, true); //turns out filesize had to be 8192 bytes to write properly!
	
	struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress;
	sprintf(dbgBuf, "Written to state file \"%s\"!\nClock:%d:%d:%d\n", path, TGDSIPC->tmInst.tm_hour, TGDSIPC->tmInst.tm_min, TGDSIPC->tmInst.tm_sec);
	printMessage((char *)dbgBuf);
	
	TGDSARM9Free(buf);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void state_load(char *path)
{
	FILE *f;
	state_t *state;
	int bufOffset = 0;
	u8 * buf = (u8*)TGDSARM9Malloc(8192);
	uint32_t i;
	char dbgBuf[128];
	
	state = tamalib_get_state();

	f = fopen(path, "r");
	if (f == NULL) {
		//fprintf(stderr, "FATAL: Cannot open state file \"%s\" !\n", path);
		sprintf(dbgBuf, "FATAL: Cannot open state file \"%s\" !\n", path);
		nocashMessage(dbgBuf);
		return;
	}
	
	fread(buf, 1, 8192, f);
	/* First the magic, then the version, and finally the fields of
	 * the state_t struct written as u8, u16 little-endian or u32
	 * little-endian following the struct order
	 */
	if (buf[0] != (uint8_t) STATE_FILE_MAGIC[0] || buf[1] != (uint8_t) STATE_FILE_MAGIC[1] ||
		buf[2] != (uint8_t) STATE_FILE_MAGIC[2] || buf[3] != (uint8_t) STATE_FILE_MAGIC[3]) {
		//fprintf(stderr, "FATAL: Wrong state file magic in \"%s\" !\n", path);
		sprintf(dbgBuf, "FATAL: Wrong state file magic in \"%s\" !\n", path);
		nocashMessage(dbgBuf);
		return;
	}
	
	bufOffset+=4;
	
	if (buf[bufOffset] != STATE_FILE_VERSION) {
		//fprintf(stderr, "FATAL: Unsupported version %u (expected %u) in state file \"%s\" !\n", buf[0], STATE_FILE_VERSION, path);
		/* TODO: Handle migration at a point */
		sprintf(dbgBuf, "FATAL: Unsupported version %u (expected %u) in state file \"%s\" !\n", buf[0], STATE_FILE_VERSION, path);
		nocashMessage(dbgBuf);
		return;
	}
	bufOffset++;
	
	*(state->pc) = buf[bufOffset+0] | ((buf[bufOffset+1] & 0x1F) << 8);
	bufOffset+=2;
	
	*(state->x) = buf[bufOffset + 0] | ((buf[bufOffset + 1] & 0xF) << 8);
	bufOffset+=2;
	
	*(state->y) = buf[bufOffset + 0] | ((buf[bufOffset + 1] & 0xF) << 8);
	bufOffset+=2;
	
	*(state->a) = buf[bufOffset + 0] & 0xF;
	bufOffset++;
	
	*(state->b) = buf[bufOffset + 0] & 0xF;
	bufOffset++;
	
	*(state->np) = buf[bufOffset + 0] & 0x1F;
	bufOffset++;
	
	*(state->sp) = buf[bufOffset + 0];
	bufOffset++;
	
	*(state->flags) = buf[bufOffset + 0] & 0xF;
	bufOffset++;
	
	*(state->tick_counter) = buf[bufOffset + 0] | (buf[bufOffset + 1] << 8) | (buf[bufOffset + 2] << 16) | (buf[bufOffset + 3] << 24);
	bufOffset+=4;
	
	*(state->clk_timer_timestamp) = buf[bufOffset + 0] | (buf[bufOffset + 1] << 8) | (buf[bufOffset + 2] << 16) | (buf[bufOffset + 3] << 24);
	bufOffset+=4;
	
	*(state->prog_timer_timestamp) = buf[bufOffset + 0] | (buf[bufOffset + 1] << 8) | (buf[bufOffset + 2] << 16) | (buf[bufOffset + 3] << 24);
	bufOffset+=4;
	
	*(state->prog_timer_enabled) = buf[bufOffset + 0] & 0x1;
	bufOffset++;
	
	*(state->prog_timer_data) = buf[bufOffset + 0];
	bufOffset++;
	
	*(state->prog_timer_rld) = buf[bufOffset + 0];
	bufOffset++;
	
	*(state->call_depth) = buf[bufOffset + 0] | (buf[bufOffset + 1] << 8) | (buf[bufOffset + 2] << 16) | (buf[bufOffset + 3] << 24);
	bufOffset+=4;
	
	for (i = 0; i < INT_SLOT_NUM; i++) {
		state->interrupts[i].factor_flag_reg = buf[bufOffset] & 0xF;
		bufOffset++;
		
		state->interrupts[i].mask_reg = buf[bufOffset] & 0xF;
		bufOffset++;
		
		state->interrupts[i].triggered = buf[bufOffset] & 0x1;
		bufOffset++;
	}

	/* First 640 half bytes correspond to the RAM */
	for (i = 0; i < MEM_RAM_SIZE; i++) {
		state->memory[i + MEM_RAM_ADDR] = buf[bufOffset] & 0xF;
		bufOffset++;
	}

	/* I/Os are from 0xF00 to 0xF7F */
	for (i = 0; i < MEM_IO_SIZE; i++) {
		state->memory[i + MEM_IO_ADDR] = buf[bufOffset] & 0xF;
		bufOffset++;
	}

	TGDSARM9Free(buf);
	fclose(f);
	tamalib_refresh_hw();
}