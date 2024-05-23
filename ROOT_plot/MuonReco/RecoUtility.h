#ifndef MUON_CHECKEVENT
#define MUON_CHECKEVENT

#include <iostream>

#include "MuonReco/Event.h"
#include "MuonReco/Geometry.h"
#include "MuonReco/Cluster.h"
#include "MuonReco/Hit.h"
#include "MuonReco/Signal.h"
#include "MuonReco/TimeCorrection.h"
#include "MuonReco/ParameterSet.h"

namespace MuonReco {

  const int PASTEVENTCHECK = true;
  const int NOTPASTEVENTCHECK = false;


  /*! \class RecoUtility RecoUtility.h "MuonReco/RecoUtility.h"
   * 
   * \brief Provide static class interface for high level reconstruction
   * tasks, including checking event quality and populating event
   * with high level reconstructed objects
   *
   * Cuts on data are configurable using the ConfigParser object.
   * This allows users to specify the number of hits and clusters
   * that must be present for an Event to be marked as passing cuts.
   * Only Events that pass cuts will be used by the Optimizer class
   *
   * \author Kevin Nelson
   *         kevin.nelson@cern.ch
   * \date   17 July 2020
   */
  class RecoUtility {
  public:
    RecoUtility();
    RecoUtility(ParameterSet ps);

    static const     int    ROLLOVER  = 131072;
    static constexpr double BINSIZE   = 0.78125;

    void Configure(ParameterSet ps);
    bool CheckEvent(Event e, int* status);
    void DoHitClustering(Event *e);
    int  DoHitFinding(Event *e, TimeCorrection* tc, Geometry& geo);
    int  DoHitFindingPhase1(Event *e, TimeCorrection* tc, Geometry& geo);
    int  DoHitFindingPhase2(Event *e, TimeCorrection* tc, Geometry& geo, double adc_cut, int relative);
    int  rollover_bindiff_cal(int a, int b, int rollover);
    bool IsPhase2Data();
    void ResetDefaultChecking();
  private:
    bool    CHECK_TRIGGERS;
    bool    IS_PHASE2_DATA;
    int     WIDTHSEL;
    double  ADC_NOISE_CUT; // in ns
    bool    IS_RELATIVE_DATA;
    int     MIN_HITS_NUMBER;     // min total number of event hits
    int     MAX_HITS_NUMBER;     // max total number of event hits
    double  MAX_TIME_DIFFERENCE; // in ns

    int     MIN_CLUSTER_SIZE;
    int     MAX_CLUSTER_SIZE;

    int     MIN_CLUSTERS_PER_ML;
    int     MAX_CLUSTERS_PER_ML;

    double  TRIGGER_OFFSET;

    int     SIG_VOLTAGE_INVERT;
    int     TRG_VOLTAGE_INVERT;
  };
}

#endif
