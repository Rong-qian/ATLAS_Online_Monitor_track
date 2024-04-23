/**
 * @file EventDecoding.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include "MuonReco/EventDecoding.h"
using namespace std;
namespace MuonReco {

  // TODO: This does three different things. Break it up.

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

  // TODO: Separate errors that we drop the event for and errors that we 
  //       report without dropping the event
  //         -- validateEventErrors and validateEventWarnings
  // TODO: Use exceptions?
  // Validation that, when failed, drops the event
  bool validateEventErrors(const Event &e) {

    // ErrorLogger &logger = ErrorLogger::getInstance();

    if(!e.TrigSignals().at(0).isEventHeader()) {

      // logger.logError(
      //   "ERROR -- Found event with no header",
      //   EVENT_ERROR,
      //   ERROR
      // );
      cout << "ERROR -- Found event with no header" <<endl;

      return false;

    }

    if(!e.TrigSignals().at(1).isEventTrailer()) {

      // logger.logError(
      //   "ERROR -- Found event with no trailer",
      //   EVENT_ERROR,
      //   ERROR
      // );

      cout << "ERROR -- Found event with no trailer" <<endl;

      return false;

    }

    if(e.TrigSignals().at(0).HeaderEID() != e.TrigSignals().at(1).TrailerEID()) {

      // logger.logError(
      //   "ERROR -- Found event with mismatched event IDs",
      //   EVENT_ERROR,
      //   ERROR
      // );
      cout << "ERROR -- Found event with mismatched event IDs" <<endl;

    }

    for(const Signal &sig : e.WireSignals()) {

      if(sig.isEventHeader()) {

        // logger.logError(
        //   "ERROR -- Found event with multiple headers",
        //   EVENT_ERROR,
        //   ERROR
        // );
        cout << "ERROR -- Found event with multiple headers" <<endl;


        return false;

      }

      if(sig.isEventTrailer()) {

        // logger.logError(
        //   "ERROR -- Found event with multiple trailers",
        //   EVENT_ERROR,
        //   ERROR
        // );
        cout << "ERROR -- Found event with multiple trailers" <<endl;


        return false;

      }

    }

    return true;

  }

  // Validation that, when failed, keeps the event but warns the user.
  void validateEventWarnings(const Event &e) {

    // ErrorLogger &logger = ErrorLogger::getInstance();

    if(e.TrigSignals().at(1).HeaderCountErr()) {

      // logger.logError(
      //   string("WARNING -- Header count error flag. Got ")
      //   + to_string(e.TrigSignals().at(1).TDCHdrCount())
      //   + " header(s)!",
      //   EVENT_ERROR,
      //   WARNING
      // );
      cout << "WARNING -- Header count error flag. Got "<<to_string(e.TrigSignals().at(1).TDCHdrCount())
      << " header(s)!" << endl;

    }

    if(e.TrigSignals().at(1).TrailerCountErr()) {

      // logger.logError(
      //   string("WARNING -- Trailer count error flag. Got ")
      //   + e.TrigSignals().at(1).TDCTlrCount()
      //   + " trailer(s)!",
      //   EVENT_ERROR,
      //   WARNING
      // );
      cout << "WARNING -- Trailer count error flag. Got "<<to_string(e.TrigSignals().at(1).TDCHdrCount())
      << " header(s)!" << endl;

    }

    /*
    if(e.TrigSignals().at(1).HitCount() != e.Signals().size() + [error_word_count]) {

      logger.logError(
        string("WARNING -- Hit count in trailer = ")
        + to_string(e.TrigSignals().at(1).HitCount())
        + ", real hit count = "
        + to_string(e.Signals().size())
        + ", error word count = TODO: IMPLEMENT",
        EVENT_ERROR
      );

    }
    */

    int TDCHeaderCount  = 0;
    int TDCTrailerCount = 0;

    for(const Signal &sig : e.WireSignals()) {

      if(sig.isTDCHeader ()) ++TDCHeaderCount ;
      if(sig.isTDCTrailer()) ++TDCTrailerCount;

    }

    if(TDCHeaderCount != e.TrigSignals().at(1).TDCHdrCount()) {

      // logger.logError(
      //   string("WARNING -- ")
      //   + to_string(TDCHeaderCount)
      //   + " TDC header(s) found in data, event trailer indicates "
      //   + to_string(e.TrigSignals().at(1).TDCHdrCount())
      //   + "!",
      //   EVENT_ERROR,
      //   WARNING
      // );
      cout << "WARNING -- " << TDCHeaderCount << " TDC header(s) found in data, event trailer indicates "
      << e.TrigSignals().at(1).TDCHdrCount() << "!" <<endl;

    }

    if(TDCTrailerCount != e.TrigSignals().at(1).TDCTlrCount()) {

      // logger.logError(
      //   string("WARNING -- ")
      //   + to_string(TDCTrailerCount)
      //   + " TDC trailer(s) found in data, event trailer indicates "
      //   + to_string(e.TrigSignals().at(1).TDCTlrCount())
      //   + "!",
      //   EVENT_ERROR,
      //   WARNING
      // );
      cout << "WARNING -- " << TDCTrailerCount << " TDC trailer(s) found in data, event trailer indicates "
      << e.TrigSignals().at(1).TDCTlrCount() <<"!"<<endl;

    }

    // TODO: Validate trailer hit count against e.signals().size() + error_word_count
    //         -- make sure to exclude TDC headers and trailers

  }

  void processEvent(Event *e, TimeCorrection* tc, Geometry& geo) {

    // RecoUtility::DoHitFinding(e, tc, geo);
    // TODO: No hit clustering?

    e->SetPassCheck();
    e->CheckClusterTime();

  }
} // namespace MuonReco