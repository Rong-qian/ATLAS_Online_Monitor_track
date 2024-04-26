#include "MuonReco/Track.h"

namespace MuonReco {
  Track::Track() {
    slope = 0; 
    y_int = 0;
    ori = false;
  }
  
  Track::Track(double _slope, double _y_int, bool _ori) {
    slope = _slope;
    y_int = _y_int;
    ori = _ori;
  }
 
  double Track::Slope() {
    return slope;
  }
  
  double Track::YInt() {
    return y_int;
  }

  bool Track::Orientation() {
    return ori;
  }

  void Track::SetSlope(double _slope) {
    slope = _slope;
  }
  
  void Track::SetYInt(double _y_int) {
    y_int = _y_int;
  }

  void Track::SetOrientation(bool _ori) {
    ori = _ori;
  }

  double Track::Distance(double x, double y) {
    return TMath::Abs(x*slope + y_int - y)/TMath::Sqrt(slope*slope + 1);
  }

  void Track::Draw() {    
    return;
  }
}

