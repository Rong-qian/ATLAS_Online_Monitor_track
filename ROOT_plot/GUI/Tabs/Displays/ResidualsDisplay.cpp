#include "ResidualsDisplay.h"

#include "TCanvas.h"

using namespace std;

ResidualsDisplay::ResidualsDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

	// Make sure an update happens when the tab is opened
	update();

}

ResidualsDisplay::~ResidualsDisplay() {

}

void ResidualsDisplay::update() {

	GetCanvas()->cd();
	
}