#include "EfficiencyDisplay.h"

#include "TCanvas.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

EfficiencyDisplay::EfficiencyDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

}

EfficiencyDisplay::~EfficiencyDisplay() {

}

void EfficiencyDisplay::update() {

	// NOTE: This is synchronized with other UI ops, so we don't
	//       need to include the cd() call in the critical section.
	GetCanvas()->cd();

	DAQData &data = DAQData::getInstance();

	data.lock();
	data.plots.tube_efficiency->Draw("colz");
	data.unlock();

	GetCanvas()->Update();
	
}