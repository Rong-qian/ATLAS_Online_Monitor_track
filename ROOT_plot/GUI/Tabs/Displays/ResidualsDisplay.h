/**
 * @file ResidualsDisplay.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "CanvasTab.h"

class ResidualsDisplay : public CanvasTab {

public:

	ResidualsDisplay(const TGWindow *p, int width, int height);

	virtual ~ResidualsDisplay();

	virtual void update() override;

};