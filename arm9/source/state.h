/*
 * MCUGotchi - A Tamagotchi P1 emulator for microcontrollers
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
#ifndef _STATE_H_
#define _STATE_H_

#include "typedefsTGDS.h"

//User storage
#define STATE_SLOTS_NUM					10 //the entire slot count in the FS
#define STATE_SLOT_SIZE					256 // x (sizeof(u32)) = 1024 bytes each slot in FS

//Internal
#define STORAGE_SLOTS_OFFSET				64 // in words (sizeof(u32))

#endif /* _STATE_H_ */

#ifdef __cplusplus
extern "C" {
#endif

extern void state_save(uint8 slot);
extern void state_load(uint8 slot);
extern void state_erase(uint8 slot);
extern uint8 state_check_if_used(uint8 slot);

#ifdef __cplusplus
}
#endif

