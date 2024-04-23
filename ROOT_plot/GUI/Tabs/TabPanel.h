/**
 * @file TabPanel.h
 *
 * @brief Wrapper for TGTab that better handles associations with menubar
 * options.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TGTab.h"
#include "TGLabel.h"

#include "../Components/Submenu.h"
#include "UITab.h"

/*
 * NOTE: New tabs should inherit from UITab.h. They can be added into the GUI
 *       by building them in the implementation of AttachToMenu(). See that
 *       implementation for more notes.
 */

class TabPanel : public TGTab {

public:

	/**
	 * Constructor.
	 * 
	 * @param width The width of constituent tab windows.
	 * 
	 * @param height The height of constituent tab windows.
	 */
	TabPanel(const TGWindow *p, int width, int height);

	/**
	 * Associates the tab panel with tabMenu. tabMenu will be populated with
	 * menu options that open and close tabs in the tab panel. 
	 * 
	 * REQUIRES: tabMenu is an empty Submenu.
	 */
	void AttachToMenu(Submenu *tabMenu);

	/**
	 * Returns true if a tab with the given title exists, and false otherwise.
	 */
	bool hasTab(const std::string &title);

	/**
	 * Internal use only. Signal handler.
	 */
	void handleCloseTab(int id);

	/**
	 * Internal use only.
	 */
	void makeConnections();

private:

	// A landing page opened when the program starts
	TGCompositeFrame *baseTab;

		TGLabel *baseLabel;

	// REQUIRES: A tab associated with 'title' does not already exist.
	void buildTab(const std::string &title, UITab *tab);

	int width;
	int height;

};