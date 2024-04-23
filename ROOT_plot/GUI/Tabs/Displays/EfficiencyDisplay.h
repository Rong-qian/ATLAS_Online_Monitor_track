/**
 * @file EfficiencyDisplay.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "CanvasTab.h"

class EfficiencyDisplay : public CanvasTab {

public:

	EfficiencyDisplay(const TGWindow *p, int width, int height);

	virtual ~EfficiencyDisplay();

	virtual void update() override;

};