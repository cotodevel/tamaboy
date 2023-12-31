#ifndef _DEMO_H_
#define _DEMO_H_

#ifdef __cplusplus
#include "alert.h"
#include "woopsi.h"
#include "woopsiheaders.h"
#include "filerequester.h"
#include "textbox.h"
#include "soundTGDS.h"
#include "button.h"

#include <string>
using namespace std;

using namespace WoopsiUI;

class WoopsiTemplate : public Woopsi, public GadgetEventHandler {
public:
	void startup(int argc, char **argv);
	void shutdown();
	void handleValueChangeEvent(const GadgetEventArgs& e);	//Handles UI events if they change
	void handleClickEvent(const GadgetEventArgs& e);	//Handles UI events when they take click action
	void waitForAOrTouchScreenButtonMessage(MultiLineTextBox* thisLineTextBox, const WoopsiString& thisText);
	void handleLidClosed();
	void handleLidOpen();
	void ApplicationMainLoop();
	
	//Game User Interface screen (touchscreen mapped to Engine B)
	AmigaScreen* uiScreen;
	MultiLineTextBox* _MultiLineTextBoxLogger;
	Button* _Index;
	Button* _lastFile;
	Button* _nextFile;
	Button* _play;
	Button* _stop;
	
	//Todo: add a Save & Exit button which saves state into the FS
	
private:
	Alert* _alert;
};
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern WoopsiTemplate * WoopsiTemplateProc;
#endif

extern u32 pendPlay;
extern char currentFileChosen[256+1];
extern int textureID;
extern float rotateX;
extern float rotateY;
extern float camDist;
extern void printMessage(char * msg);
extern void disableWaitForVblankC();
extern void enableWaitForVblankC();

#ifdef __cplusplus
}
#endif
