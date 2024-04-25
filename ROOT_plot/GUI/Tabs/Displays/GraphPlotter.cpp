#include "GraphPlotter.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

GraphPlotter::GraphPlotter(
	const TGWindow *p, 
	vector<TGraph*> graphs, 
	const string &title, 
	int w, 
	int h,
	int rows
) : PlotWindow(p, graphs.size(), title, w, h, rows), 
    graphs(graphs) {

}

GraphPlotter::~GraphPlotter() {

}

void GraphPlotter::update() {

	for(int i = 0; i < graphs.size(); ++i) {

		GetCanvas()->cd(i + 1);

		// TODO: Try to break this dependence on DAQData. It's messy as heck,
		//       and makes it weird to use the GraphPlotter for anything 
		//       that's not in DAQData

		DAQData &data = DAQData::getInstance();

		data.lock();
		graphs[i]->Draw("AB");
		data.unlock();

		/*
		TText *xlabel = new TText();
		xlabel->SetNDC();
		xlabel->SetTextFont(42);
		xlabel->SetTextSize(0.05);
		xlabel->SetTextAngle(0);
		string text_content;
		xlabel->DrawText(0.5, 0.9, text_content.c_str());
		*/

	}

	GetCanvas()->Update();

}