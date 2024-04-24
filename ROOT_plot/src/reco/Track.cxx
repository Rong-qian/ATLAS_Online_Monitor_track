#include "MuonReco/Track.h"

namespace MuonReco {
  Track::Track() {
    theta = 0; 
    y_int = 0;
    ori = false;
  }

  Track::Track(double _theta, double _y_int, bool _ori)
    : theta(_theta),
      y_int(_y_int),
      ori(_ori) {}

  double Track::Theta() {

    return theta;

  }
  
  double Track::YInt() {
    return y_int;
  }

  bool Track::Orientation() {
    return ori;
  }

  void Track::SetTheta(double _theta) {

    theta = _theta;

  }
  
  void Track::SetYInt(double _y_int) {
    y_int = _y_int;
  }

  void Track::SetOrientation(bool _ori) {
    ori = _ori;
  }

  double Track::Distance(double x, double y) {

    return TMath::Abs(x * TMath::Sin(theta) + (y_int - y) * TMath::Cos(theta));
  }

  void Track::Draw() {    
    return;
  }
}

