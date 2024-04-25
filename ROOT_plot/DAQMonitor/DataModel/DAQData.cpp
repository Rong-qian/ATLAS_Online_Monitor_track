#include "DAQData.h"

using namespace std;
using namespace MuonReco;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Consider implementing copy constructor to handle geo better.

DAQData::DAQData() : plots(geo, rtp) {}

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

    totalEventCount = 0;
    packetCount     = 0;

    lostPackets     = 0;

    droppedSignals  = 0;
    droppedEvents   = 0;

}

bool DAQData::isPopulated() const {

    return totalEventCount > 0;

}