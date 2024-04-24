#include "EventTab.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace MuonReco;
using namespace std;

EventTab::EventTab(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height - 20),
	  currentEventIndex(0),
	  isAutoplaying(false) {

	buttonFrame = new TGHorizontalFrame(this);
	AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

		rightButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(rightButton, new TGLayoutHints(kLHintsRight));

		autoDisplay = new TGTextButton(buttonFrame, "Start Autoplay");
		buttonFrame->AddFrame(autoDisplay, new TGLayoutHints(kLHintsRight));

		// TODO: Implement autoplay feature

		leftButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(leftButton, new TGLayoutHints(kLHintsRight));

	rightButton->SetEnabled(false);
	leftButton->SetEnabled(false);

	// TODO: Weird behavior when opened after a run has started.

	toggleAutoplay();

	// Make sure an update happens when the tab is opened
	update();

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

	GetCanvas()->Clear();
	data.eventDisplay.DrawEvent(
		GetCanvas(),
		*data.plots.eventDisplayBuffer[currentEventIndex],
		data.geo,
		NULL,
		true
	);

	data.unlock();

	return true;

}

void EventTab::showNextEvent() {

	++currentEventIndex;

	if(!showCurrentEvent()) --currentEventIndex;

	resetButtonStates();

}

void EventTab::showPreviousEvent() {

	if(currentEventIndex < 1) return;

	--currentEventIndex;

	showCurrentEvent();

	resetButtonStates();

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

	} else {

		if(currentEventIndex >= size) {

			currentEventIndex = 0;

		}

		if(currentEventIndex == 0 && size != 0) {

			showCurrentEvent();

		}

	}

	resetButtonStates();

}

void EventTab::resetButtonStates() {

	// NOTE: eventDisplayBuffer.size() is a size_t. Don't subtract from it or
	//       you might end up wrapping around to the very large numbers.

	DAQData &data = DAQData::getInstance();

	data.lock();
	if(currentEventIndex > 0) {
		leftButton->SetEnabled(true);
	}
	if(currentEventIndex + 1 < data.plots.eventDisplayBuffer.size()) {
		rightButton->SetEnabled(true);
	}
	if(currentEventIndex < 1) {
		leftButton->SetEnabled(false);
	}
	if(currentEventIndex + 2 > data.plots.eventDisplayBuffer.size()) {
		rightButton->SetEnabled(false);
	}
	data.unlock();

}