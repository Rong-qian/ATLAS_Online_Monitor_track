#include "MuonReco/TrackParam.h"

namespace MuonReco {

  TrackParam::TrackParam() : AbstractTrackParam(), Parameterization(3) {
    param[THETA]      = 0;
    param[INTERCEPT]  = 1;
    param[DELTAT0]    = 0;
    initialAngle.push_back(0);
  }

  TrackParam::~TrackParam() {
  }
  
  void TrackParam::SetRT(Callable* rtp) {
    rtfunction = rtp;
  }

  double TrackParam::deltaT0() {
    return param[TrackParam::DELTAT0];
  }

  std::vector<double> TrackParam::getVerticalAngle() {
    std::vector<double> angle;
    angle.push_back(param[THETA]);
    return angle;
  }

  std::vector<double> TrackParam::getImpactParameter() {
    std::vector<double> impact;    
    impact.push_back(param[INTERCEPT]);
    return impact;
  }

  void TrackParam::RemoveSFs() {
    if (this->size() > NPARS)
      param.erase(param.begin()+NPARS, param.end());
  }

  bool TrackParam::IsRight(Hit h) {
    return (h.X() > param[INTERCEPT]-TMath::Tan(param[THETA])*h.Y());
  }

  double TrackParam::D(int index, Hit h) {
    double hitX = h.X();
    double hitY = h.Y();
    double c = TMath::Cos(param[THETA]);
    double s = TMath::Sin(param[THETA]);
    double sign = (c*(hitX-param[INTERCEPT]) + hitY*s > 0) ? 1.0 : -1.0;

    if (index == THETA) {
      return sign*(-s*(hitX-param[INTERCEPT]) + hitY*c);
    }
    else if (index == INTERCEPT) {
      return -sign*c;
    }
    else if (index == DELTAT0) {
      /*
      if (param[THETA]>0)
	return (rtfunction->Eval(h, param[DELTAT0]+1)-rtfunction->Eval(h, param[DELTAT0]));
	else */
      return (rtfunction->Eval(h, param[DELTAT0])-rtfunction->Eval(h, param[DELTAT0]+1));
    }
    else if (index == SLEWFACTOR) {
      return 0;
    }
    else std::cout << "Attempting to access invalid index" << std::endl;
    return 0;
  }


  double TrackParam::Residual(Hit h) {
    double hitX = h.X();
    double hitY = h.Y();

    if (this->size() <= SLEWFACTOR)
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0]));
    else if (this->size() <= SIGPROPFACTOR) {
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0], param[SLEWFACTOR]));    
    }
    else {
      return (Distance(h) - rtfunction->Eval(h, param[DELTAT0], param[SLEWFACTOR], 
					     param[SIGPROPFACTOR]));
    }
  }


  double TrackParam::Distance(Hit h) {
    double hitX = h.X();
    double hitY= h.Y();
    double c = TMath::Cos(param[THETA]);
    double s = TMath::Sin(param[THETA]);
    return TMath::Abs(c*(hitX-param[INTERCEPT]) + hitY*s);
    // return TMath::Abs(s*(hitX) + (param[INTERCEPT] - hitY)*c);
  }


  void TrackParam::Initialize(Event* e) {
    // create lists of doubles to hold x, y, radius
    std::vector<double> x, y, r;
    double hitX, hitY;
    int npts = 0;
    for (Cluster c : e->Clusters()) {
      for (Hit h : c.Hits()) {
	hitX = h.X();
	hitY = h.Y();

	x.push_back(hitX);
	y.push_back(hitY);
	r.push_back(rtfunction->Eval(h));
	npts++;
      }      
    }

    // keep track of n pts
    // iterate over 2^n tries, doing a fit each time
    double bestChiSq = DBL_MAX;
    double chiSq;
    for (int bitmap = 0; bitmap < 1<<npts; bitmap++) {
      // declare containers
      std::vector<double> xtrial = std::vector<double>(x.size());
      for (int i = 0; i < npts; i++) {
	if (bitmap & 1<<i) {
	  // here the ith hit should be R
	  xtrial[i] = x[i] + r[i];
	}
	else {
	  // here the th hit should be L
	  xtrial[i] = x[i] - r[i];
	}
      }
      // do fitting
      double fitSlope, fitInt;
      chiSq = LeastSquares(xtrial, y, r, &fitSlope, &fitInt);
      if (chiSq < bestChiSq) {
	bestChiSq        = chiSq;
	param[THETA]     = -1.0*TMath::ATan(1.0/fitSlope);
	param[INTERCEPT] = -1.0*fitInt/fitSlope;
      }
    }
    param[DELTAT0] = 0;
    initialAngle[0] = getVerticalAngle()[0];    
    Print();
  }

  double TrackParam::LeastSquares(std::vector<double> x, std::vector<double> y, std::vector<double> r, double* slopeOut, double* intOut) {
    double xmean = 0.0;
    double ymean = 0.0;
    for (int i = 0; i < x.size(); i++) {
      xmean += x.at(i);
      ymean += y.at(i);
    }
    xmean /= x.size();
    ymean /= y.size();
    
    double cov = 0.0;
    double var = 0.0;
    for (int i = 0; i < x.size(); i++) {
      cov += (x[i]-xmean)*(y[i]-ymean);
      var += (x[i]-xmean)*(x[i]-xmean);
    }

    *slopeOut = cov/var;
    *intOut   = ymean - (*slopeOut)*xmean;
    
    double chiSq = 0.0;
    double dist;
    for (int i = 0; i < x.size(); i++) {
      dist   = TMath::Abs(x[i]*(*slopeOut) + (*intOut) - y[i])/TMath::Sqrt((*slopeOut)*(*slopeOut) + 1);
      chiSq += TMath::Power((dist)/(Hit::RadiusError(r[i])), 2);
    }
    return chiSq;
  }

  double TrackParam::theta() {

    return param[THETA];

  }

  double TrackParam::slope() {

    return TMath::Tan(param[THETA]);

  }

  double TrackParam::y_int() {

    return param[TrackParam::INTERCEPT];

  }

  double TrackParam::LegendreUpperCurve(double theta, double x_0, double y_0, double r_0) {
    return x_0 * cos(theta) + y_0 * sin(theta) + r_0;
  }

  double TrackParam::LegendreLowerCurve(double theta, double x_0, double y_0, double r_0) {
    return x_0 * cos(theta) + y_0 * sin(theta) - r_0;
  }

  void TrackParam::Print() {
    std::cout << "Track Parameterization Object"   << std::endl;
    std::cout << "Theta:       " << param[THETA]     << std::endl;
    std::cout << "X Intercept: " << param[INTERCEPT] << std::endl;
    std::cout << "Delta T0:    " << param[DELTAT0]   << std::endl;
    std::cout << std::endl;
  }

  std::vector<Track> TrackParam::makeTracks() {
    double slope = -1.0*TMath::Tan(TMath::Pi()/2 + param[THETA]);
    double y_int = slope*param[INTERCEPT];
    std::vector<Track> tracks;
    tracks.push_back(Track(slope, y_int));
    return tracks;
  }
 
  void TrackParam::tracksystematics(TString systname, int * systindex, double * systerror, double * maxshift, TString & rrsystname, double * systsf, Bool_t floatup) {
    if (!systname.CompareTo("t0")) {
      *systindex = TrackParam::DELTAT0;
      *systerror = 10.0;
      *maxshift = (floatup) ? 4 : -4;
      rrsystname = "#Delta t_{0} [ns]";
    }
    if (!systname.CompareTo("slope")) {
      *systindex = TrackParam::THETA;
      *systerror = 0.005;
      *maxshift = (floatup) ? 1.5 : -1.5;
      rrsystname = "#theta [mrad]";
      *systsf   = 1000;
    }
    if (!systname.CompareTo("intercept")) {
      *systindex = TrackParam::INTERCEPT;
      *systerror = 0.1;
      *maxshift = (floatup) ? 0.1 : -0.1;
      rrsystname = "impact parameter [mm]";
    }
    if (!systname.CompareTo("slew")) {
      *systindex = TrackParam::SLEWFACTOR;
      *systerror = 0.25;
    }
    if (!systname.CompareTo("sigProp")) {
      *systindex = TrackParam::SIGPROPFACTOR;
      *systerror = 1.0;
    }
  }
}
