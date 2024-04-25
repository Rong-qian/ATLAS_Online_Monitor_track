#include "EventTab.h"

#include "DAQMonitor/DataModel/DAQData.h"

// TODO: DEBUG
#include <iostream>

using namespace MuonReco;
using namespace std;

EventTab::EventTab(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height - 20),
	  currentEventIndex(0),
	  isAutoplaying(false) {

	buttonFrame = new TGHorizontalFrame(this);
	AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

		eventNum = new TGLabel(buttonFrame, "No events to display!");
		buttonFrame->AddFrame(eventNum, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

		rightButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(rightButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		autoDisplay = new TGTextButton(buttonFrame, "Start Autoplay");
		buttonFrame->AddFrame(autoDisplay, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		leftButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(leftButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

	rightButton->SetEnabled(false);
	leftButton->SetEnabled(false);

	// TODO: Weird behavior when opened after a run has started.

	toggleAutoplay();

	makeConnections();

}

void EventTab::makeConnections() {

	leftButton ->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	leftButton ->Connect("Clicked()", "EventTab", this, "showPreviousEvent()");

	rightButton->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	rightButton->Connect("Clicked()", "EventTab", this, "showNextEvent()"    );

	autoDisplay->Connect("Clicked()", "EventTab", this, "toggleAutoplay()"   );

}

EventTab::~EventTab() {

}

void EventTab::toggleAutoplay() {

	if(isAutoplaying) {

		stopAutoplay();

		return;

	}

	startAutoplay();

}

void EventTab::stopAutoplay() {

	isAutoplaying = false;

	autoDisplay->SetText("Start Autoplay");

}

void EventTab::startAutoplay() {

	isAutoplaying = true;

	autoDisplay->SetText("Stop Autoplay");

}

bool EventTab::showCurrentEvent() {

	DAQData &data = DAQData::getInstance();

	data.lock();

	if(currentEventIndex >= data.plots.eventDisplayBuffer.size()) {

		data.unlock();

		return false;

	}

	Event e = data.plots.eventDisplayBuffer[currentEventIndex];
	size_t size = data.plots.eventDisplayBuffer.size();

	data.unlock();

	GetCanvas()->Clear();
	data.eventDisplay.DrawEvent(
		GetCanvas(),
		e,
		data.geo,
		NULL,
		true
	);

	updateLabel(size);
	resetButtonStates(size);

	return true;

}

void EventTab::showNextEvent() {

	++currentEventIndex;

	if(!showCurrentEvent()) --currentEventIndex;

}

void EventTab::showPreviousEvent() {

	if(currentEventIndex < 1) return;

	--currentEventIndex;

	showCurrentEvent();

}

void EventTab::updateLabel(size_t size) {

	if(size == 0) {

		eventNum->SetText("No events to display!");
		eventNum->Resize(eventNum->GetDefaultSize());

		return;

	}

	eventNum->SetText((
		string("Displaying event ") 
			+ to_string(currentEventIndex + 1)
			+ string(" of ")
			+ to_string(size)).data()
	);
	eventNum->Resize(eventNum->GetDefaultSize());

}

void EventTab::update() {

	DAQData &data = DAQData::getInstance();

	data.lock();

	size_t size = data.plots.eventDisplayBuffer.size();

	data.unlock();

	// If autoplaying, automatically show the last event in the buffer.
	if(isAutoplaying) {

		if(size != 0) currentEventIndex = size - 1;

		showCurrentEvent();

		return;

	} 

	if(currentEventIndex >= size) currentEventIndex = 0;

	if(currentEventIndex == 0 && size != 0) {

		showCurrentEvent();

		return;

	}

	updateLabel(size);

	resetButtonStates(size);

}

void EventTab::resetButtonStates(size_t size) {

	// NOTE: eventDisplayBuffer.size() is a size_t. Don't subtract from it or
	//       you might end up wrapping around to the very large numbers.

	DAQData &data = DAQData::getInstance();

	data.lock();
	if(currentEventIndex > 0) {
		leftButton->SetEnabled(true);
	}
	if(currentEventIndex + 1 < size) {
		rightButton->SetEnabled(true);
	}
	if(currentEventIndex < 1) {
		leftButton->SetEnabled(false);
	}
	if(currentEventIndex + 2 > size) {
		rightButton->SetEnabled(false);
	}
	data.unlock();

}