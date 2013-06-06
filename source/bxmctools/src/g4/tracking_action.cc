// -*- mode: c++ ; -*-
/* tracking_action.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <datatools/properties.h>

#include <mctools/g4/tracking_action.h>

#include <globals.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TrackingManager.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4Step.hh>

namespace mctools {

  namespace g4 {

    using namespace std;

    // ctor:
    tracking_action::tracking_action ()
    {
      return;
    }

    // dtor:
    tracking_action::~tracking_action ()
    {
      return;
    }

    void tracking_action::initialize (const datatools::properties & config_)
    {
      // parsing configuration properties...
      _initialize_logging_support(config_);
      return;
    }

    void tracking_action::PreUserTrackingAction (const G4Track * track_)
    {
      /*
      G4int track_id     = track_->GetTrackID ();
      G4int mother_id    = track_->GetParentID ();
      G4String part_name = track_->GetDefinition ()->GetParticleName ();
      std::string volume = track_->GetVolume ()->GetName ();
      CLHEP::Hep3Vector position = track_->GetPosition ();
      */

      return;
    }

    void tracking_action::PostUserTrackingAction(const G4Track* track_)
    {
      //G4int             track_id = track_->GetTrackID ();
      //CLHEP::Hep3Vector position = track_->GetPosition ();

      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of tracking_action.cc
