/**
 * @file GraphPlotter.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <vector>

#include "TGraph.h"

#include "PlotWindow.h"

class GraphPlotter : public PlotWindow {

public:

	GraphPlotter(
		const TGWindow *p, 
		std::vector<TGraph*> graphs, 
		const std::string &title = "", 
		int w = 1, 
		int h = 1,
		int rows = 1
	);

	virtual ~GraphPlotter();

	virtual void update() override;

private:

	// DATA

	std::vector<TGraph*> graphs;

};