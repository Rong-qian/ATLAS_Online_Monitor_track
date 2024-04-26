#include "MuonReco/Track.h"

namespace MuonReco {
  Track::Track() {
    theta = 0; 
    x_int = 0;
    ori = false;
  }

  Track::Track(double _theta, double _x_int, bool _ori)
    : theta(_theta),
      x_int(_x_int),
      ori(_ori) {}

  double Track::Theta() {

    return theta;

  }
  
  double Track::XInt() {
    return x_int;
  }

  bool Track::Orientation() {
    return ori;
  }

  void Track::SetTheta(double _theta) {

    theta = _theta;

  }
  
  void Track::SetXInt(double _x_int) {
    x_int = _x_int;
  }

  void Track::SetOrientation(bool _ori) {
    ori = _ori;
  }

  double Track::Distance(double x, double y) {

    return TMath::Abs(x * TMath::Sin(theta) + (x_int - y) * TMath::Cos(theta));
  }

  void Track::Draw() {    
    return;
  }
}

