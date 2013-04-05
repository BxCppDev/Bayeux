// -*- mode: c++ ; -*- 
/* stepping_verbose.cc
 */

#include <mctools/g4/stepping_verbose.h>

// G4 stuff:
#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"
#include "G4NavigationHistory.hh"
#include "G4ReferenceCountedHandle.hh"

namespace mctools {

  namespace g4 {
  
    using namespace std;

    stepping_verbose::stepping_verbose ()
    {
    }
     
    stepping_verbose::~stepping_verbose ()
    {
    }
    
    void stepping_verbose::StepInfo ()
    {
      CopyState ();
      
      int saved_precision = clog.precision (10);
      
      if (verboseLevel >= 1)
        {
          if (verboseLevel >= 4) VerboseTrack ();
          if (verboseLevel >= 3)
            {
              clog << endl;    
              clog << "mctools::g4::stepping_verbose::StepInfo " << endl
                   << setw ( 6) << "Step num"
                   << setw ( 6) << "X"          << "    "
                   << setw ( 6) << "Y"          << "    "
                   << setw ( 6) << "Z"          << "    "
                   << setw ( 9) << "KineE"      << " "
                   << setw ( 9) << "dEStep"     << " "  
                   << setw (10) << "StepLeng"     
                   << setw (10) << "TrakLeng"  << "  "
                   << setw (20) << "Volume"    << "  "
                   << setw (10) << "Material"
                   << setw (10) << "Process"   << endl;           
            }
          
          clog << setw (5) << fTrack->GetCurrentStepNumber () << " "
               << setw (6) << G4BestUnit (fTrack->GetPosition ().x (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetPosition ().y (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetPosition ().z (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetKineticEnergy (), "Energy")
               << setw (6) << G4BestUnit (fStep->GetTotalEnergyDeposit (), "Energy")
               << setw (6) << G4BestUnit (fStep->GetStepLength (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetTrackLength (),"Length")
               << "  ";
          
          // if (fStepStatus != fWorldBoundary){ 
          if (fTrack->GetNextVolume () != 0) 
            { 
              clog << setw (20) << fTrack->GetVolume ()->GetName ();
            } 
          else 
            {
              clog << setw (20) << "<out of world>";
            }
          
          clog << setw (10) << fTrack->GetMaterial ()->GetName ();
          
          if (fStep->GetPostStepPoint ()->GetProcessDefinedStep () != NULL)
            {
              clog << "  " 
                   << setw (10) << fStep->GetPostStepPoint ()->GetProcessDefinedStep ()
                ->GetProcessName ();
            } 
          else 
            {
              clog << "   UserLimit";
            }
          
          clog << endl;
          
          if (verboseLevel == 2)
            {
              G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
                fN2ndariesAlongStepDoIt +
                fN2ndariesPostStepDoIt;
              if (tN2ndariesTot > 0)
                {
                  clog << endl 
                       << "    :----- List of 2ndaries - "
                       << "#SpawnInStep=" << setw (3) << tN2ndariesTot << "  "
                       << " (Rest="  << setw (2) << fN2ndariesAtRestDoIt
                       << ",Along=" << setw (2) << fN2ndariesAlongStepDoIt
                       << ",Post="  << setw (2) << fN2ndariesPostStepDoIt
                       << "), "
                       << "#SpawnTotal=" << setw (3) << (*fSecondary).size ()
                       << " ---------------"
                       << endl;
                  
                  for (size_t lp1= (*fSecondary).size () - tN2ndariesTot; 
                       lp1 < (*fSecondary).size (); 
                       lp1++)
                    {
                      clog << "    : "
                           << setw (6)
                           << G4BestUnit ((*fSecondary)[lp1]->GetPosition ().x (), "Length")
                           << setw (6)
                           << G4BestUnit ((*fSecondary)[lp1]->GetPosition ().y (), "Length")
                           << setw (6)
                           << G4BestUnit ((*fSecondary)[lp1]->GetPosition ().z (), "Length")
                           << setw (6)
                           << G4BestUnit ((*fSecondary)[lp1]->GetKineticEnergy (), "Energy")
                           << setw (10)
                           << (*fSecondary)[lp1]->GetDefinition ()->GetParticleName ()
                           << endl;
                    }
                
                  clog << "    :-----------------------------"
                       << "----------------------------------"
                       << "-- EndOf2ndaries Info ---------------"
                       << endl;
                }
            } 
        }
      clog.precision (saved_precision);
      return;
    }


    void stepping_verbose::TrackingStarted ()
    {
      CopyState();
    
      int saved_precision = clog.precision (10);
      if (verboseLevel > 0)
        {
          clog << setw (5) << "In Stepping Verbose: Tracking Started:" << endl << " Step "
               << setw (6) << "X"          << "    "
               << setw (6) << "Y"          << "    "  
               << setw (6) << "Z"          << "    "
               << setw (9) << "KineE"      << " "
               << setw (9) << "dEStep"     << " "  
               << setw (10) << "StepLeng"  
               << setw (10) << "TrakLeng"   << "  "
               << setw (20) << "Volume"     << "  "
               << setw (10) << "Material"
               << setw (10) << "Process"    << endl;         

          clog << setw (5) << fTrack->GetCurrentStepNumber () << " "
               << setw (6) << G4BestUnit (fTrack->GetPosition ().x (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetPosition ().y (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetPosition ().z (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetKineticEnergy (), "Energy")
               << setw (6) << G4BestUnit (fStep->GetTotalEnergyDeposit (), "Energy")
               << setw (6) << G4BestUnit (fStep->GetStepLength (), "Length")
               << setw (6) << G4BestUnit (fTrack->GetTrackLength (), "Length")
               << "  ";

          if (fTrack->GetNextVolume () != 0)
            {
              clog << setw (10) << fTrack->GetVolume ()->GetName ();
            } 
          else 
            {
              clog << setw (20) << "<out of world>";
            }
          // clog << setw (10) << "  XXXXXX";
          // clog << "    initStep" << endl;
          clog << endl;
        }
      clog.precision (saved_precision);
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of stepping_verbose.cc
