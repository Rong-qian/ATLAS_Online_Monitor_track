/**
 * @file TabSelector.cpp
 *
 * @brief Popup menu providing UI for selecting DAQ monitor tabs.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

using namespace std;

class TabSignalBus {

	RQ_OBJECT("TabSignalBus");

public:

	TabSignalBus();

    TabSignalBus  (      TabSignalBus &other) = delete;
    void operator=(const TabSignalBus &other) = delete;

    void onSelected  (const char *selection); // *SIGNAL*

};

class TabSelector : public TGPopupMenu {

public:

	TabSelector(const TGWindow *p);

	void handleActivate(int id);

	TabSignalBus &getSignalBus();

	virtual void AddPopup(
		const char *s, 
		TGPopupMenu *popup, 
		TGMenuEntry *before = nullptr, 
		const TGPicture *p = nullptr
	) override {

		TGPopupMenu::AddPopup(s, popup, before, p);

		submenus.push_back(popup);

	}

private:

	// SUBVIEWS

	vector<TGPopupMenu*> submenus;

	// VIEW

	TGPopupMenu *adcPlotSelector;

	TGPopupMenu *tdcPlotSelector;

	// SIGNALS

	TabSignalBus signals;

	// CONNECTIONS 

	void makeConnections();

};

TabSelector::TabSelector(const TGWindow *p) : TGPopupMenu(p) {

	submenus.push_back(this);

	makeConnections();

}

void TabSelector::makeConnections() {

	Connect("Activated(Int_t)", "TabSelector", this, "handleActivate(int)");

}

void TabSelector::handleActivate(int id) {

	// We want to unify the handling for activation of entries in this menu or
	// in any submenu. So we override AddPopup to store the new popup in a 
	// vector of submenus, and here we just scan all the submenus for the
	// id of the activated entry.
	for(TGPopupMenu *menu : submenus) {

		TGMenuEntry *entry = menu->GetEntry(id);

		if(entry) {

			signals.onSelected(entry->GetName());

			return;

		}

	}

	throw invalid_argument("TabSelector::handleActivate() received nonexistent entry ID.");

}

TabSignalBus &TabSelector::getSignalBus() { return signals; }

TabSignalBus::TabSignalBus() {}

void TabSignalBus::onSelected(const char *selection) {

	Emit("onSelected(const char*)", selection);

}