// class to define mu2e track fit direction convention.  In a Kalman fit, this is the direction the partlcle
// physically travels WRT time, and the pitch sign.  Together with the BField and the particle charge,
// this also defines the angular velocity sign.
//
// $Id: TrkFitDirection.hh,v 1.2 2012/08/04 00:38:06 brownd Exp $
// $Author: brownd $ 
// $Date: 2012/08/04 00:38:06 $
#ifndef TrkFitDirection_HH
#define TrkFitDirection_HH
#include <string>

namespace mu2e 
{
  class TrkFitDirection {
    public:
// define the fit direction as downstream (towards positive Z) or upstream (towards negative Z).
      enum FitDirection {downstream=0,upstream};
      TrkFitDirection(FitDirection fdir=downstream);
      FitDirection fitDirection() const { return _fdir; }
      double dzdt() const { return _fdir == downstream ? 1.0 : -1.0; }
      std::string const& name() const;
    private:
      FitDirection _fdir;
  };
}
#endif
