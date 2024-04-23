#include "EventTab.h"

#include "TCanvas.h"

using namespace std;

EventTab::EventTab(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

	// display.SetCanvas(GetCanvas());

	// Make sure an update happens when the tab is opened
	update();

}

EventTab::~EventTab() {

}

void EventTab::update() {

	GetCanvas()->cd();
	
}