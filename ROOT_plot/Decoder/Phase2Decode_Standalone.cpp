/**
 * @file Phase2Decode_Standalone.cpp
 *
 * @brief Provides a standalone application for decoding raw data into events.
 * Adapted from DecodeRawData found in the sMDT reco library.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Optarg, e.g. "--help"

#include <iostream>
#include <fstream>

#include "TApplication.h"
#include "TUnixSystem.h"
#include "TString.h"
#include "TROOT.h"

#include "src/Decoder.h"

#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/RecoUtility.h"
#include "MuonReco/ArgParser.h"
#include "MuonReco/ConfigParser.h"
#include "MuonReco/IOUtility.h"

using namespace std;
using namespace MuonReco;

const size_t CONSOLE_LINE_LENGTH = 80;

void printUsage  ();
bool argsAreValid(ArgParser &ap);

int     getRunN    (ArgParser &ap, ConfigParser &cp);
TString getFilename(ArgParser &ap, ConfigParser &cp);

size_t getFileSize(ifstream &in);
size_t getRemainingFileLength(ifstream &in);

void displayProgressBar(double current, double total);

int main(int argc, char **argv) {

    // NOTE: This appears to populate the global gApplication and gSystem 
    //       variables.
    TApplication app("p2Decode", nullptr, nullptr);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	ArgParser ap = ArgParser(argc, argv);

	if(ap.hasKey("--help")) {

		printUsage();
		return 0;

	}

	if(!argsAreValid(ap)) {

		printUsage();
		return 1;

	}

	ConfigParser cp = ConfigParser(ap.getTString("--conf"));

	int runN = getRunN(ap, cp);

	TString filename = getFilename(ap, cp);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	// Set batch mode to improve the speed.
	gROOT->SetBatch(kTRUE);

	ifstream in(IOUtility::getRawInputFilePath(filename).Data());
	if(!in.is_open()) {

		cerr << "Couldn't open input file: " << filename << endl;

		return 1;

	}

	size_t fileSize = getFileSize(in);

	// FIXME: We need to initialize geometry properly.
	Geometry geo;
	geo.Configure(cp.items("Geometry"));

	TimeCorrection tc(cp);
	RecoUtility recoUtil(cp.items("RecoUtility"));

	Decoder decoder(1000);

	DecodeData aggregateData;

	cout << "Decoding data from input file: " << filename << endl;

	while(hasNewData(in)) {

		DecodeData loopData = decoder.decodeStream(in, geo, tc, recoUtil);

		aggregateData.eventCount     += loopData.eventCount;
		aggregateData.droppedSignals += loopData.droppedSignals;
		aggregateData.droppedEvents  += loopData.droppedEvents;

		aggregateData.nonemptyEvents.insert(
			aggregateData.nonemptyEvents.end(), 
			loopData.nonemptyEvents.cbegin  (), 
			loopData.nonemptyEvents.cend    ()
		);

		displayProgressBar(in.tellg(), fileSize);

		// cout << "\rProcessed " << aggregateData.eventCount << " events" << flush;

	}

	cout << endl;

	// TODO: Store the events

	cout << "Finished decoding" << endl;

	cout << "Processed " << aggregateData.eventCount            << " events"          << endl;
	cout << "Found "     << aggregateData.nonemptyEvents.size() << " nonempty events" << endl;
	cout << "Dropped "   << aggregateData.droppedSignals        << " signals"         << endl;
	cout << "Dropped "   << aggregateData.droppedEvents         << " events"          << endl;

	return 0;

}

bool argsAreValid(ArgParser &ap) {

	if(!ap.hasKey("--conf")) {

		cerr << "ERROR: Arguments must contain the '--conf' key." << endl;

		return false;

	}

	return true;

}

int getRunN(ArgParser &ap, ConfigParser &cp) {

	if(ap.hasKey("--runN")) return ap.getInt("--runN");
		
	return cp.items("General").getInt("RunNumber");

}

TString getFilename(ArgParser &ap, ConfigParser &cp) {

	if(ap.hasKey("--inputFile")) return ap.getTString("--inputFile");

	return cp.items("General").getStr("RawFileName");

}

size_t getFileSize(ifstream &in) {

	streampos pos = in.tellg();

	in.seekg(0, in.end);
	size_t size = static_cast<size_t>(in.tellg());

	in.clear();
	in.seekg(pos, in.beg);

	return size;

}

size_t getRemainingFileLength(ifstream &in) {

	size_t fileSize = getFileSize(in);

	return fileSize - static_cast<size_t>(in.tellg());

}

void displayProgressBar(double current, double total) {

	// Total number of progress steps is console line length minus the start
	// and end chars of the progress bar

	int progress = static_cast<int>(
		(CONSOLE_LINE_LENGTH - 2) * current / total
	);

	cout << "\r<";

	for(int i = 0; i < progress; ++i) {

		cout << "=";

	}

	for(int i = 0; i < (CONSOLE_LINE_LENGTH - 2) - progress; ++i) {

		cout << " ";

	}

	cout << ">" << flush;

}

void printUsage() {

	cout << "Usage:"                                               << endl
	     << "p2Decode  --conf CONFIG"                              << endl
	     << "         [--inputFile FILENAME]"                      << endl
	     << "         [--runN RUNNUMBER]"                          << endl
         << "Note:"                                                << endl 
         << "RUNUMBER and FILENAME will override their definition" << endl 
         << "in the config file.  "                                << endl 
         << "FILENAME is the name of the file, not a path. "       << endl
         << "The path is automatically resolved."                  << endl;

}
