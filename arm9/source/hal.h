#ifndef __hal_h__
#define __hal_h__

static int* const TILES = (int*)0x0600c000;
static u16* const LCD_MAP = (u16*)0x06000000; /* 1k */
static u16* const ICONS_MAP = (u16*)0x06000800; /* 256 bytes */

#define PIXEL_SIZE					3
#define ICON_SIZE					8
#define ICON_STRIDE_X					24
#define ICON_STRIDE_Y					56
#define ICON_OFFSET_X					24
#define ICON_OFFSET_Y					0
#define LCD_OFFET_X					16
#define LCD_OFFET_Y					8

#endif

#ifdef __cplusplus
extern "C"{
#endif

extern hal_t tama_hal;
extern u8 LCD_BUFFER[1024];
extern int show_overlay;
extern int lcd_icon_state;
extern void copy_mono_pixels(int* dest, int graphics, char zero, char one);
extern void ClrBuf();
extern void hal_update_screen(void);
extern bool_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH];
extern bool_t icon_buffer[ICON_NUM];
extern const bool_t icons[ICON_NUM][ICON_SIZE][ICON_SIZE];

extern void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val);
extern void hal_set_lcd_icon(u8_t icon, bool_t val);

#ifdef __cplusplus
}
#endif