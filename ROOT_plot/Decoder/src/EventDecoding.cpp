/**
 * @file EventDecoding.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include "EventDecoding.h"

#include <string>
#include <algorithm>

#include "Logging/ErrorLogger.h"

using namespace std;
using namespace MuonReco;

const string EVENT_ERROR = "event" ;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Signal getHeader (const Event &e);
Signal getTrailer(const Event &e);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: We also want some metadata, as described in DecodeOffline
Event assembleEvent(vector<Signal> signals) {

	return Event(
		signals.front(), 
		signals.back(),
		vector<Signal>(
			signals.begin() + 1, 
			signals.end  () - 1
		),
		signals.back().TrailerEID()
	);

}

void processEvent(
	Event          &e       , 
	Geometry       &geo     , 
	TimeCorrection &tc      , 
	RecoUtility    &recoUtil
) {

	int status = 0;

	recoUtil.DoHitFinding(&e, &tc, geo);
	recoUtil.DoHitClustering(&e);

	bool pass_event_check = recoUtil.CheckEvent(e, &status);
	e.SetPassCheck(pass_event_check);
	e.CheckClusterTime();

}

// Validation that, when failed, drops the event
bool validateEventErrors(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	if(!getHeader(e).isEventHeader()) {

		logger.logError(
			"Found event with no header",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	if(!getTrailer(e).isEventTrailer()) {

		logger.logError(
			"Found event with no trailer",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	// Make sure the header and trailer event IDs match
	if(getHeader(e).HeaderEID() != getTrailer(e).TrailerEID()) {

		logger.logError(
			"Found event with mismatched event IDs",
			EVENT_ERROR,
			ERROR
		);

	}

	// Make sure there aren't any extraneous event headers or trailers
	for(const Signal &sig : e.WireSignals()) {

		if(sig.isEventHeader()) {

			logger.logError(
				"Found event with multiple headers",
				EVENT_ERROR,
				ERROR
			);

			return false;

		}

		if(sig.isEventTrailer()) {

			logger.logError(
				"Found event with multiple trailers",
				EVENT_ERROR,
				ERROR
			);

			return false;

		}

	}

	return true;

}

// Validation that, when failed, keeps the event but warns the user.
void validateEventWarnings(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	// Check the event trailer for the header count error flag
	if(getTrailer(e).HeaderCountErr()) {

		logger.logError(
			string("Header count error flag. Got ")
			+ to_string(getTrailer(e).TDCHdrCount())
			+ " header(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check the event trailer for the trailer count error flag
	if(getTrailer(e).TrailerCountErr()) {

		logger.logError(
			string("Trailer count error flag. Got ")
			+ to_string(getTrailer(e).TDCTlrCount())
			+ " trailer(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that all TDC headers have matching event IDs
	int lastTDCHeaderID = -1;
	for(const Signal &sig : e.WireSignals()) {

		if(sig.isTDCHeader()) {

			if(lastTDCHeaderID != -1) {

				if(sig.TDCHeaderEID() != lastTDCHeaderID) {

					logger.logError(
						string("TDC ")
						+ to_string(sig.TDC())
						+ " EventID mismatch! Current = "
						+ to_string(sig.TDCHeaderEID())
						+ ", previous = "
						+ to_string(lastTDCHeaderID),
						EVENT_ERROR,
						WARNING
					);

				}

			}

			lastTDCHeaderID = sig.TDCHeaderEID();

		}

	}

	int TDCHeaderCount  = 0;
	int TDCTrailerCount = 0;
	int TDCErrorCount   = 0;

	// Event::Signals() now filters out TDC headers/trailers/errors to ensure
	// backwards compatibility with other code that expects these signals not
	// to be included in the first place. Event::Signals_All() includes these
	// signals.

	// Count all TDC headers, trailers, and errors in the event.
	for(const Signal &sig : e.WireSignals()) {

		if(sig.isTDCHeader  ()) ++TDCHeaderCount ;
		if(sig.isTDCTrailer ()) ++TDCTrailerCount;
		if(sig.isTDCOverflow()) ++TDCErrorCount  ;

	}

	// Check that the real hit count matches the count reported in the 
	// event trailer
	if(getTrailer(e).HitCount() != e.WireSignals().size()) {

		logger.logError(
			string("Hit count in trailer = ")
			+ to_string(getTrailer(e).HitCount())
			+ ", real hit count = "
			+ to_string(e.WireSignals().size()),
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC header count matches the count reported in the
	// event trailer
	if(TDCHeaderCount != getTrailer(e).TDCHdrCount()) {

		logger.logError(
			to_string(TDCHeaderCount)
			+ " TDC header(s) found in data, event trailer indicates "
			+ to_string(getTrailer(e).TDCHdrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

	// Check that the real TDC trailer count matches the count reported in the
	// event trailer
	if(TDCTrailerCount != getTrailer(e).TDCTlrCount()) {

		logger.logError(
			to_string(TDCTrailerCount)
			+ " TDC trailer(s) found in data, event trailer indicates "
			+ to_string(getTrailer(e).TDCTlrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

}

Signal getHeader (const Event &e) {

	return e.TrigSignals().front();

}

Signal getTrailer(const Event &e) {

	return e.TrigSignals().back();

}