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
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "typedefsTGDS.h"

#define SAV_FILENAME ((char*)"0:/ndstamaboy.sav")

#endif /* _STORAGE_H_ */


#ifdef __cplusplus
extern "C" {
#endif

extern sint8 storage_read(u32 offset, u32 *data, u32 length);
extern sint8 storage_write(u32 offset, u32 *data, u32 length);
extern int globalStatePos;

#ifdef __cplusplus
}
#endif

