#include "EventTab.h"

using namespace MuonReco;
using namespace std;

// TODO: DEBUG
#include <iostream>

EventTab::EventTab(const TGWindow *p, int width, int height) 
	: CanvasTab(p, "Event Display", width, height - 20),
	  currentEventIndex(-1) {

	buttonFrame = new TGHorizontalFrame(this);
	AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

		rightButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(rightButton, new TGLayoutHints(kLHintsRight));

		leftButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(leftButton, new TGLayoutHints(kLHintsRight));

	rightButton->SetEnabled(false);
	leftButton->SetEnabled(false);

	// TODO: Weird behavior when opened after a run has started.

	// Make sure an update happens when the tab is opened
	update();

	makeConnections();

}

void EventTab::makeConnections() {

	leftButton ->Connect("Clicked()", "EventTab", this, "showPreviousEvent()");
	rightButton->Connect("Clicked()", "EventTab", this, "showNextEvent()"    );

}

EventTab::~EventTab() {

}

void EventTab::showNextEvent() {

	DAQData &data = DAQData::getInstance();

	data.lock();

	if(currentEventIndex + 1 >= data.plots.eventDisplayBuffer.size()) {

		data.unlock();
		return;

	}

	Event *e = data.plots.eventDisplayBuffer[currentEventIndex + 1];

	++currentEventIndex;

	resetButtonStates(data);

	GetCanvas()->Clear();
	data.eventDisplay.DrawEvent(GetCanvas(), *e, data.geo, nullptr, true);

	data.unlock();

}

void EventTab::showPreviousEvent() {

	DAQData &data = DAQData::getInstance();

	data.lock();
	if(currentEventIndex - 1 < 0) {

		data.unlock();
		return;

	}

	Event *e = data.plots.eventDisplayBuffer[currentEventIndex - 1];

	--currentEventIndex;

	resetButtonStates(data);

	GetCanvas()->Clear();
	data.eventDisplay.DrawEvent(GetCanvas(), *e, data.geo, nullptr, true);

	data.unlock();

}

void EventTab::update() {

	DAQData &data = DAQData::getInstance();

	data.lock();

	if(currentEventIndex >= data.plots.eventDisplayBuffer.size()) {

		currentEventIndex = -1;

	}

	resetButtonStates(data);

	if(currentEventIndex < 0 && !data.plots.eventDisplayBuffer.empty()) {

		data.unlock();
		showNextEvent();

	}

	data.unlock();

}

void EventTab::resetButtonStates(DAQData &data) {

	// NOTE: eventDisplayBuffer.size() is a size_t. Don't subtract from it or
	//       you might end up wrapping around to the very large numbers.

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

}