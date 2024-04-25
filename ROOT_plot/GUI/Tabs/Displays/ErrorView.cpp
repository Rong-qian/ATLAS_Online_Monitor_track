#include "ErrorView.h"

#include <string>

#include "GUI/Core/UISignals.h"

#include "Logging/ErrorLogger.h"

using namespace std;

// TODO: Make sure scrolling doesn't get reset every time it updates

// TODO: By default, scroll from bottom

// TODO: Only update when there's new data

const string DEFAULT_COLOR = "black"  ;
const string DEBUG_COLOR   = "black"  ;
const string INFO_COLOR    = "black"  ;
const string WARNING_COLOR = "orange" ;
const string ERROR_COLOR   = "red"    ;
const string FATAL_COLOR   = "#880808";

string getColor(const ErrorData &error);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ErrorView::ErrorView(const TGWindow *p, int width, int height) : UITab(p) {

	errorPanel = new TGHtml(this, width, height);
	AddFrame(errorPanel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	SetWindowName("Error Log");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();

}

ErrorView::~ErrorView() {

}

void ErrorView::update() {

	// TODO: Allow filtering by error level (use checkbuttons for this)

	// TODO: Clean up error logging system

	// TODO: Expand to be a "ConsoleView" module that displays console and
	//       debug info

	vector<ErrorData> errors = ErrorLogger::getInstance().getErrors();

	string htmlString = "";
	for(const ErrorData &error : errors) {

		htmlString += "<p>";

			htmlString += "<font color=";
			htmlString += getColor(error);
			htmlString += ">";

				htmlString += error.stringify();

			htmlString += "</font>";

		htmlString += "</p>";

	}

	if(errors.empty()) {

		htmlString = "<p>Nothing to display!</p>";

	}

	char *htmlText = new char[htmlString.size() + 1];

	try {

		strcpy(htmlText, htmlString.data());

		errorPanel->Clear();

		errorPanel->ParseText(htmlText);

		errorPanel->Layout();

		// TODO: Override this if user manually scrolls, and put it back if
		//       user scrolls back to bottom
		// NOTE: 20 pixels per line, with a flat 110 pixel adjustment.
		// TODO: Where does the -110 come from? It's not the window height.
		// errorPanel->ScrollUp(20 * errors.size() - 110);

	} catch(exception &e) {

		delete[] htmlText;
		htmlText = nullptr;

		throw e;

	}

	delete[] htmlText;
	htmlText = nullptr;

	/*
	cout << errorPanel->GetScrollPosition().fX << endl;
	cout << errorPanel->GetScrollPosition().fY << endl;
	*/

}

string getColor(const ErrorData &error) {

	switch(error.level) {

		case DEBUG:
			return DEBUG_COLOR;
			break;

		case INFORMATIVE:
			return INFO_COLOR;
			break;

		case WARNING:
			return WARNING_COLOR;
			break;

		case ERROR:
			return ERROR_COLOR;
			break;

		case FATAL:
			return FATAL_COLOR;
			break;

		default:
			return DEFAULT_COLOR;

	}

}