#include "CanvasTab.h"

using namespace std;

CanvasTab::CanvasTab(
	const TGWindow *p,
	const string &title,
	int width,
	int height
) : UITab(p) {

	// Build the TRootEmbeddedCanvas element
	canvas = new TRootEmbeddedCanvas(title.data(), this, width, height);
	AddFrame(canvas, new TGLayoutHints(kLHintsCenterX));

}

CanvasTab::~CanvasTab() {

	delete canvas;
	canvas = nullptr;

}

TCanvas *CanvasTab::GetCanvas() {

	return canvas->GetCanvas();

}