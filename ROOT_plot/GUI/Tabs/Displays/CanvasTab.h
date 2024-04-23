/**
 * @file CanvasTab.h
 *
 * @brief A class for tabs whose primary element is a TRootEmbeddedCanvas.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "../UITab.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"

class CanvasTab : public UITab {

public:

	CanvasTab(
		const TGWindow *p, 
		const std::string &title, 
		int width, 
		int height
	);

	virtual ~CanvasTab();

	virtual TCanvas *GetCanvas();

protected:

	TRootEmbeddedCanvas *canvas;

};