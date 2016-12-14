#ifndef WIN32
#include <unistd.h>
#endif

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"

#include "WLSMaterials.hh"
#include "WLSPhysicsList.hh"
#include "WLSDetectorConstruction.hh"
#include "WLSPrimaryGeneratorAction.hh"

#include "WLSRunAction.hh"
#include "WLSEventAction.hh"
#include "WLSSteppingAction.hh"
#include "WLSStackingAction.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "CLHEP/Random/Randomize.h"
#include "MakeCrvPhotonArrivals.hh"

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

bool findArgs(int argc, char** argv, const char* c)
{
  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i],c)==0) return true;
  }
  return false; 
}

bool findArgs(int argc, char** argv, const char* c, std::string &value)
{
  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i],c)==0)
    {
      if(i+1<argc)
      {
        if(argv[i+1][0]!='-')
        {
          value=argv[i+1];
          return true;
        } 
      }
      std::cout<<"The argument "<<c<<" requires a value"<<std::endl;
      return false; 
    }
  }
  return false; 
}
bool findArgs(int argc, char** argv, const char* c, int &value)
{
  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i],c)==0)
    {
      if(i+1<argc)
      {
        if(argv[i+1][0]!='-')
        {
          value=atoi(argv[i+1]);
          return true;
        } 
      }
      std::cout<<"The argument "<<c<<" requires a value"<<std::endl;
      return false; 
    }
  }
  return false; 
}
bool findArgs(int argc, char** argv, const char* c, double &value)
{
  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i],c)==0)
    {
      if(i+1<argc)
      {
        if(argv[i+1][0]!='-')
        {
          value=atof(argv[i+1]);
          return true;
        } 
      }
      std::cout<<"The argument "<<c<<" requires a value"<<std::endl;
      return false; 
    }
  }
  return false; 
}

void DrawHistograms(const std::string &lookupFilename)
{
  std::unique_ptr<mu2eCrv::MakeCrvPhotonArrivals> crvPhotonArrivals;
  CLHEP::HepJamesRandom  engine(0);
  CLHEP::RandFlat randFlat(engine);
  crvPhotonArrivals = std::unique_ptr<mu2eCrv::MakeCrvPhotonArrivals>(new mu2eCrv::MakeCrvPhotonArrivals(randFlat));
  crvPhotonArrivals->LoadLookupTable(lookupFilename);
  crvPhotonArrivals->DrawHistograms();
}

int main(int argc, char** argv) 
{
  int mode=-2;
  int simType=-1;
  int minBin=0;
  int maxBin=-1;
  int n=1000;
  int lengthOption=-1;
  std::string lookupFilename="";

  bool verbose = findArgs(argc, argv, "-v");

  if(findArgs(argc, argv, "-h"))
  {
    std::cout<<"Usage ./wls [OPTION]"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Available options:"<<std::endl;
    std::cout<<"-v           Verbose"<<std::endl;
    std::cout<<"-h           Help"<<std::endl;
    std::cout<<"-c           Create lookup table"<<std::endl;
    std::cout<<"-s           Run a simulation with full GEANT"<<std::endl;
    std::cout<<"-S           Run a simulation with lookup table"<<std::endl;
    std::cout<<"-d           Draw histograms of lookup table"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Options for creating the lookup table:"<<std::endl;
    std::cout<<"-t simtype        Simulation type:"<<std::endl;
    std::cout<<"                  0  scintillation in scintillator"<<std::endl;
    std::cout<<"                  1  cerenkov in scintillator"<<std::endl;
    std::cout<<"                  2  cerenkov in fiber 0"<<std::endl;
    std::cout<<"                  3  cerenkov in fiber 1"<<std::endl;
    std::cout<<"-l length option  Length of the scintillator counter:"<<std::endl;
    std::cout<<"                  7600  7600 mm"<<std::endl;
    std::cout<<"                  7350  7350 mm"<<std::endl;
    std::cout<<"                  7100  7100 mm"<<std::endl;
    std::cout<<"                  6600  6600 mm"<<std::endl;
    std::cout<<"                  5600  5600 mm"<<std::endl;
    std::cout<<"                  4500  4500 mm"<<std::endl;
    std::cout<<"                  3000  3000 mm"<<std::endl;
    std::cout<<"                  2300  2300 mm"<<std::endl;
    std::cout<<"                   900   900 mm"<<std::endl;
    std::cout<<"-m minbin         Minimum bin in lookup table (default is 0)."<<std::endl;
    std::cout<<"-M maxbin         Maximum bin in lookup table (default is"<<std::endl;
    std::cout<<"                  the maximum number of bins for this simulation type)."<<std::endl;
    std::cout<<"-n photons        Number of photons to simulate for each bin (default 1000)."<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Options for running the simulation:"<<std::endl;
    std::cout<<"-l length option  Length of the scintillator counter:"<<std::endl;
    std::cout<<"                  7600  7600 mm"<<std::endl;
    std::cout<<"                  7350  7350 mm"<<std::endl;
    std::cout<<"                  7100  7100 mm"<<std::endl;
    std::cout<<"                  6600  6600 mm"<<std::endl;
    std::cout<<"                  5600  5600 mm"<<std::endl;
    std::cout<<"                  4500  4500 mm"<<std::endl;
    std::cout<<"                  3000  3000 mm"<<std::endl;
    std::cout<<"                  2300  2300 mm"<<std::endl;
    std::cout<<"                   900   900 mm"<<std::endl;
    std::cout<<"-n events    Number of events to simulate (default 1000)."<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Options for running the simulation with lookup table:"<<std::endl;
    std::cout<<"-f filename  File with lookup table used for running a simulation"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Options for drawing histograms of the lookup table:"<<std::endl;
    std::cout<<"-f filename  File with lookup table used for running a simulation"<<std::endl;
    std::cout<<std::endl;
    return 0;
  }

  if(findArgs(argc, argv, "-c")+findArgs(argc, argv, "-s")+findArgs(argc, argv, "-S")+findArgs(argc, argv, "-d")!=1)
  {
    std::cout<<"Need to specify exactly one of the following options: -c, -s, -S, or -d."<<std::endl;
    std::cout<<"Use -h for help."<<std::endl;
    return -1;
  }

  if(findArgs(argc, argv, "-S") || findArgs(argc, argv, "-d"))
  {
    if(!findArgs(argc, argv, "-f", lookupFilename))
    {
      std::cout<<"Filename for lookup table needs to be specified"<<std::endl;
      std::cout<<"Use -h for help"<<std::endl;
      return -1;
    }
  }

  if(findArgs(argc, argv, "-d"))
  {
    DrawHistograms(lookupFilename);
    return 0;
  }

  if(findArgs(argc, argv, "-c")) mode=-1;
  if(findArgs(argc, argv, "-s")) mode=0;
  if(findArgs(argc, argv, "-S")) mode=1;

  if(mode==-1)
  {
    if(!findArgs(argc, argv, "-t", simType))
    {
      std::cout<<"Simulation type needs to be specified"<<std::endl;
      std::cout<<"Use -h for help"<<std::endl;
      return -1;
    }
    findArgs(argc, argv, "-m", minBin);
    findArgs(argc, argv, "-M", maxBin);
    findArgs(argc, argv, "-n", n);
  }

  if(mode==0 || mode==1)
  {
    findArgs(argc, argv, "-n", n);
  }

  if(!findArgs(argc, argv, "-l", lengthOption))
  {
    std::cout<<"Option for scintillator counter length needs to be specified"<<std::endl;
    std::cout<<"Use -h for help"<<std::endl;
    return -1;
  }
  else
  {
    if(lengthOption!=900 && 
       lengthOption!=2300 &&
       lengthOption!=3000 &&
       lengthOption!=4500 &&
       lengthOption!=5600 &&
       lengthOption!=6600 &&
       lengthOption!=7100 &&
       lengthOption!=7350 &&
       lengthOption!=7600)
    {
      std::cout<<"Invalid option for scintillator counter length"<<std::endl;
      std::cout<<"Use -h for help"<<std::endl;
      return -1;
    }
  }

  G4String physName = "QGSP_BERT_EMV";
//  G4String physName = "QGSP_BERT_HP";  //for neutrons
  G4int seed = 0;

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(seed);

  G4RunManager *runManager = new G4RunManager;

  WLSMaterials::GetInstance();
  runManager->SetUserInitialization(new WLSDetectorConstruction(lengthOption));
  runManager->SetUserInitialization(new WLSPhysicsList(physName));

  WLSPrimaryGeneratorAction *generator = new WLSPrimaryGeneratorAction(mode, n, simType, minBin, verbose);   //n,simType,minBin not needed in modes 0,1
  WLSRunAction* runAction = new WLSRunAction();
  WLSEventAction* eventAction = new WLSEventAction(mode, n, simType, minBin, verbose); 
  WLSSteppingAction* steppingAction = new WLSSteppingAction(mode, lookupFilename);  //filename not needed in modes -1,0

  runManager->SetUserAction(generator);
  runManager->SetUserAction(runAction);
  runManager->SetUserAction(eventAction);
  runManager->SetUserAction(steppingAction);
  runManager->SetUserAction(new WLSStackingAction);

#if 0 
  G4VisManager *visManager = new G4VisExecutive();
  G4UImanager *UImanager = G4UImanager::GetUIpointer();
  G4UIExecutive *ui = new G4UIExecutive(1,argv);

  visManager->Initialize();
  UImanager->ApplyCommand("/run/initialize");
  UImanager->ApplyCommand("/control/execute vis.mac");
  UImanager->ApplyCommand("/run/beamOn");

  ui->SessionStart();

  delete ui;
  delete visManager;

#else

  runManager->Initialize();
  if(mode==-1)
  {
    const std::vector<double> &xBins     = WLSDetectorConstruction::Instance()->GetXBins();
    const std::vector<double> &yBins     = WLSDetectorConstruction::Instance()->GetYBins();
    const std::vector<double> &zBins     = WLSDetectorConstruction::Instance()->GetZBins();
    const std::vector<double> &betaBins  = WLSDetectorConstruction::Instance()->GetBetaBins();
    const std::vector<double> &thetaBins = WLSDetectorConstruction::Instance()->GetThetaBins();
    const std::vector<double> &phiBins   = WLSDetectorConstruction::Instance()->GetPhiBins();
    const std::vector<double> &rBins     = WLSDetectorConstruction::Instance()->GetRBins();

    int nXBins=xBins.size()-1;   //e.g. 3 bins need 4 entries in the vector (for 3 bin boundaries)
    int nYBins=yBins.size()-1;
    int nZBins=zBins.size()-1;
    int nBetaBins=betaBins.size()-1;
    int nThetaBins=thetaBins.size()-1;
    int nPhiBins=phiBins.size()-1;
    int nRBins=rBins.size()-1;

    int nBins=0;
    switch(simType)
    {
      case 0: //scintillation in scintillator
      case 1: //Cerenkov in scintillator
              nBins=nZBins*nYBins*nXBins;
              break;
      case 2: //Cerenkov in fiber 0
      case 3: //Cerenkov in fiber 1
              nBins=nZBins*nRBins*nPhiBins*nThetaBins*nBetaBins;
              break;
    }

    if(maxBin==-1 || maxBin>=nBins) maxBin=nBins-1;

    int numberOfEvents=maxBin-minBin+1;
    if(numberOfEvents>0)
    {
      std::cout<<std::endl<<std::endl;
      std::cout<<"About to simulate "<<numberOfEvents<<" bins from "<<minBin<<" to "<<maxBin<<"."<<std::endl;
      runManager->BeamOn(numberOfEvents);
      std::cout<<std::endl<<std::endl;
    }
    else
    {

      std::cout<<std::endl<<std::endl;
      std::cout<<"Exceeded the maximum number of bins. no simulation done."<<std::endl;
      std::cout<<std::endl<<std::endl;

      std::stringstream filename;
      filename<<"LookupTable_"<<simType<<"_";
      filename.fill('0');
      filename.width(6);
      filename<<minBin;
      std::remove(filename.str().c_str());
      std::ofstream lookupfile(filename.str(),std::ios::binary|std::ios::app);  //create an empty file to avoid anomalies in the script
      lookupfile.close();
    }
  }
  else if(mode==0 || mode==1)
  {
    runManager->BeamOn(n);
  }
#endif

  delete runManager;

  return 0;
}
