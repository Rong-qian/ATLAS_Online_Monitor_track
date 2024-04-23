/**
 * @file PlotWindow.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"

#include "CanvasTab.h"

class PlotWindow : public CanvasTab {

public:

	PlotWindow(
		const TGWindow *p, 
		int canvasPanels = 1,
		const std::string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	virtual ~PlotWindow();

protected:

	// VIEW

	TRootEmbeddedCanvas *canvas;

};