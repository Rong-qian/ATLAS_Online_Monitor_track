/**
 * @file EventTab.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGButton.h"
#include "TGFrame.h"

#include "CanvasTab.h"

#include "src/EventDisplay.h"

#include "DAQMonitor/DataModel/DAQData.h"

class EventTab : public CanvasTab {

public:

	EventTab(const TGWindow *p, int width, int height);

	virtual ~EventTab();

	virtual void update() override;

	virtual void showNextEvent    ();
	virtual void showPreviousEvent();

private:

	TGHorizontalFrame *buttonFrame;

		TGTextButton *leftButton ;
		TGTextButton *rightButton;

	int currentEventIndex;

	void resetButtonStates(DAQData &data);

	virtual void makeConnections() override;

};