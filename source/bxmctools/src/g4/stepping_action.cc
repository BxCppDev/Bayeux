// -*- mode: c++ ; -*-
/* stepping_action.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <mctools/g4/stepping_action.h>

#include <datatools/ioutils.h>
#include <datatools/properties.h>

#include <globals.hh>
#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"
#include "G4NavigationHistory.hh"
#include "G4ReferenceCountedHandle.hh"

namespace mctools {

  namespace g4 {

    using namespace std;

    // ctor:
    stepping_action::stepping_action ()
    {
      _debug_  = false;
      _number_of_steps_ = 0;
      return;
    }

    // dtor:
    stepping_action::~stepping_action ()
    {
      clog << "NOTICE: " << "snemo::g4::stepping_action::~stepping_action: "
           << "Number of steps = " 
           << _number_of_steps_
           << endl;
      return;
    }
    
    void stepping_action::initialize (const datatools::properties & config_)
    {
      // parsing configuration properties...
      return;
    }

    void stepping_action::UserSteppingAction (const G4Step * g4_step_)
    {
      this->_stepping_action_base (g4_step_);
      return;
    }

    void stepping_action::_stepping_action_base (const G4Step * g4_step_)
    {
      bool devel = false;
      if (! devel) devel = _debug_;
      //devel = true;
      if (devel)
        {
          clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
               << "Entering..." << endl;
        }
 
      _number_of_steps_++;

      /*
      bool process_step = false;

      if (process_step)
        {
          const G4VTouchable * post_step_touch
            = g4_step_->GetPostStepPoint()->GetTouchable ();
          const G4VTouchable * pre_step_touch
            = g4_step_->GetPreStepPoint()->GetTouchable ();

          const G4ReferenceCountedHandle<G4VTouchable> theTouchable
            = g4_step_->GetPostStepPoint ()->GetTouchableHandle ();

          G4Track * track              = fpSteppingManager->GetTrack ();
          G4int     track_id           = track->GetTrackID ();
          G4int     mother_track_id    = track->GetParentID ();
          G4int     step_id            = track->GetCurrentStepNumber ();
          G4String  particle_name      = track->GetDefinition ()->GetParticleName ();
          G4double  particle_charge    = track->GetDynamicParticle ()->GetCharge ();
          G4VPhysicalVolume * vol = track->GetVolume ();
          // if (vol == 0)
          //   {
          //     clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
          //       << "!!! NULL VOLUME !!!" << endl;
          //   }
          G4String  volume_name        = vol->GetName ();
          G4double  delta_energy       = g4_step_->GetDeltaEnergy ();
          G4String  next_volume_name   = track->GetNextVolume ()->GetName ();
          G4double  kinetic_energy     = track->GetKineticEnergy ();
          G4ThreeVector pre_position   = pre_step_touch->GetTranslation ();
          G4ThreeVector post_position  = post_step_touch->GetTranslation ();
          G4ThreeVector track_position = track->GetPosition ();
          G4ThreeVector track_time     = G4ThreeVector (track->GetGlobalTime () * CLHEP::ns);
          G4ThreeVector track_momentum = track->GetMomentum ();
          G4String process
            = g4_step_->GetPostStepPoint ()->GetProcessDefinedStep ()->GetProcessName ();
          G4double total_energy_deposit = g4_step_->GetTotalEnergyDeposit ();
        }
      */
      if (devel)
        {
          clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
               << "Exiting." << endl;
        }
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/*
  // Code sample that make be useful but care must be taken with the
  // navigation through the depth of the G4TouchableHistory:

  G4TouchableHandle pre_step_touchable_handle = g4_step_->GetPreStepPoint ()->GetTouchableHandle ();
  G4VPhysicalVolume * mother_volume = 0;
  int depth = pre_step_touchable_handle->GetHistoryDepth();
  clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
  << "depth==" << depth << " volume=" << volume_name << endl;
  if (depth < 1)
  {
  clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
  << "depth==0 for " << pre_step_touchable_handle->GetVolume (0)->GetName () << endl;

  }
  else
  {
  mother_volume = pre_step_touchable_handle->GetVolume (1);
  if (! mother_volume)
  {
  if (devel)
  {
  clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
  << "mother_volume==NULL" << endl;
  }
  }
  else
  {
  G4VPhysicalVolume * grand_mother_volume = 0;
  grand_mother_volume = pre_step_touchable_handle->GetVolume (2);
  if (! grand_mother_volume)
  {
  if (devel)
  {
  clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
  << "grand_mother_volume==NULL" << endl;
  }
  }
  }
  G4int some_copy_number = pre_step_touchable_handle->GetCopyNumber (1);
  }
*/

// end of stepping_action.cc
