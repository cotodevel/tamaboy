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
#ifndef _STATE_H_
#define _STATE_H_

#define STATE_TEMPLATE			"save%u.bin"

#endif /* _STATE_H_ */

#ifdef __cplusplus
extern "C" {
#endif

extern void state_find_next_name(char *path);
extern void state_find_last_name(char *path);
extern void state_save(char *path);
extern void state_load(char *path);

#ifdef __cplusplus
}
#endif
