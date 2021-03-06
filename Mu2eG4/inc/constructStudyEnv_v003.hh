#ifndef Mu2eG4_constructStudyEnv_v003_hh
#define Mu2eG4_constructStudyEnv_v003_hh
//
// Free function to create calorimetric study environment geometry plus a helper function
//
// $Id: constructStudyEnv_v003.hh,v 1.2 2013/04/09 23:18:20 genser Exp $
// $Author: genser $
// $Date: 2013/04/09 23:18:20 $
//
// Original author KLG
//

#include <vector>
#include <string>
#include "G4Colour.hh"

namespace mu2e {

  class VolumeInfo;
  class SimpleConfig;

  void constructStudyEnv_v003(VolumeInfo   const & parentVInfo,
                              SimpleConfig const & _config
                              );

  void constructDoubleLayerdModule(std::string const & mNamePrefix,
                                   VolumeInfo  const & parentVInfo,
                                   std::vector<double> const & tHL,
                                   std::vector<double> const & tCInParent,
                                   std::vector<double> const & hHL,
                                   std::vector<std::string> const & mMat,
                                   G4int    mNumberOfLayers,
                                   G4int    verbosityLevel,
                                   G4double mLCenterInParent,
                                   G4int passiveVolumeStartingCopyNumber,
                                   G4int  activeVolumeStartingCopyNumber,
                                   bool const isVisible,
                                   G4Colour const & passiveVolumeColour,
                                   G4Colour const &  activeVolumeColour,
                                   bool const forceSolid,
                                   bool const forceAuxEdgeVisible,
                                   bool const placePV,
                                   bool const doSurfaceCheck
                                   );

}

#endif /* Mu2eG4_constructStudyEnv_v003_hh */
