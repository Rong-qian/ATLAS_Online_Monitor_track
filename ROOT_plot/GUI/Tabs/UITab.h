/**
 * @file UITab.h
 *
 * @brief Abstract base class for GUI tab elements.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"

class UITab : public TGCompositeFrame {

public:

	UITab(const TGWindow *p);

	virtual ~UITab();

	virtual void update() = 0;

protected:

	// CONNECTIONS

	virtual void makeConnections ();
	virtual void breakConnections();

	int width;
	int height;

};