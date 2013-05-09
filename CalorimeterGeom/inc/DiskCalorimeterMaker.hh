#ifndef CalorimeterGeom_DiskCalorimeterMaker_hh
#define CalorimeterGeom_DiskCalorimeterMaker_hh
//
// $Id: DiskCalorimeterMaker.hh,v 1.4 2013/05/09 23:14:14 echenard Exp $
// $Author: echenard $
// $Date: 2013/05/09 23:14:14 $
//
// original authors B. Echenard

//
// C++ includes
#include <iomanip>
#include <memory>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "CLHEP/Vector/ThreeVector.h"

//
//Mu2e includes
#include "CalorimeterGeom/inc/DiskCalorimeter.hh"
#include "CalorimeterGeom/inc/Disk.hh"
#include "ConfigTools/inc/SimpleConfig.hh"


namespace mu2e{


    class SimpleConfig;
    class DiskCalorimeter;

    class DiskCalorimeterMaker{

    public:
 
       DiskCalorimeterMaker(SimpleConfig const& config, double solenoidOffset);
      ~DiskCalorimeterMaker();

      // Accessor and unique_ptr to calorimeter needed by GeometryService.
      std::unique_ptr<DiskCalorimeter> _calo;
      std::unique_ptr<DiskCalorimeter> calorimeterPtr() { return std::move(_calo); }

    private:

      void MakeDisks(void);
      void CheckIt(void);

      int _verbosityLevel;

    };

} //namespace mu2e

#endif /* CalorimeterGeom_DiskCalorimeterMaker_hh */
