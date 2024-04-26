#ifndef MUON_TRACK
#define MUON_TRACK

#include "TMath.h"

#include "MuonReco/RecoObject.h"

namespace MuonReco {
  class Track : public RecoObject {
  public:
    Track();
    Track(double _theta, double _x_int, bool _ori = false);

    double Theta   ();
    double XInt    ();
    bool Orientation();
    double Distance(double x, double y);
    void     SetTheta(double _theta);
    void     SetXInt (double _x_int);
    void     SetOrientation (bool _ori);

    void     Draw    () override;

  private:
    double theta, x_int;
    bool ori;
  };
}
#endif
