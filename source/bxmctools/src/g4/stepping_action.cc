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

    // ctor:
    stepping_action::stepping_action ()
    {
      _number_of_steps_ = 0;
      return;
    }

    // dtor:
    stepping_action::~stepping_action ()
    {
      // clog << "NOTICE: " << "snemo::g4::stepping_action::~stepping_action: "
      //      << "Number of steps = "
      //      << _number_of_steps_
      //      << endl;
      return;
    }

    void stepping_action::initialize (const datatools::properties & config_)
    {
      // parsing configuration properties...
      loggable_support::_initialize_logging_support(config_);
      return;
    }

    void stepping_action::UserSteppingAction (const G4Step * g4_step_)
    {
      this->_stepping_action_base (g4_step_);
      return;
    }

    void stepping_action::_stepping_action_base (const G4Step * g4_step_)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      _number_of_steps_++;

      /*
        bool process_step = false;

        if (process_step) {
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
      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::stepping_action,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::stepping_action");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation optional stepping action");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is Geant4 simulation engine embedded stepping action.      \n"
                                );

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"fatal\"``       : print fatal error messages    \n"
                            " * ``\"critical\"``    : print critical error messages \n"
                            " * ``\"error\"``       : print error messages          \n"
                            " * ``\"warning\"``     : print warnings                \n"
                            " * ``\"notice\"``      : print notice messages         \n"
                            " * ``\"information\"`` : print informational messages  \n"
                            " * ``\"debug\"``       : print debug messages          \n"
                            " * ``\"trace\"``       : print trace messages          \n"
                            "                                                       \n"
                            "Default value: ``\"warning\"``                         \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  logging.priority : string = \"warning\"              \n"
                            "                                                       \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Stacking action logging priority                        \n"
                               " logging.priority : string = \"warning\"                               \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::stepping_action,"mctools::g4::stepping_action")

/*
// Code sample that may be useful but care must be taken with the
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
