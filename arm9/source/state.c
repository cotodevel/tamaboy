//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

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

#ifdef WIN32
#include "SDL.h"
#endif

#include "lib/tamalib.h"
#include "state.h"
#include "ff.h"
#include "debugNocash.h"
#include "fatfslayerTGDS.h"

#define STATE_FILE_MAGIC				"TLST"
#define STATE_FILE_VERSION				2

static uint32_t find_next_slot(void)
{
	char path[256];
	uint32_t i = 0;

	for (i = 0;; i++) {
		sprintf(path, STATE_TEMPLATE, i);
		if (!fopen(path, "r")) {
			break;
		}
	}

	return i;
}

void state_find_next_name(char *path)
{
	sprintf(path, STATE_TEMPLATE, find_next_slot());
}

void state_find_last_name(char *path)
{
	uint32_t num = find_next_slot();

	if (num > 0) {
		sprintf(path, STATE_TEMPLATE, num - 1);
	} else {
		path[0] = '\0';
	}
}

void state_save(char *path)
{
	FIL fPagingFD;
	state_t *state;
	uint8_t buf[4];
	uint32_t num = 0;
	uint32_t i;
	int writtenSize=0;
	char debugBuf[256];

	state = tamalib_get_state();

	int flags = charPosixToFlagPosix("w+");
	BYTE mode = posixToFatfsAttrib(flags);
	FRESULT result = f_open(&fPagingFD, (const TCHAR*)path, mode);

	if(result != FR_OK){
		sprintf(debugBuf, "FATAL: Cannot create state file \"%s\" !", path);
		nocashMessage(debugBuf);
		return;
	}

	/* First the magic, then the version, and finally the fields of
	 * the state_t struct written as u8, u16 little-endian or u32
	 * little-endian following the struct order
	 */
	f_lseek (
			&fPagingFD,   /* Pointer to the file object structure */
			(DWORD)0       /* File offset in unit of byte */
		);

	buf[0] = (uint8_t) STATE_FILE_MAGIC[0];
	buf[1] = (uint8_t) STATE_FILE_MAGIC[1];
	buf[2] = (uint8_t) STATE_FILE_MAGIC[2];
	buf[3] = (uint8_t) STATE_FILE_MAGIC[3];
	
	result = f_write(&fPagingFD, buf, (int)4, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = STATE_FILE_VERSION & 0xFF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->pc) & 0xFF;
	buf[1] = (*(state->pc) >> 8) & 0x1F;
	result = f_write(&fPagingFD, buf, (int)2, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->x) & 0xFF;
	buf[1] = (*(state->x) >> 8) & 0xF;
	result = f_write(&fPagingFD, buf, (int)2, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->y) & 0xFF;
	buf[1] = (*(state->y) >> 8) & 0xF;
	result = f_write(&fPagingFD, buf, (int)2, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->a) & 0xF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->b) & 0xF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->np) & 0x1F;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->sp) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->flags) & 0xF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->tick_counter) & 0xFF;
	buf[1] = (*(state->tick_counter) >> 8) & 0xFF;
	buf[2] = (*(state->tick_counter) >> 16) & 0xFF;
	buf[3] = (*(state->tick_counter) >> 24) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)4, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->clk_timer_timestamp) & 0xFF;
	buf[1] = (*(state->clk_timer_timestamp) >> 8) & 0xFF;
	buf[2] = (*(state->clk_timer_timestamp) >> 16) & 0xFF;
	buf[3] = (*(state->clk_timer_timestamp) >> 24) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)4, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->prog_timer_timestamp) & 0xFF;
	buf[1] = (*(state->prog_timer_timestamp) >> 8) & 0xFF;
	buf[2] = (*(state->prog_timer_timestamp) >> 16) & 0xFF;
	buf[3] = (*(state->prog_timer_timestamp) >> 24) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)4, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->prog_timer_enabled) & 0x1;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->prog_timer_data) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->prog_timer_rld) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
	num += writtenSize;

	buf[0] = *(state->call_depth) & 0xFF;
	buf[1] = (*(state->call_depth) >> 8) & 0xFF;
	buf[2] = (*(state->call_depth) >> 16) & 0xFF;
	buf[3] = (*(state->call_depth) >> 24) & 0xFF;
	result = f_write(&fPagingFD, buf, (int)4, (UINT*)&writtenSize);
	num += writtenSize;

	for (i = 0; i < INT_SLOT_NUM; i++) {
		buf[0] = state->interrupts[i].factor_flag_reg & 0xF;
		result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
		num += writtenSize;

		buf[0] = state->interrupts[i].mask_reg & 0xF;
		result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
		num += writtenSize;

		buf[0] = state->interrupts[i].triggered & 0x1;
		result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
		num += writtenSize;
	}

	/* First 640 half bytes correspond to the RAM */
	for (i = 0; i < MEM_RAM_SIZE; i++) {
		buf[0] = GET_RAM_MEMORY(state->memory, i + MEM_RAM_ADDR) & 0xF;
		result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
		num += writtenSize;
	}

	/* I/Os are from 0xF00 to 0xF7F */
	for (i = 0; i < MEM_IO_SIZE; i++) {
		buf[0] = GET_IO_MEMORY(state->memory, i + MEM_IO_ADDR) & 0xF;
		result = f_write(&fPagingFD, buf, (int)1, (UINT*)&writtenSize);
		num += writtenSize;
	}

	f_close(&fPagingFD);
}

void state_load(char *path)
{
	FIL fPagingFD;
	state_t *state;
	uint8_t buf[4];
	uint32_t num = 0;
	uint32_t i;
	int readSize=0;
	char debugBuf[256];

	state = tamalib_get_state();

	int flags = charPosixToFlagPosix("r");
	BYTE mode = posixToFatfsAttrib(flags);
	FRESULT result = f_open(&fPagingFD, (const TCHAR*)path, mode);
	if(result != FR_OK){
		sprintf(debugBuf, "FATAL: Cannot open state file \"%s\" !", path);
		nocashMessage(debugBuf);
		return;
	}
	f_lseek (
			&fPagingFD,   /* Pointer to the file object structure */
			(DWORD)0       /* File offset in unit of byte */
		);
	/* First the magic, then the version, and finally the fields of
	 * the state_t struct written as u8, u16 little-endian or u32
	 * little-endian following the struct order
	 */
	result = f_read(&fPagingFD, buf, (int)4, (UINT*)&readSize);
	num += readSize;
	if (buf[0] != (uint8_t) STATE_FILE_MAGIC[0] || buf[1] != (uint8_t) STATE_FILE_MAGIC[1] ||
		buf[2] != (uint8_t) STATE_FILE_MAGIC[2] || buf[3] != (uint8_t) STATE_FILE_MAGIC[3]) {
		sprintf(debugBuf, "FATAL: Wrong state file magic in \"%s\" !", path);
		nocashMessage(debugBuf);
		return;
	}

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	if (buf[0] != STATE_FILE_VERSION) {
		sprintf(debugBuf, "FATAL: Unsupported version %u (expected %u) in state file \"%s\" !", buf[0], STATE_FILE_VERSION, path);
		nocashMessage(debugBuf);
		/* TODO: Handle migration at a point */
		return;
	}

	
	result = f_read(&fPagingFD, buf, (int)2, (UINT*)&readSize);
	num += readSize;
	*(state->pc) = buf[0] | ((buf[1] & 0x1F) << 8);

	result = f_read(&fPagingFD, buf, (int)2, (UINT*)&readSize);
	num += readSize;
	*(state->x) = buf[0] | ((buf[1] & 0xF) << 8);

	result = f_read(&fPagingFD, buf, (int)2, (UINT*)&readSize);
	num += readSize;
	*(state->y) = buf[0] | ((buf[1] & 0xF) << 8);

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->a) = buf[0] & 0xF;

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->b) = buf[0] & 0xF;

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->np) = buf[0] & 0x1F;

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->sp) = buf[0];

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->flags) = buf[0] & 0xF;

	result = f_read(&fPagingFD, buf, (int)4, (UINT*)&readSize);
	num += readSize;
	*(state->tick_counter) = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	result = f_read(&fPagingFD, buf, (int)4, (UINT*)&readSize);
	num += readSize;
	*(state->clk_timer_timestamp) = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	result = f_read(&fPagingFD, buf, (int)4, (UINT*)&readSize);
	num += readSize;
	*(state->prog_timer_timestamp) = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->prog_timer_enabled) = buf[0] & 0x1;

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->prog_timer_data) = buf[0];

	result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
	num += readSize;
	*(state->prog_timer_rld) = buf[0];

	result = f_read(&fPagingFD, buf, (int)4, (UINT*)&readSize);
	num += readSize;
	*(state->call_depth) = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	for (i = 0; i < INT_SLOT_NUM; i++) {
		result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
		num += readSize;
		state->interrupts[i].factor_flag_reg = buf[0] & 0xF;

		result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
		num += readSize;
		state->interrupts[i].mask_reg = buf[0] & 0xF;

		result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
		num += readSize;
		state->interrupts[i].triggered = buf[0] & 0x1;
	}

	/* First 640 half bytes correspond to the RAM */
	for (i = 0; i < MEM_RAM_SIZE; i++) {
		result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
		num += readSize;
		SET_RAM_MEMORY(state->memory, i + MEM_RAM_ADDR, buf[0] & 0xF);
	}

	/* I/Os are from 0xF00 to 0xF7F */
	for (i = 0; i < MEM_IO_SIZE; i++) {
		result = f_read(&fPagingFD, buf, (int)1, (UINT*)&readSize);
		num += readSize;
		SET_IO_MEMORY(state->memory, i + MEM_IO_ADDR, buf[0] & 0xF);
	}

	f_close(&fPagingFD);
	tamalib_refresh_hw();
}
