#include "RunView.h"

#include <string>

using namespace std;

// TODO: Examine this example:
//       https://root.cern/doc/master/calendar_8C.html

RunView::RunView(const TGWindow *p) 
	: TGGroupFrame(p, "Run Viewer", kVerticalFrame) {

	table = new TGHtml(this, 100, 220);
	AddFrame(table, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	Resize(320, 250);

}

void RunView::update(const UpdatePacket &packet) {

	statsTable.setEntry("Packets Recorded", to_string(packet.packetCount    ));
	statsTable.setEntry("Events Processed", to_string(packet.totalEventCount));
	statsTable.setEntry("Passing Events"  , to_string(packet.passEventCount ));
	statsTable.setEntry("Empty Events"    , to_string(packet.emptyEventCount));
	statsTable.setEntry("Error Count"     , to_string(packet.errorCount     ));
	statsTable.setEntry("Decoding Error"  , to_string(packet.decodeError    ));
	statsTable.setEntry("Warning Count"   , to_string(packet.warningCount   ));
	statsTable.setEntry("Packets Lost"    , to_string(packet.lostPackets    ));
	statsTable.setEntry("Dropped Signals" , to_string(packet.droppedSignals ));
	statsTable.setEntry("Dropped Events"  , to_string(packet.droppedEvents  ));

	string htmlString = statsTable.stringify();

	// Package the html into the html GUI element
	char *htmlText = new char[htmlString.size() + 1];
	try{

		strcpy(htmlText, htmlString.data());

		table->Clear();

		table->ParseText(htmlText);

		table->Layout();

	} catch (exception &e) {

		delete[] htmlText;
		htmlText = nullptr;

		throw e;

	}

	delete[] htmlText;
	htmlText = nullptr;

}