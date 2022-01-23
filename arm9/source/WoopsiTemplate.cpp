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
#include "VideoGL.h"
#include "videoTGDS.h"
#include "math.h"
#include "imagepcx.h"
#include "dswnifi_lib.h"
#include "tamalib.h"
#include "tama_process.h"
#include "hal.h"
#include "hw.h"
#include "rom.h"
#include "state.h"

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
void WoopsiTemplate::startup(int argc, char **argv)   {
	
	//Init Main Engine (A) as 3D
	bool isTGDSCustomConsole = true;	//set default console or custom console: custom console
	GUI_init(isTGDSCustomConsole);
	GUI_clear();
	
	//xmalloc init removes args, so save them
	int i = 0;
	for(i = 0; i < argc; i++){
		argvs[i] = argv[i];
	}

	bool isCustomTGDSMalloc = true;
	setTGDSMemoryAllocator(getProjectSpecificMemoryAllocatorSetup(TGDS_ARM7_MALLOCSTART, TGDS_ARM7_MALLOCSIZE, isCustomTGDSMalloc, TGDSDLDI_ARM7_ADDRESS));
	sint32 fwlanguage = (sint32)getLanguage();
	
	//argv destroyed here because of xmalloc init, thus restore them
	for(i = 0; i < argc; i++){
		argv[i] = argvs[i];
	}

	switch_dswnifi_mode(dswifi_idlemode);
	asm("mcr	p15, 0, r0, c7, c10, 4");
	flush_icache_all();
	flush_dcache_all();
	
	int ret=FS_init();
	if (ret == 0)
	{
		
	}
	else{
		
	}
	
	//tama setup
	//render TGDSLogo from a LZSS compressed file
	RenderTGDSLogoMainEngine((uint8*)&TGDSLogoLZSSCompressed[0], TGDSLogoLZSSCompressed_size);
	//apply scaling to main engine
	REG_BG3PA = stretch_x;
	REG_BG3PB = 0; REG_BG3PC = 0;
	REG_BG3PD = regBG3PD;
	REG_BG3X = regBG3X;
	REG_BG3Y = regBG3Y;
	
	disableWaitForVblankC();
    tamalib_register_hal(&tama_hal);
    tamalib_init(g_program, NULL, 1000);
    cpu_set_speed(0);
    cycle_count = cpu_get_state()->tick_counter;
    
	//restore last state
	
	//Load state now.
	reEnableVblank = false;
	state_load(STATE_TEMPLATE);
	Rect rect;

	/** SuperBitmap preparation **/
	// Create bitmap for superbitmap
	Bitmap* superBitmapBitmap = new Bitmap(164, 191);

	// Get a graphics object from the bitmap so that we can modify it
	Graphics* gfx = superBitmapBitmap->newGraphics();

	// Clean up
	delete gfx;

	// Create screens
	uiScreen = new AmigaScreen(TGDSPROJECTNAME, Gadget::GADGET_DRAGGABLE, AmigaScreen::AMIGA_SCREEN_SHOW_DEPTH);
	woopsiApplication->addGadget(uiScreen);
	uiScreen->setPermeable(true);

	// Add child windows
	AmigaWindow* controlWindow = new AmigaWindow(0, 13, 256, 33, "Controls", Gadget::GADGET_DRAGGABLE, AmigaWindow::AMIGA_WINDOW_SHOW_DEPTH);
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
	
	enableDrawing();	// Ensure Woopsi can now draw itself
	redraw();			// Draw initial state
	
}

void WoopsiTemplate::shutdown()   {
	Woopsi::shutdown();
}

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
				state_save(STATE_TEMPLATE);
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
	
	//tama process
	tama_process();
}