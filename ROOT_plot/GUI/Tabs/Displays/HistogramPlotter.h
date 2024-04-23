/**
 * @file HistogramPlotter.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "TH1.h"

#include "PlotWindow.h"

class HistogramPlotter : public PlotWindow {

public:

	HistogramPlotter(
		const TGWindow *p, 
		std::vector<TH1*> histograms, 
		const std::string &title, 
		int w, 
		int h,
		int rows
	);

	virtual ~HistogramPlotter();

	virtual void update() override;

private:

	// DATA

	std::vector<TH1*> histograms;

};