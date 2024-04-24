#ifndef MUON_TRACK_PARAMETERIZATION
#define MUON_TRACK_PARAMETERIZATION

#include <vector>
#include <iostream>

#include "TMath.h"

#include "MuonReco/Hit.h"
#include "MuonReco/Parameterization.h"
#include "MuonReco/Optimizer.h"
#include "MuonReco/Event.h"
#include "MuonReco/RTParam.h"
#include "MuonReco/Callable.h"
#include "MuonReco/AbstractTrackParam.h"
#include "MuonReco/Track.h"
#include "MuonReco/ResolutionResult.h"

namespace MuonReco {
  
  /*****************************************
   * Parameterization of track in 2D       *
   * in terms of intercept and slope       *
   *                                       *
   * Can be optimized by a Muon::Optimizer *
   * object                                *
   *                                       *
   * Author:        Kevin Nelson           *
   * Date:          May 31, 2019           *
   * Last Modified: May 31, 2019           *
   *                                       *
   *****************************************
   */
  class TrackParam :virtual public AbstractTrackParam, virtual public Parameterization {
  public:
    TrackParam();
    ~TrackParam();

    void                SetRT             (Callable* rtp)    override;
    void                Initialize        (Event *e)         override;
    void                Print             ()                 override;
    std::vector<Track>  makeTracks        ()                 override;
    double              D                 (int index, Hit h) override;
    double              Residual          (Hit h)            override;
    double              Distance          (Hit h)            override;
    bool                IsRight           (Hit h)            override;
    double              deltaT0           ()                 override;
    void                RemoveSFs         ()                 override;
    std::vector<double> getVerticalAngle  ()                 override;
    std::vector<double> getImpactParameter()                 override;

    double slope();
    double theta();
    double y_int();

    void tracksystematics(TString systname, int * systindex, double * systerror, double * maxshift, TString & rrsystname, double * systsf, Bool_t floatup) override;

    double LegendreLowerCurve(double theta, double x_0, double y_0, double r_0);
    double LegendreUpperCurve(double theta, double x_0, double y_0, double r_0);

    double   LeastSquares(std::vector<double> x, std::vector<double> y,  std::vector<double> r, double* slopeOut, double* intOut);

    static const int THETA         = 0;
    static const int INTERCEPT     = 1;
    static const int DELTAT0       = 2;
    static const int SLEWFACTOR    = 3;
    static const int SIGPROPFACTOR = 4;
    static const int NPARS         = 3;

  };

}
#endif
