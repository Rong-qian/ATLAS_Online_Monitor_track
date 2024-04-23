#include "Decoder.h"

#include <utility>

#include "EventDecoding.h"
#include "SignalDecoding.h"

using namespace std;
using namespace MuonReco;

///////////////////////////////////////////////////////////////////////////////
//////////////////////// HELPER FUNCTIONS /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Returns true if signals represents a complete event (i.e. if signals ends
 * with a trailer).
 */
bool isEvent(const vector<Signal> &signals);

///////////////////////////////////////////////////////////////////////////////
/////////////////////// IMPLEMENTATION ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Consider separating decoding from validation to better observe a
//       one-function-one-job pattern. Also consider separating out event
//       processing. That can be separate too.

Decoder::Decoder(int maxSignalCount) : maxSignalCount(maxSignalCount) {}

DecodeData Decoder::decodeStream(
	istream        &in      , 
	Geometry       &geo     , 
	TimeCorrection &tc      ,
	RecoUtility    &recoUtil
) {

	vector<Event> eventBuffer;

	DecodeData result;

	int signalCount = 0;

	// As long as unread signals exist in the input stream,
	while(hasSignals(in)) {

		// Abort the loop if we have at least one finished event, but have too
		// many signals. Limits memory footprint for applications where all 
		// processed events do not need to be preserved.
		if(maxSignalCount > 0) {
			if(!eventBuffer.empty() && signalCount > maxSignalCount) {
				break;
			}
		}

		// extract a signal,
		Signal sig = extractSignal(in);
		++signalCount;

		// validate it
		if(validateSignalErrors(sig, geo)) {

			// TODO: We should keep some metadata if it's a TDC header or
			//       trailer. See DecodeOffline.cpp

			signalBuffer.push_back(sig);

		} else {

			cerr << "Dropped signal" << endl;
			++result.droppedSignals;

		}

		validateSignalWarnings(sig, geo);

		// and, if it completes an event,
		if(isEvent(signalBuffer)) {

			// create the event,
			Event e = assembleEvent(signalBuffer);

			// and validate the event.
			if(validateEventErrors(e)) {

				eventBuffer.push_back(e);

			} else {

				cerr << "Dropped event" << endl;

				++result.droppedEvents;

			}

			signalBuffer.clear();

			validateEventWarnings(e);

		}

	}

	// Once all signals have been extracted,

	// count the events,
	result.eventCount = eventBuffer.size();

	// validate the buffer,
	eventBufferValidator.validateWarnings(eventBuffer);

	// and for each event
	for(Event &e : eventBuffer) {

		// if it's nonempty
		if(e.TrigSignals().back().HitCount() != 0) {

			// process it
			processEvent(e, geo, tc, recoUtil);

			// and store it in the return vector.
			result.nonemptyEvents.push_back(e);

		}

	}

	// Finally, return all processed events.
	return result;

}

bool isEvent(const vector<Signal> &signals) {
	
	if(signals.size() > 1) {

		// As long as the data is well-formed, any signal vector that ends in an
		// event trailer represents an event.
		return signals.back().isEventTrailer();

	}

	return false;

}

bool hasNewData(istream &in) {

	return hasSignals(in);

}