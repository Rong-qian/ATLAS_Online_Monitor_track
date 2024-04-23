/**
 * @file EventDisplay.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "CanvasTab.h"

class EventDisplay : public CanvasTab {

public:

	EventDisplay(const TGWindow *p, int width, int height);

	virtual ~EventDisplay();

	virtual void update() override;

};