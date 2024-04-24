/**
 * @file Plots.h
 *
 * @brief Data structure containing histograms for ATLAS cosmic ray testing.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>

#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>

#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/RTParam.h"

// NOTE: There should really only be one of those. DAQData holds it.
struct Plots {

	Plots(MuonReco::Geometry &geo, MuonReco::RTParam &rtp);

	Plots         (const Plots &other);
	void operator=(const Plots &other) = delete; // TODO: It's a little bit
	                                             //       weird to have a
	                                             //       copy constructor but
	                                             //       not copy assignment

	TH1F *                           p_leading_time          ;
	TH1F *                           p_trailing_time         ;

	std::vector<TH1F*>               p_hits_distribution     ;

	std::vector<std::vector<TH1F*>>  p_tdc_time              ;
	std::vector<std::vector<TH1F*>>  p_tdc_time_original     ;
	std::vector<std::vector<TH1F*>>  p_tdc_time_corrected    ;
	std::vector<std::vector<TH1F*>>  p_tdc_time_selected     ;
	std::vector<std::vector<TH1F*>>  p_adc_time              ;


	std::vector<TH1F*>               p_tdc_tdc_time_original ;
	std::vector<TH1F*>               p_tdc_tdc_time_corrected;
	std::vector<TH1F*>               p_tdc_tdc_time_selected ;
	std::vector<TH1F*>               p_tdc_adc_time          ;
	std::vector<TH1F*>               p_tdc_channel           ;

	std::vector<TH2F*>               p_adc_vs_tdc            ;

	std::vector<std::vector<double>> p_tdc_hit_rate          ;
	std::vector<TGraph*>             p_tdc_hit_rate_graph    ;

	TH2D *                           hitByLC                 ;
	TH2D *                           badHitByLC              ;
	TH2D *                           goodHitByLC             ; 
	TH2D *                           tube_efficiency         ;

	TH1D *                           residuals               ;

	std::vector<std::vector<double>> nHits                   ;
	std::vector<std::vector<double>> nTotal                  ;

	// TODO: This would ideally be in DAQData.
	std::vector<Event*>              eventDisplayBuffer      ;

	void binEvent     (MuonReco::Event &e  );
	void updateHitRate(int total_events    );
	void clear        (                    );

private:

	// NOTE: This is configured externally in DataRunOperations. No need to do
	//       anything with it here. It's just here to be accessible.
	MuonReco::Geometry &geo; 
	MuonReco::RTParam  &rtp;

};