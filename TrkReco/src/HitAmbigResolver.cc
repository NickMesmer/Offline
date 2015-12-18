//
// class to resolve hit ambiguities one hit at a time, assuming a reasonable track
// fit as input
//
// $Id: HitAmbigResolver.cc,v 1.3 2012/08/31 22:39:00 brownd Exp $
// $Author: brownd $ 
// $Date: 2012/08/31 22:39:00 $
//
#include "TrkReco/inc/HitAmbigResolver.hh"
#include "Mu2eBTrk/inc/TrkStrawHit.hh"
#include "BTrk/KalmanTrack/KalRep.hh"
#include "BTrk/KalmanTrack/KalSite.hh"
#include "BTrk/KalmanTrack/KalHit.hh"
#include "BTrk/TrkBase/TrkPoca.hh"
#include <vector>
#include <algorithm>
#include <functional>

namespace mu2e {
  typedef std::vector<TrkStrawHit*>::iterator TSHI;

  HitAmbigResolver::HitAmbigResolver(fhicl::ParameterSet const& pset, double extErr) : 
    AmbigResolver(extErr),
    _mindrift(pset.get<double>("HitMinDrift",0.1)),
    _zeropenalty(pset.get<double>("ZeroDriftPenalty",0.05)),
    _penalty(pset.get<bool>("HitAmbigPenalty",false)),
    _expnorm(pset.get<double>("HitExpNorm",0.03907)),
    _lambda(pset.get<double>("HitLambda",0.1254)),
    _offset(pset.get<double>("HitOffset",0.073)),
    _slope(pset.get<double>("HitSlope",-0.002374))
  {}

  HitAmbigResolver::~HitAmbigResolver() {}

  void
  HitAmbigResolver::resolveTrk(KalRep* krep) const {

    initHitErrors(krep);

// loop over all the hits
    TrkStrawHitVector tshv;
    convert(krep->hitVector(),tshv);
    for (auto itsh=tshv.begin();itsh!=tshv.end(); ++itsh){
// don't allow the hit to auto-update its ambiguity
      (*itsh)->setAmbigUpdate(false);
// get the drift radius
      double rdrift = (*itsh)->driftRadius();
      if(rdrift <= _mindrift){
	(*itsh)->setAmbig(0);
	(*itsh)->setPenalty(_zeropenalty);
      } else {
// find the best trajectory we can local to these hits, but excluding their information ( if possible).
	std::vector<TrkStrawHit*> hits;
	hits.push_back(*itsh);
	const TrkDifTraj* traj = findTraj(hits,krep);
	// compute POCA to this traj
	TrkPoca poca(*traj,(*itsh)->fltLen(),*(*itsh)->hitTraj(),(*itsh)->hitLen());
	if(poca.status().success()){
	  // set the ambiguity if allowed, based on the sign of DOCA
	  int newamb = poca.doca() > 0 ? 1 : -1;
	  (*itsh)->setAmbig(newamb);
	  // based on the drift distance, set the penalty error based on the a-priori function.
	  if(_penalty){
	    double perr = penaltyError((*itsh)->driftRadius());
	    (*itsh)->setPenalty(perr);
	  }
	}
      }
    }
  }

  double
  HitAmbigResolver::penaltyError(double rdrift) const {
    // model is of an exponential plus a linear.
    double frac = _expnorm*exp(-rdrift/_lambda)/_lambda + _offset + _slope*rdrift;
    // the penalty term for a discrete ambiguity error depends only on the mis-assignment probability and the drift distance
    static double sqrt2 = sqrt(2.0);
    double perr = sqrt2*rdrift*sqrt(frac);
    return perr;
  }
}