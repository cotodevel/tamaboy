#include "typedefsTGDS.h"
#include "tamalib.h"
#include "hal_types.h"
#include "hal.h"
#include "rom.h"
#include "dmaTGDS.h"
#include "tama_process.h"
#include "clockTGDS.h"
#include "ipcfifoTGDS.h"

bool_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH] = {{0}};
bool_t icon_buffer[ICON_NUM] = {0};

const bool_t icons[ICON_NUM][ICON_SIZE][ICON_SIZE] = {
	{
		{1, 0, 1, 0, 1, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 1, 1, 1, 1, 0, 1, 1},
		{0, 1, 1, 1, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 0, 1, 1},
		{0, 0, 1, 0, 0, 0, 0, 1},
		{0, 0, 1, 0, 0, 0, 0, 1},
	},
	{
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{1, 0, 1, 0, 0, 1, 0, 1},
		{0, 0, 1, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	},
	{
		{1, 1, 1, 0, 0, 1, 1, 1},
		{1, 0, 1, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 1, 1, 1, 1},
		{0, 0, 0, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
	},
	{
		{0, 0, 0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 1, 1, 0},
		{0, 0, 0, 1, 1, 1, 1, 1},
		{0, 0, 1, 1, 0, 1, 1, 1},
		{0, 1, 1, 0, 1, 1, 0, 1},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0},
	},
	{
		{0, 1, 1, 0, 0, 0, 0, 0},
		{1, 0, 0, 1, 0, 0, 1, 1},
		{1, 0, 0, 1, 1, 1, 0, 1},
		{0, 1, 0, 1, 0, 0, 1, 1},
		{0, 1, 0, 0, 1, 1, 0, 1},
		{0, 1, 0, 0, 0, 0, 0, 1},
		{0, 1, 1, 0, 0, 0, 1, 1},
		{0, 0, 1, 1, 1, 1, 1, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 0, 0, 1, 0, 0, 0, 1},
		{0, 1, 0, 0, 1, 0, 1, 0},
		{0, 0, 1, 1, 1, 1, 0, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 1, 0, 1},
		{0, 1, 1, 1, 0, 1, 0, 1},
		{1, 1, 1, 0, 0, 1, 0, 1},
		{1, 1, 0, 0, 1, 0, 1, 0},
		{1, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0},
	},
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0, 0, 0},
		{1, 1, 0, 1, 1, 1, 1, 0},
		{1, 0, 1, 0, 0, 1, 0, 1},
		{0, 1, 1, 1, 1, 0, 1, 1},
		{0, 0, 0, 1, 0, 0, 0, 1},
		{0, 0, 0, 0, 1, 1, 1, 0},
	},
};

static void do_nothing(void) {
    return;
}

//RTC
time_t time_get(void){
	struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress;
	time_t t = TGDSIPC->tmInst.tm_sec; 
	uint32_t cnt = getNDSRTCInSeconds(); 
	return (t << 23) | (cnt << 7);
}

timestamp_t hal_get_timestamp(void)
{
	return (timestamp_t)time_get();
}

static u16 thisFreq = 0;
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void set_frequency(u32_t freq) {
    short const freq_table[] = {(2076<<4),2068<<4,2060<<4,2830<<5,2044<<4,2028<<4,2011<<4,1996<<4};
    int n = freq_table[freq] + 125;
	thisFreq = (u16)n;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void play_frequency(bool_t en) {
	int channel = 9; //PSG
	//Bit24-26  Wave Duty    (0..7) ;HIGH=(N+1)*12.5%, LOW=(7-N)*12.5% (PSG only)
	u8 wavDuty = 6;
	u32 cnt   = SOUND_ONE_SHOT | SOUND_VOL(2) | SOUND_PAN(48) | (3 << 29) | (wavDuty << 24); //3=PSG/Noise)
	if (en){
		cnt = (SCHANNEL_ENABLE | cnt);
	}
	writeARM7SoundChannel(channel, cnt, thisFreq);
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void ClrBuf(){
	/* Dot matrix */
	int j = 0;
	int i = 0;
	for (j = 0; j < LCD_HEIGHT; j++) {
		for (i = 0; i < LCD_WIDTH; i++) {
			draw_square(i * PIXEL_SIZE + LCD_OFFET_X, j * PIXEL_SIZE + LCD_OFFET_Y, PIXEL_SIZE, PIXEL_SIZE, 0);
		}
	}

	/* Icons */
	for (i = 0; i < ICON_NUM; i++) {
		draw_icon((i % 4) * ICON_STRIDE_X + ICON_OFFSET_X, (i / 4) * ICON_STRIDE_Y + ICON_OFFSET_Y, i, 0);
	}
}

//DS is fast. So we can handle pixel by pixel on a small screen
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void hal_update_screen(void){
	u8_t i, j;
	ClrBuf();

	/* Dot matrix */
	for (j = 0; j < LCD_HEIGHT; j++) {
		for (i = 0; i < LCD_WIDTH; i++) {
			if (matrix_buffer[j][i]) {
				draw_square(i * PIXEL_SIZE + LCD_OFFET_X, j * PIXEL_SIZE + LCD_OFFET_Y, PIXEL_SIZE, PIXEL_SIZE, 1);
			}
		}
	}

	/* Icons */
	for (i = 0; i < ICON_NUM; i++) {
		if (icon_buffer[i]) {
			draw_icon((i % 4) * ICON_STRIDE_X + ICON_OFFSET_X, (i / 4) * ICON_STRIDE_Y + ICON_OFFSET_Y, i, 1);
		}
	}
	dmaTransferWord(3, (u32)TAMA_DRAW_BUFFER, (u32)VRAM_BUFFER, (uint32)(64*1024));
}

void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val)
{
	matrix_buffer[y][x] = val;
}

void hal_set_lcd_icon(u8_t icon, bool_t val)
{
	if (icon_buffer[icon] == 0 && val == 1) {
		/* The Tamagotchi is calling */
		/*
		if (menu_is_visible()) {
			menu_close();
		}
		*/
	}

	icon_buffer[icon] = val;
}

/* global variables */
hal_t tama_hal = {
    .get_timestamp = &hal_get_timestamp,
    .set_lcd_matrix = &hal_set_lcd_matrix,
    .set_lcd_icon = &hal_set_lcd_icon,
    .set_frequency = set_frequency,
    .play_frequency = play_frequency,

    .sleep_until = do_nothing,
    .update_screen = do_nothing,
    .handler = do_nothing
};
