#include "EfficiencyDisplay.h"

#include "TCanvas.h"

using namespace std;

EfficiencyDisplay::EfficiencyDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

	// Make sure an update happens when the tab is opened
	update();

}

EfficiencyDisplay::~EfficiencyDisplay() {

}

void EfficiencyDisplay::update() {

	GetCanvas()->cd();
	
}