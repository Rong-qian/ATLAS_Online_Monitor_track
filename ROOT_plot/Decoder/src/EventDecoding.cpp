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

#include "src/HitFinder.cpp"
#include "src/TimeCorrection.cpp"

using namespace std;
using namespace Muon;

const string EVENT_ERROR = "event" ;

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
		)
	);

}

// Validation that, when failed, drops the event
bool validateEventErrors(const Event &e) {

	ErrorLogger &logger = ErrorLogger::getInstance();

	if(!e.Header().isEventHeader()) {

		logger.logError(
			"ERROR -- Found event with no header",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	if(!e.Trailer().isEventTrailer()) {

		logger.logError(
			"ERROR -- Found event with no trailer",
			EVENT_ERROR,
			ERROR
		);

		return false;

	}

	if(e.Header().HeaderEID() != e.Trailer().TrailerEID()) {

		logger.logError(
			"ERROR -- Found event with mismatched event IDs",
			EVENT_ERROR,
			ERROR
		);

	}

	for(const Signal &sig : e.Signals()) {

		if(sig.isEventHeader()) {

			logger.logError(
				"ERROR -- Found event with multiple headers",
				EVENT_ERROR,
				ERROR
			);

			return false;

		}

		if(sig.isEventTrailer()) {

			logger.logError(
				"ERROR -- Found event with multiple trailers",
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

	if(e.Trailer().HeaderCountErr()) {

		logger.logError(
			string("WARNING -- Header count error flag. Got ")
			+ to_string(e.Trailer().TDCHdrCount())
			+ " header(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	if(e.Trailer().TrailerCountErr()) {

		logger.logError(
			string("WARNING -- Trailer count error flag. Got ")
			+ to_string(e.Trailer().TDCTlrCount())
			+ " trailer(s)!",
			EVENT_ERROR,
			WARNING
		);

	}

	/*
	if(e.Trailer().HitCount() != e.Signals().size() + [error_word_count]) {

		logger.logError(
			string("WARNING -- Hit count in trailer = ")
			+ to_string(e.Trailer().HitCount())
			+ ", real hit count = "
			+ to_string(e.Signals().size())
			+ ", error word count = TODO: IMPLEMENT",
			EVENT_ERROR
		);

	}
	*/

	int TDCHeaderCount  = 0;
	int TDCTrailerCount = 0;

	for(const Signal &sig : e.Signals()) {

		if(sig.isTDCHeader ()) ++TDCHeaderCount ;
		if(sig.isTDCTrailer()) ++TDCTrailerCount;

	}

	if(TDCHeaderCount != e.Trailer().TDCHdrCount()) {

		logger.logError(
			string("WARNING -- ")
			+ to_string(TDCHeaderCount)
			+ " TDC header(s) found in data, event trailer indicates "
			+ to_string(e.Trailer().TDCHdrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

	if(TDCTrailerCount != e.Trailer().TDCTlrCount()) {

		logger.logError(
			string("WARNING -- ")
			+ to_string(TDCTrailerCount)
			+ " TDC trailer(s) found in data, event trailer indicates "
			+ to_string(e.Trailer().TDCTlrCount())
			+ "!",
			EVENT_ERROR,
			WARNING
		);

	}

	// TODO: Validate trailer hit count against e.signals().size() + error_word_count
	//         -- make sure to exclude TDC headers and trailers

}

void processEvent(Event &e) {

	DoHitFinding(&e, *TimeCorrection::getInstance(), 0, 0);
	// TODO: No hit clustering?

	e.SetPassCheck(true);
	e.CheckClusterTime();

}