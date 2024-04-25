#include "HistogramPlotter.h"

#include "DAQMonitor/DataModel/DAQData.h"

#include "GUI/Core/UILock.h"

using namespace std;

HistogramPlotter::HistogramPlotter(
	const TGWindow *p, 
	vector<TH1*> histograms, 
	const string &title = "", 
	int w = 1, 
	int h = 1,
	int rows = 1
) : PlotWindow(p, histograms.size(), title, w, h, rows), 
    histograms(histograms) {

}

HistogramPlotter::~HistogramPlotter() {

}

void HistogramPlotter::update() {

	for(int i = 0; i < histograms.size(); ++i) {

		GetCanvas()->cd(i + 1);

		// TODO: Try to break this dependence on DAQData. It's messy as heck,
		//       and makes it weird to use the HistogramPlotter for anything 
		//       that's not in DAQData

		DAQData &data = DAQData::getInstance();

		data.lock();
		histograms[i]->Draw();
		data.unlock();
	}

	GetCanvas()->Update();

}