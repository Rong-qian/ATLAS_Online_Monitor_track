#include "ResidualsDisplay.h"

#include "TCanvas.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

ResidualsDisplay::ResidualsDisplay(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height) {

}

ResidualsDisplay::~ResidualsDisplay() {

}

void ResidualsDisplay::update() {

	// NOTE: This is synchronized with other UI ops, so we don't
	//       need to include the cd() call in the critical section.
	GetCanvas()->cd();

	DAQData &data = DAQData::getInstance();

	data.lock();
	data.plots.residuals->Draw();
	data.unlock();

	GetCanvas()->Update();
	
}