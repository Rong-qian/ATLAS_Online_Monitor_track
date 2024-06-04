/**
 * @file DAQData.h
 *
 * @brief Singleton storage for data retrieved from the DAQ. 
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * CAUTION: DAQData is not innately threadsafe, and must be locked
 * before modification to ensure thread safety. Locking and unlocking
 * functions are provided.
 */

#pragma once

#include <vector>
#include <mutex>
#include <deque>

#include "Plots.h"

#include "src/EventDisplay.h"

#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/RecoUtility.h"
#include "MuonReco/RTParam.h"

struct DAQData {

    // A vector of the fully processed and populated events decoded in the
    // last decode iteration, with hit finding already performed.
    // Excludes empty events.
    std::vector<MuonReco::Event> newEvents;

    //std::vector<MuonReco::Event> eventDisplayBuffer;
    
    std::deque<MuonReco::Event> eventDisplayBuffer;
    
    // A struct of histograms containing aggregate event data. See
    // src/DataModel/Plots.cpp.
    Plots plots;

    MuonReco::Geometry       geo     ;
    MuonReco::TimeCorrection tc      ;
    MuonReco::RecoUtility    recoUtil;
    MuonReco::RTParam        rtp     ;
    MuonReco::RecoUtility    ru      ;
    MuonReco::EventDisplay   eventDisplay;

    int totalEventCount    = 0;
    int nonemptyEventCount = 0;
    int packetCount        = 0;

    int lostPackets     = 0;

    int droppedSignals  = 0;
    int droppedEvents   = 0;
    
    int max_display_event = 10000;
    int num_display_event = 0;
    
    /**
     * Checks if the DAQData has any events in it at all.
     * Useful for making empty checks before expecting
     * DAQData members to be initialized.
     * 
     * E.g. DAQData::geo is sure to be initialized before any events are
     * processed, so if isPopulated() is true, we can expect DAQData::geo to be
     * safe to use.
     */
    bool isPopulated() const;

    void lock  () const; // Locks an internal mutex
    void unlock() const; // Unlocks an internal mutex

    void binEvent     (MuonReco::Event &e  );
    void updateHitRate(int total_events    );
    void Reload();
    /**
 *  For event check
    **/  

    bool pass_event_check;
    int status;

    /**
     * Clears all data stored in the DAQData. Clear is called at the
     * beginning of each run to clear any latent data from the last run.
     */
    void clear();

    // DAQData should not be copied
    DAQData       (      DAQData &other) = delete;
    void operator=(const DAQData &other) = delete;

    // Access the singleton DAQData instance by reference here.
    static DAQData &getInstance();

    std::string T0Path;
    std::string RTPath;
private:

    DAQData();

    mutable std::mutex dataLock;

    std::vector<std::vector<double>> nHits ;
    std::vector<std::vector<double>> nTotal;

};
