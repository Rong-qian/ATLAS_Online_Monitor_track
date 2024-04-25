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
#include "TGLabel.h"

#include "CanvasTab.h"

#include "src/EventDisplay.h"

class EventTab : public CanvasTab {

public:

	EventTab(const TGWindow *p, int width, int height);

	virtual ~EventTab();

	virtual void update() override;

	virtual void showNextEvent    ();
	virtual void showPreviousEvent();
	virtual void stopAutoplay     ();
	virtual void startAutoplay    ();
	virtual void toggleAutoplay   ();

private:

	TGHorizontalFrame *buttonFrame;

		TGLabel      *eventNum   ;

		TGTextButton *leftButton ;
		TGTextButton *autoDisplay;
		TGTextButton *rightButton;


	virtual void resetButtonStates(size_t size);
	virtual void updateLabel      (size_t size);
	virtual bool showCurrentEvent (           );

	virtual void makeConnections() override;

	int  currentEventIndex;
	bool isAutoplaying    ;

};