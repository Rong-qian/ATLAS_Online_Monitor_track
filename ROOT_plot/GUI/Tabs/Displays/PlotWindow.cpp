#include "PlotWindow.h"

#include "GUI/Core/UISignals.h"

using namespace std;

PlotWindow::PlotWindow(
	const TGWindow *p, 
	int canvasPanels,
	const string &title, 
	int w, 
	int h,
	int rows
) : CanvasTab(p, title, w, h) {

	int count = canvasPanels;
	int cols = count / rows;
    if(count % rows != 0) ++cols;

    GetCanvas()->Divide(cols, rows);

	SetWindowName(title.data());
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();

}

PlotWindow::~PlotWindow() {

}