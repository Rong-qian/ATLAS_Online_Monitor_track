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

		endButton = new TGTextButton(buttonFrame, "->|");
		buttonFrame->AddFrame(endButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		rightButton = new TGTextButton(buttonFrame, "-->");
		buttonFrame->AddFrame(rightButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		autoDisplay = new TGTextButton(buttonFrame, "Start Autoplay");
		buttonFrame->AddFrame(autoDisplay, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		leftButton = new TGTextButton(buttonFrame, "<--");
		buttonFrame->AddFrame(leftButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

		begButton = new TGTextButton(buttonFrame, "|<-");
		buttonFrame->AddFrame(begButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY));

	begButton  ->SetEnabled(false);
	rightButton->SetEnabled(false);
	leftButton ->SetEnabled(false);
	endButton  ->SetEnabled(false);

	// TODO: Weird behavior when opened after a run has started.

	toggleAutoplay();

	makeConnections();

}

void EventTab::makeConnections() {

	begButton  ->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	begButton  ->Connect("Clicked()", "EventTab", this, "showFirstEvent()"   );

	leftButton ->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	leftButton ->Connect("Clicked()", "EventTab", this, "showPreviousEvent()");

	rightButton->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	rightButton->Connect("Clicked()", "EventTab", this, "showNextEvent()"    );

	autoDisplay->Connect("Clicked()", "EventTab", this, "toggleAutoplay()"   );

	endButton  ->Connect("Clicked()", "EventTab", this, "stopAutoplay()"     );
	endButton  ->Connect("Clicked()", "EventTab", this, "showLastEvent()"    );

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

	// Lock data to avoid race conditions
	data.lock();

	if(data.eventDisplayBuffer.empty()) {

		data.unlock();

		return false;

	}

	// If currentEventIndex is out of bounds, just stop
	if(currentEventIndex >= data.eventDisplayBuffer.size()) {

		data.unlock();

		return false;

	}

	// Copy the size and event while we're sure data isn't getting modified
	//Event e = data.eventDisplayBuffer[currentEventIndex];
	Event e = data.eventDisplayBuffer.at(currentEventIndex);

	
	size_t size = data.eventDisplayBuffer.size();

	// Release data before the draw to avoid slow operations in the critical
	// section. We've already gotten local copies of the members we need.
	data.unlock();

	// Clear the canvas
	GetCanvas()->Clear();

	// Clear the last draw's hits and tracks
	data.eventDisplay.Clear(); 

	// Draw the event
	data.eventDisplay.DrawEvent(
		GetCanvas(),
		e,
		data.geo,
		NULL,
		true
	);

	// Update the interface around the canvas
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

void EventTab::showFirstEvent() {

	currentEventIndex = 0;

	showCurrentEvent();

}

void EventTab::showLastEvent() {

	DAQData &data = DAQData::getInstance();

	data.lock();

	if(data.eventDisplayBuffer.empty()) {

		data.unlock();
		return;

	}
	currentEventIndex = data.eventDisplayBuffer.size() - 1;

	data.unlock();

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

	size_t size = data.eventDisplayBuffer.size();

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
		begButton ->SetEnabled(true);
	}
	if(currentEventIndex + 1 < size) {
		rightButton->SetEnabled(true);
		endButton  ->SetEnabled(true);
	}
	if(currentEventIndex < 1) {
		leftButton->SetEnabled(false);
		begButton ->SetEnabled(false);
	}
	if(currentEventIndex + 2 > size) {
		rightButton->SetEnabled(false);
		endButton  ->SetEnabled(false);
	}
	data.unlock();

}
