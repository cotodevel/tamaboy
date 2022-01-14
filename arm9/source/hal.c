#include "typedefsTGDS.h"
#include "tamalib.h"
#include "hal_types.h"
#include "hal.h"
#include "rom.h"

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void copy_mono_pixels(int* dest, int graphics, char zero, char one) {
    int data;
    int i = 1;
    do {
        data = (graphics&i) ? one : zero;
        i <<= 1;
        data |= ((graphics&i) ? one : zero) << 8;
        i <<= 1;
        data |= ((graphics&i) ? one : zero) << 16;
        i <<= 1;
        data |= ((graphics&i) ? one : zero) << 24;
        i <<= 1;
        *(dest++) = data;
    } while (i != 0);
}

static void do_nothing(void) {
    return;
}

static timestamp_t get_timestamp(void) {
    return 0;
}

static void set_lcd_matrix(u8_t x, u8_t y, bool_t val) {
    int index;
    val = val ? 1 : 0;
    index = y*32 + x;
    LCD_BUFFER[index] = val;
}
static void set_lcd_icon(u8_t icon, bool_t en) {
    if (((lcd_icon_state>>icon)&1) != en) {
        int x = (icon&3)*4;
        int y = (icon>=4)?4:0;
        int col = en ? 1 : 3;
        int dx=0;
		for (dx=0; dx<4; dx++) {
            int dy=0;
			for (dy=0; dy<4; dy++) {
               int i = 2 * ( (y+dy)*16 + dx+x );
               copy_mono_pixels(TILES+8*(i+4), LCD_ICONS_RAW[i], 2,col);
               copy_mono_pixels(TILES+8*(i+5), LCD_ICONS_RAW[i+1], 2,col);
            }
        }
        show_overlay = 3*60;
        lcd_icon_state ^= 1<<icon;
    }
}

static void set_frequency(u32_t freq) {
    short const freq_table[] = {2016,2008,2000,1992,1984,1968,1951,1936};
    int n = freq_table[freq];
    //REG_SOUND1CNT_X = n | 1<<15; //todo sound
}
static void play_frequency(bool_t en) {
    //if (en) REG_SOUNDCNT_L=0x1177; //todo sound
    //else REG_SOUNDCNT_L=0x0077; //todo sound
}

/* global variables */
hal_t tama_hal = {
    .get_timestamp = get_timestamp,
    .set_lcd_matrix = set_lcd_matrix,
    .set_lcd_icon = set_lcd_icon,
    .set_frequency = set_frequency,
    .play_frequency = play_frequency,

    .sleep_until = do_nothing,
    .update_screen = do_nothing,
    .handler = do_nothing
};
u8 LCD_BUFFER[1024];
int show_overlay;
int lcd_icon_state;
