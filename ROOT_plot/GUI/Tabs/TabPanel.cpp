#include "TabPanel.h"

// TODO: Try to get rid of this dependency?
#include "DAQMonitor/DataModel/DAQData.h"

#include "Displays/ErrorView.h"
#include "Displays/GraphPlotter.h"
#include "Displays/HistogramPlotter.h"
#include "Displays/EventDisplay.h"
#include "Displays/ResidualsDisplay.h"
#include "Displays/EfficiencyDisplay.h"

using namespace std;

// Add new tabs here
void TabPanel::AttachToMenu(Submenu *tabMenu) {

	DAQData &data = DAQData::getInstance();

	/*
     * To add a new tab, add an entry to the tab menu or one of its submenus.
     * AddEntry will add an option to the View menu or one of its submenus.
     * The option's name will be given by the first parameter to AddEntry,
     * and the behavior it should perform on selection will be given by the
     * second parameter.
     *
     * Pass a lambda into the second parameter. The lambda should return
     * nothing, and should accept a single integer argument -- this will be
     * the ID of the entry in ROOT's TGPopupMenu representation of
     * the menu. The lambda should check if the tab already exists with 
     * hasTab(). If it doesn't, construct a tab object as a subclass of 
     * UITab, then call buildTab("[Tab title]", [pointer to tab object]).
     *
     * Make sure that the tab titles passed to hasTab() and buildTab() match.
	 *
     * Keep in mind that UITabs are expected to clean themselves up when 
     * destructed. Override the teardown() function if need be, but make sure
     * the parent teardown() gets called. As long as that's done, the TabPanel
     * will delete tabs automatically when they're closed.
     *
     * After the tab is constructed and no matter whether the tab exists or
     * not, call switchTab("[Tab title]") at the end.
	 * 
	 * And that's it!
     */

	///////////////////////////////////////////////////////////////////////////
	// ADC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *adcMenu = tabMenu->AddSubmenu("ADC Plots");

		adcMenu->AddEntry("ADC Overview", [this, &data](int id) {

			if(!hasTab("ADC Overview")) {

				vector<TH1*> plotList(
					data.plots.p_tdc_adc_time.begin(),
					data.plots.p_tdc_adc_time.end()
				);

				HistogramPlotter *adcOverview = new HistogramPlotter(
					this,
					plotList,
					"ADC Plots",
					width,
					height,
					4
				);

				buildTab("ADC Overview", adcOverview);

			}

			SetTab("ADC Overview");

		});

		adcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_adc_time.size(); ++i) {

			adcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[this, &data, i](int id) {

					string plotTitle = string("TDC ") 
						+ to_string(i) 
						+ string(" ADC Channels");

					if(!hasTab(plotTitle.data())) {

						vector<TH1*> plotList(
							data.plots.p_adc_time[i].begin(),
							data.plots.p_adc_time[i].end()
						);

						HistogramPlotter *adcChannelPlot = new HistogramPlotter(
							this,
							plotList,
							plotTitle,
							width,
							height,
							4
						);

						buildTab(plotTitle, adcChannelPlot);

					}

					SetTab(plotTitle.data());

				}
			);

		}

	///////////////////////////////////////////////////////////////////////////
	// TDC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *tdcMenu = tabMenu->AddSubmenu("TDC Plots");

		tdcMenu->AddEntry("TDC Overview", [this, &data](int id) {

			if(!hasTab("TDC Overview")) {

				vector<TH1*> plotList(
					data.plots.p_tdc_tdc_time_corrected.begin(),
					data.plots.p_tdc_tdc_time_corrected.end()
				);

				HistogramPlotter *tdcOverview = new HistogramPlotter(
					this,
					plotList,
					"TDC Plots",
					width,
					height,
					4
				);

				buildTab("TDC Overview", tdcOverview);

			}

			SetTab("TDC Overview");

		});

		tdcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_tdc_time_corrected.size(); ++i) {

			tdcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[this, &data, i](int id) {

					string plotTitle = string("TDC ") 
						+ to_string(i)
						+ string(" TDC Channels");

					if(!hasTab(plotTitle.data())) {

						vector<TH1*> plotList(
							data.plots.p_tdc_time_corrected[i].begin(),
							data.plots.p_tdc_time_corrected[i].end()
						);

						HistogramPlotter *tdcChannelPlot = new HistogramPlotter(
							this,
							plotList,
							plotTitle,
							width,
							height,
							4
						);

						buildTab(plotTitle, tdcChannelPlot);

					}

					SetTab(plotTitle.data());

				}
			);

		}

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Event Displays
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Residuals", [this](int id) {

		if(!hasTab("Residuals")) {

			ResidualsDisplay *residuals = new ResidualsDisplay(
				this, 
				width, 
				height
			);

			buildTab("Residuals", residuals);

		}

		SetTab("Residuals");

	});

	tabMenu->AddEntry("Efficiency", [this](int id) {

		if(!hasTab("Efficiency")) {

			EfficiencyDisplay *efficiency = new EfficiencyDisplay(
				this, 
				width, 
				height
			);

			buildTab("Efficiency", efficiency);

		}

		SetTab("Efficiency");

	});

	tabMenu->AddEntry("Event Display", [this](int id) {

		if(!hasTab("Event Display")) {

			EventDisplay *eventDisplay = new EventDisplay(this, width, height);

			buildTab("Event Display", eventDisplay);

		}

		SetTab("Event Display");

	});

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Noise Rate Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Noise Rate", [this, &data](int id) {

		if(!hasTab("Noise Rate")) {

			GraphPlotter *noiseDisplay = new GraphPlotter(
				this,
				data.plots.p_tdc_hit_rate_graph,
				"Noise Rate Display",
				width,
				height,
				4
			);

			buildTab("Noise Rate", noiseDisplay);

		}

		SetTab("Noise Rate");

	});

	///////////////////////////////////////////////////////////////////////////
	// Error Log Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Error Log", [this](int id) {

		if(!hasTab("Error Log")) {

			ErrorView *errorViewer = new ErrorView(this, width, height);

			buildTab("Error Log", errorViewer);

		}

		SetTab("Error Log");

	});

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// "Close all" option
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Close All", [this](int id) {

		// We don't delete the first one -- that's the HOME tab.
		while(GetNumberOfTabs() > 1) {

			TGCompositeFrame *tab = GetTabContainer(1);

			RemoveTab(1, kFALSE);
			Layout();

			delete tab;

		}

	});

}

TabPanel::TabPanel(const TGWindow *p, int width, int height) 
	: TGTab(p), width(width), height(height) {

    // TODO: Base tab can be expanded into an info panel of some sort -- like a 
    //       readme or like the info screens that pop up sometimes talking about e.g.
    //       program title/version/usage
	baseTab = new TGCompositeFrame(this, width, 850, kFixedSize);
	AddTab("Home", baseTab);

		baseLabel = new TGLabel(baseTab, "Open tabs from the view menu");
		baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

	makeConnections();

}

void TabPanel::makeConnections() {

	Connect(
		"CloseTab(Int_t)", 
		"TabPanel", 
		this, 
		"handleCloseTab(int)"
	);

}

void TabPanel::buildTab(const string &label, UITab *tab) {

	AddTab(label.data(), tab);
	MapSubwindows();
	Layout();

	// Show the close button for the tab
	GetTabTab(label.data())->ShowClose();

}

void TabPanel::handleCloseTab(int id) {

	TGCompositeFrame *tab = GetTabContainer(id);

	RemoveTab(id, kFALSE);
	Layout();

	delete tab;

	// Switch to the tab just before the one we closed
	if(id != 0) SetTab(id - 1);

}

bool TabPanel::hasTab(const string &title) {

	return GetTabTab(title.data());

}