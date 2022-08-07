//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
using namespace std;

class AUX_RGBImageRec {
	void convertBGRtoRGB();
	public:
	byte *data;
	DWORD sizeX;
	DWORD sizeY;
	bool NoErrors;
	AUX_RGBImageRec(): NoErrors(false), data(NULL) {};
	AUX_RGBImageRec(const char *FileName);
	~AUX_RGBImageRec();
	bool loadFile(const char *FileName);
	friend AUX_RGBImageRec *auxDIBImageLoad(const char *FileName);
};