/*
 * A Tamagotchi P1 emulator for microcontrollers
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
#include "typedefsTGDS.h"
#include "storage.h"
#include "state.h"
#include "fatfslayerTGDS.h"

int globalStatePos = -1;
sint8 storage_read(u32 offset, u32 *data, u32 length){
	if (length == 0) {
		/* Nothing to do */
		return 0;
	}
	FILE * fout = fopen(SAV_FILENAME, "r");
	if(fout != NULL){
		int readSize = 0;
		fseek(fout, (STATE_SLOT_SIZE * sizeof(u32) * (int)offset), SEEK_SET);
		readSize = fread((u8*)data, 1, length, fout);
		fclose(fout);
		return readSize;
	}
	return 0;
}

sint8 storage_write(u32 offset, u32 *data, u32 length)
{
	if (length == 0) {
		/* Nothing to do */
		return 0;
	}
	FILE * fout = fopen(SAV_FILENAME, "r+");
	if(fout != NULL){
		int writtenSize = 0;
		fseek(fout, (STATE_SLOT_SIZE * sizeof(u32) * (int)offset), SEEK_SET);
		writtenSize = fwrite((u8*)data, 1, length, fout);
		if(writtenSize != length){
			*(u32*)0x02000000 = (u32)0xc373FFFF;
			while(1==1){
			}
		}
		fclose(fout);
		return writtenSize;
	}
	return 0;
}
