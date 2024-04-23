#include "EventDisplay.h"

#include "TCanvas.h"

using namespace std;

EventDisplay::EventDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

	// Make sure an update happens when the tab is opened
	update();

}

EventDisplay::~EventDisplay() {

}

void EventDisplay::update() {

	GetCanvas()->cd();
	
}