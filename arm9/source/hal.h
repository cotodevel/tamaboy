static int* const TILES = (int*)0x0600c000;
static u16* const LCD_MAP = (u16*)0x06000000; /* 1k */
static u16* const ICONS_MAP = (u16*)0x06000800; /* 256 bytes */

#ifdef __cplusplus
extern "C"{
#endif

extern hal_t tama_hal;
extern u8 LCD_BUFFER[1024];
extern int show_overlay;
extern int lcd_icon_state;
extern void copy_mono_pixels(int* dest, int graphics, char zero, char one);

#ifdef __cplusplus
}
#endif
