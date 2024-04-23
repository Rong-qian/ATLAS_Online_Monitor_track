/**
 * @file EventTab.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "CanvasTab.h"

#include "MuonReco/EventDisplay.h"

class EventTab : public CanvasTab {

public:

	EventTab(const TGWindow *p, int width, int height);

	virtual ~EventTab();

	virtual void update() override;

private:

	// EventDisplay display;

};