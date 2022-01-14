#ifndef __tama_process_h__
#define __tama_process_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "fatfslayerTGDS.h"
#include "utilsTGDS.h"

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void setup_vram(void);
extern void tama_process();

extern u32* cycle_count;
extern u32 next_frame_count;
extern int next_frame_overflow;

#ifdef __cplusplus
}
#endif