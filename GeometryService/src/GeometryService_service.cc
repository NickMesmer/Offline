//
// Maintain up to date geometry information and serve it to
// other services and to the modules.
//
// $Id: GeometryService_service.cc,v 1.13 2011/12/09 01:28:31 gandr Exp $
// $Author: gandr $
// $Date: 2011/12/09 01:28:31 $
//
// Original author Rob Kutschke
//

// C++ include files
#include <iostream>
#include <typeinfo>

// Framework include files
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Persistency/Provenance/ModuleDescription.h"
#include "art/Persistency/Provenance/EventID.h"
#include "art/Persistency/Provenance/Timestamp.h"
#include "art/Persistency/Provenance/SubRunID.h"
#include "art/Persistency/Provenance/RunID.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


// Mu2e include files
#include "GeometryService/inc/GeometryService.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/src/DetectorSystemMaker.hh"
#include "GeometryService/inc/WorldG4.hh"
#include "GeometryService/inc/WorldG4Maker.hh"
#include "GeometryService/inc/Mu2eBuilding.hh"
#include "GeometryService/inc/Mu2eBuildingMaker.hh"
#include "GeometryService/inc/ProductionTarget.hh"
#include "GeometryService/inc/ProductionTargetMaker.hh"
#include "GeometryService/inc/ProtonBeamDump.hh"
#include "GeometryService/inc/ProtonBeamDumpMaker.hh"
#include "TargetGeom/inc/Target.hh"
#include "TargetGeom/inc/TargetMaker.hh"
#include "LTrackerGeom/inc/LTracker.hh"
#include "LTrackerGeom/inc/LTrackerMaker.hh"
#include "TTrackerGeom/inc/TTracker.hh"
#include "TTrackerGeom/inc/TTrackerMaker.hh"
#include "ITrackerGeom/inc/ITracker.hh"
#include "ITrackerGeom/inc/ITrackerMaker.hh"
#include "CalorimeterGeom/inc/Calorimeter.hh"
#include "CalorimeterGeom/inc/CalorimeterMaker.hh"
#include "BFieldGeom/inc/BFieldManager.hh"
#include "BFieldGeom/inc/BFieldManagerMaker.hh"
#include "BeamlineGeom/inc/Beamline.hh"
#include "BeamlineGeom/inc/BeamlineMaker.hh"
#include "VirtualDetectorGeom/inc/VirtualDetector.hh"
#include "VirtualDetectorGeom/inc/VirtualDetectorMaker.hh"
#include "CosmicRayShieldGeom/inc/CosmicRayShield.hh"
#include "CosmicRayShieldGeom/inc/CosmicRayShieldMaker.hh"
#include "ExtinctionMonitorFNAL/inc/ExtMonFNAL.hh"
#include "ExtinctionMonitorFNAL/inc/ExtMonFNAL_Maker.hh"

using namespace std;

namespace mu2e {

  GeometryService::GeometryService(fhicl::ParameterSet const& iPS,
                                   art::ActivityRegistry&iRegistry) :
    _inputfile(iPS.get<std::string>("inputFile","geom000.txt")),
    _detectors(),
    _run_count()
  {
    iRegistry.watchPreBeginRun(this, &GeometryService::preBeginRun);
  }

  GeometryService::~GeometryService(){
  }

  void
  GeometryService::preBeginRun(art::Run const &) {

    if(++_run_count > 1) {
      mf::LogWarning("GEOM") << "This test version does not change geometry on run boundaries.";
      return;
    }

    mf::LogInfo  log("GEOM");
    log << "Geometry input file is: " << _inputfile << "\n";

    _config = auto_ptr<SimpleConfig>(new SimpleConfig(_inputfile));

    if ( _config->getBool("printConfig",false) ){
      log << *_config;
    }

    if ( _config->getBool("printConfigStats",false) ){
      // Work around absence of << operator for this print method.
      ostringstream os;
      _config->printStatistics(os);
      log << os.str();
    }

    // Throw if the configuration is not self consistent.
    checkConfig();

    // This must be the first detector added since other makers may wish to use it.
    addDetector( DetectorSystemMaker( *_config).getDetectorSystemPtr() );

    // Make a detector for every component present in the configuration.

    if(_config->getBool("hasBeamline",false)){
      BeamlineMaker beamlinem( *_config );
      addDetector( beamlinem.getBeamlinePtr() );
    }

    if(true) {
      ProductionTargetMaker tm(*_config);
      addDetector(tm.getDetectorPtr());
    }

    if(true) {
      addDetector(ProtonBeamDumpMaker(*_config).getPtr());
    }

    if(true) {
      Mu2eBuildingMaker b(*_config);
      addDetector(b.getPtr());
    }

    if(_config->getBool("hasTarget",false)){
      TargetMaker targm( *_config );
      addDetector( targm.getTargetPtr() );
    }

    if(_config->getBool("hasLTracker",false)){
      LTrackerMaker ltm( *_config );
      addDetector( ltm.getLTrackerPtr() );
    } else if (_config->getBool("hasITracker",false)){
      ITrackerMaker itm( *_config );
      addDetector( itm.getITrackerPtr() );
    } else if (_config->getBool("hasTTracker",false)){
      TTrackerMaker ttm( *_config );
      addDetector( ttm.getTTrackerPtr() );
    }

    if(_config->getBool("hasCalorimeter",false)){
      CalorimeterMaker calorm( *_config );
      addDetector( calorm.getCalorimeterPtr() );
    }

    if(_config->getBool("hasCosmicRayShield",false)){
      CosmicRayShieldMaker crs( *_config );
      addDetector( crs.getCosmicRayShieldPtr() );
    }

    if(_config->getBool("hasExtMonFNAL",false)){
      ExtMonFNAL::ExtMonMaker extmon( *_config );
      addDetector( extmon.getDetectorPtr() );
    }
    
    if(_config->getBool("hasVirtualDetector",false)){
      VirtualDetectorMaker vdm( *_config );
      addDetector( vdm.getVirtualDetectorPtr() );
    }

    if(_config->getBool("hasBFieldManager",false)){
      BFieldManagerMaker bfmgr( *_config);
      addDetector( bfmgr.getBFieldManager() );
    }

  }

  // Check that the configuration is self consistent.
  void GeometryService::checkConfig(){
    int ntrackers(0);
    string allTrackers;
    if ( _config->getBool("hasLTracker",false) ) {
      allTrackers += " LTracker";
      ++ntrackers;
    }
    if ( _config->getBool("hasTTracker",false) ) {
      allTrackers += " TTracker";
      ++ntrackers;
    }
    if ( _config->getBool("hasITracker",false) ) {
      allTrackers += " ITracker";
      ++ntrackers;
    }

    if ( ntrackers > 1 ){
      throw cet::exception("GEOM")
        << "This configuration has more than one tracker: "
        << allTrackers
        << "\n";
    }

  }

  void GeometryService::addWorldG4() {
    WorldG4Maker w(*_config);
    addDetector(w.getPtr());
  }

} // end namespace mu2e

DEFINE_ART_SERVICE(mu2e::GeometryService);
