#include "DAQData.h"

#include "MuonReco/TrackParam.h"
#include "MuonReco/Track.h"

using namespace std;
using namespace MuonReco;

// TODO: Is there a better place to put this? E.g. Geometry.cpp?
const double MATCH_WINDOW = 1.5; // us

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Consider implementing copy constructor to handle geo better.

DAQData::DAQData() {

    nHits .resize(Geometry::MAX_TUBE_LAYER);
    nTotal.resize(Geometry::MAX_TUBE_LAYER);

    for(size_t i = 0; i < Geometry::MAX_TUBE_LAYER; ++i) {

        nHits[i] .resize(Geometry::MAX_TUBE_COLUMN);
        nTotal[i].resize(Geometry::MAX_TUBE_COLUMN);

    }

}

DAQData &DAQData::getInstance() {

    static DAQData data;

    return data;

}

void DAQData::lock  () const { dataLock.lock  (); }
void DAQData::unlock() const { dataLock.unlock(); }

void DAQData::clear () {

    processedEvents.clear();
    plots.clear();
    eventDisplay.Clear();
    eventDisplayBuffer.clear();

    totalEventCount = 0;
    packetCount     = 0;

    lostPackets     = 0;

    droppedSignals  = 0;
    droppedEvents   = 0;

    for(vector<double> &vec : nHits) {

        vec.clear();
        vec.resize(Geometry::MAX_TUBE_COLUMN);

    }

    for(vector<double> &vec : nTotal) {

        vec.clear();
        vec.resize(Geometry::MAX_TUBE_COLUMN);

    }

}

bool DAQData::isPopulated() const {

    return totalEventCount > 0;

}
void DAQData::updateHitRate(int total_events) {

    for(int tdc = 0; tdc < Geometry::MAX_TDC; ++tdc) {

        for(int chnl = 0; chnl < Geometry::MAX_TDC_CHANNEL; ++chnl) {

            // Hits / total_events * (1 / MATCH_WINDOW (us)) * 1000 (us / ms)
            plots.p_tdc_hit_rate[tdc][chnl] 
                = plots.p_adc_time[tdc][chnl]->GetEntries() / total_events 
                  *
                  1000 / MATCH_WINDOW;

            plots.p_tdc_hit_rate_graph[tdc]->SetPoint(
                chnl, 
                chnl, 
                plots.p_tdc_hit_rate[tdc][chnl]
            );

            double tmp_yrange = plots.p_tdc_hit_rate_graph[tdc]->GetHistogram()->GetMaximum();
            plots.p_tdc_hit_rate_graph[tdc]->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);
            plots.p_tdc_hit_rate_graph[tdc]->GetHistogram()->SetMinimum(0);
            plots.p_tdc_hit_rate_graph[tdc]->GetXaxis()->SetLimits(-0.5, static_cast<double>(Geometry::MAX_TDC_CHANNEL) - 0.5);

        }

    }

}

void DAQData::binEvent(Event &e) {

    for(const Hit &hit : e.WireHits()) {

        plots.p_tdc_tdc_time_corrected[hit.TDC()]->Fill(hit.CorrTime());
        plots.p_tdc_adc_time          [hit.TDC()]->Fill(hit.ADCTime ());

        plots.p_tdc_time_corrected[hit.TDC()][hit.Channel()]->Fill(hit.CorrTime ());
        plots.p_tdc_time          [hit.TDC()][hit.Channel()]->Fill(hit.DriftTime());
        plots.p_adc_time          [hit.TDC()][hit.Channel()]->Fill(hit.ADCTime  ());

        double tmp_yrange = plots.p_tdc_hit_rate_graph[hit.TDC()]->GetHistogram()->GetMaximum();
        plots.p_tdc_hit_rate_graph[hit.TDC()]->GetHistogram()->SetMaximum(tmp_yrange > 0.5 ? tmp_yrange : 1);

        int hitL, hitC;
        geo.GetHitLayerColumn(hit.TDC(), hit.Channel(), &hitL, &hitC);

        plots.hitByLC->Fill(hitC, hitL);
        if(hit.CorrTime() < 0 || hit.CorrTime() > 400) { // TODO: Magic numbers

            plots.badHitByLC->Fill(hitC, hitL);
        
        } else {
        
            plots.goodHitByLC->Fill(hitC, hitL);
        
        }
        plots.p_hits_distribution[hitL]->Fill(hitC);

    }

    TrackParam tp;
    tp.SetRT(&rtp);
    tp.setVerbose(0);
    tp.setMaxResidual(1000000);

    // Residuals, efficiency, and event display modified from work by 
    // Rongqian Qian. See:
    // https://github.com/Rong-qian/ATLAS_Online_Monitor/
    if(e.Pass()) {

        // TODO: Shouldn't this object be persistent?
        //         -- It isn't in the legacy DAQ.cpp though
        TTree *optTree = new TTree("optTree", "optTree");

        optTree->Branch("event", "Event", &e);
        optTree->Fill();

        tp.setTarget(optTree);
        tp.setRangeSingle(0);
        tp.setIgnoreNone();
        tp.optimize();

        // Populate residuals
        for(Cluster &c : e.Clusters()) {

            for(Hit &hit : c.Hits()) {

                plots.residuals->Fill(tp.Residual(hit) * 1000.0);

            }

        }

        /*
        for(const Hit &hit : e.WireHits()) {

            int hit_layer;
            int hit_column;

            geo.GetHitLayerColumn(
                hit.TDC(), 
                hit.Channel(),
                &hit_layer,
                &hit_column
            );

            ++nTotal[hit_layer][hit_column];

            int iL, iC;
            double _hitX, _hitY;

            geo.GetHitLayerColumn(hit.TDC(), hit.Channel(), &iL, &iC);
            geo.GetHitXY(hit.TDC(), hit.Channel(), &_hitX, &_hitY);

            double trackDist = tp.Distance(Hit(
                0, 0, 0, 0, 0, 0, iL, iC, _hitX, _hitY
            ));

            if(trackDist <= Geometry::column_distance / 2) {

                ++nHits[hit_layer][hit_column];

            }

            tube_efficiency->SetBinContent(
                hit_layer  + 1,
                hit_column + 1,
                nHits[hit_layer][hit_column] / nTotal[hit_layer][hit_column]
            );

            cout << hit_layer << endl;
            cout << hit_column << endl;



            // need to check distance

        }
        */

        // Populate efficiency
        // Iterate through each tube via tdc and channel index
        for(int tdc_index = 0; tdc_index < Geometry::MAX_TDC; ++tdc_index) {

            for(int ch_index = 0; ch_index < Geometry::MAX_TDC_CHANNEL; ++ch_index) {

                // If the channel is active,
                if(geo.IsActiveTDCChannel(tdc_index, ch_index)) {

                    int iL, iC;
                    double _hitX, _hitY;

                    geo.GetHitLayerColumn(tdc_index, ch_index, &iL, &iC);
                    geo.GetHitXY(tdc_index, ch_index, &_hitX, &_hitY);

                    // get track x position and figure out what tube(s) it may go through
                    double trackDist = tp.Distance(Hit(
                        0, 0, 0, 0, 0, 0, iL, iC, _hitX, _hitY
                    ));

                    if(trackDist <= Geometry::column_distance / 2) {

                        bool tubeIsHit = false;

                        for(Hit hit : e.WireHits()) {

                            int hit_layer;
                            int hit_column;

                            geo.GetHitLayerColumn(
                                hit.TDC(), 
                                hit.Channel(),
                                &hit_layer,
                                &hit_column
                            );

                            // If this is true, the tube was hit
                            if(hit_layer == iL && hit_column == iC) {
                                    
                                tubeIsHit = true;

                            }

                        }

                        ++nTotal[iL][iC];

                        if(tubeIsHit) ++nHits[iL][iC];

                    }

                }

            }

        }

        for(int iL = 0; iL < Geometry::MAX_TUBE_LAYER; ++iL) {

            for(int iC = 0; iC < Geometry::MAX_TUBE_COLUMN; ++iC) {

                if(nHits.at(iL).at(iC) != 0) {

                    // FIXME: Make sure the axes are right
                    plots.tube_efficiency->SetBinContent(
                        iL + 1, 
                        iC + 1, 
                        nHits[iL][iC] / nTotal[iL][iC]
                    );

                }

            }

        }

        delete optTree;

        // TODO: It would be nice to put this in the display buffer in
        //       the decoder.
        vector<Track> eventTracks = tp.makeTracks();
        for(const Track &track : eventTracks) {

            e.AddTrack(track);

        }
        eventDisplayBuffer.push_back(e);

    }

}