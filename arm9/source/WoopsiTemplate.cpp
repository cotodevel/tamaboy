// Includes
#include "typedefsTGDS.h"
#include "WoopsiTemplate.h"
#include "woopsiheaders.h"
#include "bitmapwrapper.h"
#include "bitmap.h"
#include "graphics.h"
#include "rect.h"
#include "gadgetstyle.h"
#include "fonts/newtopaz.h"
#include "woopsistring.h"
#include "colourpicker.h"
#include "filerequester.h"
#include "soundTGDS.h"
#include "main.h"
#include "posixHandleTGDS.h"
#include "keypadTGDS.h"
#include "gui_console_connector.h"
#include "TGDSLogoLZSSCompressed.h"
#include "videoTGDS.h"
#include "math.h"
#include "imagepcx.h"
#include "dswnifi_lib.h"
#include "tamalib.h"
#include "hal.h"
#include "hw.h"
#include "rom.h"
#include "state.h"
#include "timerTGDS.h"
#include "cpu.h"
#include "program.h"
#include "mem_edit.h"
#include "debugNocash.h"

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void enableWaitForVblankC(){
	WoopsiTemplateProc->enableWaitForVblank();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void disableWaitForVblankC(){
	WoopsiTemplateProc->disableWaitForVblank();
}

__attribute__((section(".dtcm")))
WoopsiTemplate * WoopsiTemplateProc = NULL;

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::startup(int argc, char **argv){

	//tama setup
	//render TGDSLogo from a LZSS compressed file
	RenderTGDSLogoMainEngine((uint8*)&TGDSLogoLZSSCompressed[0], TGDSLogoLZSSCompressed_size);
	
	initSound();
	//apply scaling to main engine
	REG_BG3PA = stretch_x;
	REG_BG3PB = 0; REG_BG3PC = 0;
	REG_BG3PD = regBG3PD;
	REG_BG3X = regBG3X;
	REG_BG3Y = regBG3Y;
	
	disableWaitForVblankC();
    
	//restore last state now.
	reEnableVblank = false;
	Rect rect;

	/** SuperBitmap preparation **/
	// Create bitmap for superbitmap
	Bitmap* superBitmapBitmap = new Bitmap(164, 191);

	// Get a graphics object from the bitmap so that we can modify it
	Graphics* gfx = superBitmapBitmap->newGraphics();

	// Clean up
	delete gfx;

	// Create screens
	uiScreen = new AmigaScreen(TGDSPROJECTNAME, Gadget::GADGET_DECORATION, AmigaScreen::AMIGA_SCREEN_SHOW_DEPTH);
	woopsiApplication->addGadget(uiScreen);
	uiScreen->setPermeable(true);

	// Add child windows
	AmigaWindow* controlWindow = new AmigaWindow(0, 13, 256, 33, "Controls", Gadget::GADGET_DECORATION, AmigaWindow::AMIGA_WINDOW_SHOW_DEPTH);
	uiScreen->addGadget(controlWindow);

	// Controls
	controlWindow->getClientRect(rect);

	_Index = new Button(rect.x, rect.y, 41, 16, "<");	
	_Index->setRefcon(1);
	controlWindow->addGadget(_Index);
	_Index->addGadgetEventHandler(this);
	
	_lastFile = new Button(rect.x + 41, rect.y, 41, 16, "v");
	_lastFile->setRefcon(2);
	controlWindow->addGadget(_lastFile);
	_lastFile->addGadgetEventHandler(this);
	
	_nextFile = new Button(rect.x + 41 + 41, rect.y, 41, 16, ">");
	_nextFile->setRefcon(3);
	controlWindow->addGadget(_nextFile);
	_nextFile->addGadgetEventHandler(this);
	
	_play = new Button(rect.x + 41 + 41 + 41, rect.y, 80, 16, "Save & Exit");
	_play->setRefcon(4);
	controlWindow->addGadget(_play);
	_play->addGadgetEventHandler(this);
	
	_MultiLineTextBoxLogger = NULL;	//destroyable TextBox
	
	//Tama process init --
	
	char rom_path[256] = ROM_PATH;
	char sprites_path[256] = {0};
	bool_t gen_header = 0;
	bool_t extract_sprites = 0;
	bool_t modify_sprites = 0;

	//startTimerCounter(tUnitsMilliseconds, 1000); //tUnitsMilliseconds equals 1 millisecond/unit. A single unit (1) is the default value for normal timer count-up scenarios. 
	tamalib_register_hal(&hal);

	/*
	for (;;) {
		int index;
		int c;

		c = getopt_long(argc, argv, short_options, long_options, &index);

		if (-1 == c)
			break;

		switch (c) {
			case 0:	// getopt_long() flag 
				break;

			case 'r':
				strncpy(rom_path, optarg, 256);
				break;

			case 'E':
				extract_sprites = 1;
				strncpy(sprites_path, optarg, 256);
				break;

			case 'M':
				modify_sprites = 1;
				strncpy(sprites_path, optarg, 256);
				break;

			case 'H':
				gen_header = 1;
				break;

			case 'l':
				strncpy(save_path, optarg, 256);
				break;

			case 's':
				tamalib_set_exec_mode(EXEC_MODE_STEP);
				break;

			case 'b':
				tamalib_add_bp(&g_breakpoints, strtoul(optarg, NULL, 0));
				break;

			case 'm':
				log_levels |= LOG_MEMORY;
				break;

#if !defined(__WIN32__)
			case 'e':
				memory_editor_enable = 1;
				break;
#endif

			case 'c':
				log_levels |= LOG_CPU;
				break;

			case 'v':
				log_levels |= LOG_MEMORY | LOG_CPU;
				break;

			case 'h':
				usage(stdout, argc, argv);
				exit(EXIT_SUCCESS);

			default:
				usage(stderr, argc, argv);
				exit(EXIT_FAILURE);
		}
	}
	*/

	g_program = program_load(&g_program_size);
	if (g_program == NULL) {
		hal_log(LOG_ERROR, "FATAL: Error while loading ROM %s !", rom_path);
		tamalib_free_bp(&g_breakpoints);
		nocashMessage("FATAL: Error while loading ROM");
	}
	else{
		nocashMessage("Load ROM OK\n");
	}

	if (gen_header || extract_sprites || modify_sprites) {
		nocashMessage("ROM manipulation only \n");
		// ROM manipulation only (no emulation) 
		if (gen_header) {
			program_to_header(g_program, g_program_size);
		} else if (extract_sprites) {
			program_get_data(g_program, g_program_size, sprites_path);
		} else if (modify_sprites) {
			program_set_data(g_program, g_program_size, sprites_path);
			program_save(rom_path, g_program, g_program_size);
		}

		TGDSARM9Free(g_program);
		tamalib_free_bp(&g_breakpoints);
	}
	else{
		nocashMessage("ROM running normally.\n");
	}

	compute_layout();
	
	#ifdef WIN32
	if (sdl_init()) {
		hal_log(LOG_ERROR, "FATAL: Error while initializing application !");
		TGDSARM9Free(g_program);
		tamalib_free_bp(&g_breakpoints);
		return -1;
	}
	#endif
	
	if (tamalib_init(g_program, NULL, 1000)) {
		hal_log(LOG_ERROR, "FATAL: Error while initializing tamalib !");
		nocashMessage("FATAL: Error while initializing tamalib !");
		#ifdef WIN32
		sdl_release();
		#endif
		TGDSARM9Free(g_program);
		tamalib_free_bp(&g_breakpoints);
	}
	else{
		nocashMessage("tamalib init OK !");
	}
    cpu_set_speed(SPEED_1X);

	state_load(SAVE_PATH);
	
	/*
	if (memory_editor_enable) {
		// Logs are not compatible with the memory editor 
		log_levels = LOG_ERROR;
		mem_edit_configure_terminal();
	}*/

	//Tama process init end--
	
	enableDrawing();	// Ensure Woopsi can now draw itself
	redraw();			// Draw initial state
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::shutdown()   {
	Woopsi::shutdown();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::waitForAOrTouchScreenButtonMessage(MultiLineTextBox* thisLineTextBox, const WoopsiString& thisText)   {
	thisLineTextBox->appendText(thisText);
	scanKeys();
	while((!(keysDown() & KEY_A)) && (!(keysDown() & KEY_TOUCH))){
		scanKeys();
	}
	scanKeys();
	while((keysDown() & KEY_A) && (keysDown() & KEY_TOUCH)){
		scanKeys();
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleValueChangeEvent(const GadgetEventArgs& e)   {

	// Did a gadget fire this event?
	if (e.getSource() != NULL) {
		/* 
		//Destroyable Textbox implementation init
		Rect rect;
		uiScreen->getClientRect(rect);
		_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
		uiScreen->addGadget(_MultiLineTextBoxLogger);
		
		_MultiLineTextBoxLogger->removeText(0);
		_MultiLineTextBoxLogger->moveCursorToPosition(0);
		_MultiLineTextBoxLogger->appendText("File open OK: ");
		_MultiLineTextBoxLogger->appendText(strObj);
		_MultiLineTextBoxLogger->appendText("\n");
		_MultiLineTextBoxLogger->appendText("Please wait calculating CRC32... \n");
		
		char arrBuild[256+1];
		sprintf(arrBuild, "%s%x\n", "Invalid file: crc32 = 0x", crc32);
		_MultiLineTextBoxLogger->appendText(WoopsiString(arrBuild));
		
		sprintf(arrBuild, "%s%x\n", "Expected: crc32 = 0x", 0x5F35977E);
		_MultiLineTextBoxLogger->appendText(WoopsiString(arrBuild));
		
		waitForAOrTouchScreenButtonMessage(_MultiLineTextBoxLogger, "Press (A) or tap touchscreen to continue. \n");
		
		_MultiLineTextBoxLogger->invalidateVisibleRectCache();
		uiScreen->eraseGadget(_MultiLineTextBoxLogger);
		_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
		//Destroyable Textbox implementation end
		*/
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleLidClosed() {
	// Lid has just been closed
	_lidClosed = true;

	// Run lid closed on all gadgets
	s32 i = 0;
	while (i < _gadgets.size()) {
		_gadgets[i]->lidClose();
		i++;
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleLidOpen() {
	// Lid has just been opened
	_lidClosed = false;

	// Run lid opened on all gadgets
	s32 i = 0;
	while (i < _gadgets.size()) {
		_gadgets[i]->lidOpen();
		i++;
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void printMessage(char * msg){
	//Destroyable Textbox implementation init
	Rect rect;
	WoopsiTemplateProc->uiScreen->getClientRect(rect);
	WoopsiTemplateProc->_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
	WoopsiTemplateProc->uiScreen->addGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->removeText(0);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->moveCursorToPosition(0);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(":");
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("\n");
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(WoopsiString(msg));
	
	WoopsiTemplateProc->waitForAOrTouchScreenButtonMessage(WoopsiTemplateProc->_MultiLineTextBoxLogger, "Press (A) or tap touchscreen to continue. \n");
	
	WoopsiTemplateProc->_MultiLineTextBoxLogger->invalidateVisibleRectCache();
	WoopsiTemplateProc->uiScreen->eraseGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
	WoopsiTemplateProc->_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
	//Destroyable Textbox implementation end
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void WoopsiTemplate::handleClickEvent(const GadgetEventArgs& e)   {
	switch (e.getSource()->getRefcon()) {
		//Tamagotchi Left Button Event
		case 1:{
			bankedButtons |= KEY_LEFT;
		}	
		break;
		
		//Tamagotchi Middle Button Event
		case 2:{
			bankedButtons |= KEY_DOWN;
		}	
		break;
		
		//Tamagotchi Right Button Event
		case 3:{
			bankedButtons |= KEY_RIGHT;
		}	
		break;
		
		//Tamagotchi Save & Exit Button Event
		case 4:{
			//Destroyable Textbox implementation init
			Rect rect;
			WoopsiTemplateProc->uiScreen->getClientRect(rect);
			WoopsiTemplateProc->_MultiLineTextBoxLogger = new MultiLineTextBox(rect.x, rect.y, 262, 170, "Loading\n...", Gadget::GADGET_DRAGGABLE, 5);
			WoopsiTemplateProc->uiScreen->addGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
			
			WoopsiTemplateProc->_MultiLineTextBoxLogger->removeText(0);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->moveCursorToPosition(0);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText(":");
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("\n");
			
			WoopsiTemplateProc->_MultiLineTextBoxLogger->appendText("Are you sure you want to Save & Exit?\n(A) No\n(B) Yes");
			bool pressedB = false;
			while(1==1){
				scanKeys();
				if(keysDown() & KEY_A){
					break;
				}
				if(keysDown() & KEY_B){
					pressedB = true;
					break;
				}
			}
			WoopsiTemplateProc->_MultiLineTextBoxLogger->invalidateVisibleRectCache();
			WoopsiTemplateProc->uiScreen->eraseGadget(WoopsiTemplateProc->_MultiLineTextBoxLogger);
			WoopsiTemplateProc->_MultiLineTextBoxLogger->destroy();	//same as delete _MultiLineTextBoxLogger;
			//Destroyable Textbox implementation end
			
			
			if(pressedB == true){
				state_save(SAVE_PATH);
				printMessage("Tamagotchi has been saved correctly. It's safe to turn off unit now.");
				shutdownNDSHardware();
				while(1==1){
					IRQWait(0, IRQ_VBLANK);
				}
			}
		}	
		break;
		
		
	}
}

__attribute__((section(".dtcm")))
u32 pendPlay = 0;

char currentFileChosen[256+1];

//Called once Woopsi events are ended: TGDS Main Loop
__attribute__((section(".itcm")))
void Woopsi::ApplicationMainLoop()  {
	//Earlier.. main from Woopsi SDK.
	
	//Handle TGDS stuff...
	tama_process();
	HaltUntilIRQ(); //Save power until next irq
}