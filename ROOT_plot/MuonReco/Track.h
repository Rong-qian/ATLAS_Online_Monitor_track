#ifndef MUON_TRACK
#define MUON_TRACK

#include "TMath.h"

#include "MuonReco/RecoObject.h"

namespace MuonReco {
  class Track : public RecoObject {
  public:
    Track();
    Track(double _slope, double _y_int, bool _ori = false);

    double Slope   ();
    double YInt    ();
    bool Orientation();
    double Distance(double x, double y);
    void     SetSlope(double _slope);
    void     SetYInt (double _y_int);
    void     SetOrientation (bool _ori);

    void     Draw    () override;

  private:
    double slope, y_int;
    bool ori;
  };
}
#endif
