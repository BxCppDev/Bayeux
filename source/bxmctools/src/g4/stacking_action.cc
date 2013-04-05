// -*- mode: c++ ; -*- 
/* stacking_action.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string/replace.hpp>

#include <datatools/properties.h>

#include <mctools/g4/stacking_action.h>

// G4 stuff:
#include <globals.hh>
#include <G4TransportationManager.hh>
#include <G4Track.hh>
#include <G4VPhysicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4Navigator.hh>
#include <G4LogicalVolumeStore.hh>

namespace mctools {
  
  namespace g4 {
    
    using namespace std;
    
    // ctor:
    stacking_action::stacking_action ()
    { 
      _debug_  = false;
      _g4_navigator_ = 0;
      _kill_secondary_particles_ = false;
      return;
    }
    
    // dtor:
    stacking_action::~stacking_action ()
    {
      _g4_navigator_ = 0;
      return;
    }
     
    void stacking_action::initialize (const datatools::properties & config_)
    {
      // parsing configuration properties...

      if (config_.has_key ("debug"))
        {
          _debug_ = true;
        }

      if (config_.has_key ("kill_particles"))
        {
          _kill_secondary_particles_ = true;

          if (config_.has_key ("kill_particles.volumes"))
            {
              config_.fetch ("kill_particles.volumes", _killer_volume_names_);
            }

          if (config_.has_key ("kill_particles.materials"))
            {
              config_.fetch ("kill_particles.materials", _killer_material_names_);
            }

        }

      _g4_navigator_ = G4TransportationManager::GetTransportationManager ()->GetNavigatorForTracking ();
      return;
    }
    
    G4ClassificationOfNewTrack stacking_action::ClassifyNewTrack (const G4Track * a_track)
    {
      //std::cerr << "DEVEL: mctools::g4::stacking::ClassifyNewTrack: Entering..." << std::endl;

      G4ClassificationOfNewTrack classification = fUrgent;

      // If the vector of `secondary particle killer logical volumes' is still empty
      // we fill it :
      if ((_killer_volume_names_.size () > 0) && (_killer_volumes_.size () == 0))
        {
          G4LogicalVolumeStore * g4_lv_store = G4LogicalVolumeStore::GetInstance ();
          for (int i = 0; i < (int) _killer_volume_names_.size (); i++)
            {
              ostringstream added_volume_name;
              added_volume_name << _killer_volume_names_[i] << ".log";
              G4String g4_log_name = added_volume_name.str ().c_str ();
              G4LogicalVolume * g4_volume_log = g4_lv_store->GetVolume (g4_log_name, false);
              if (g4_volume_log == 0)
                {
                  ostringstream message;
                  message << "mctools::g4::stacking_action::ClassifyNewTrack: "
                          << "Logical volume '" << g4_log_name << "' does not exists !";
                  throw logic_error (message.str ());
                }
              std::cerr << "DEVEL: mctools::g4::stacking::ClassifyNewTrack: Add a secondary particle killer logical volume  '" 
                        << g4_log_name << "'" << endl;
              _killer_volumes_.push_back (g4_volume_log);
            }
        }

      if ((_killer_material_names_.size () > 0) && (_killer_materials_.size () == 0))
        {
          //G4LogicalVolumeStore * g4_lv_store = G4LogicalVolumeStore::GetInstance ();
          for (int i = 0; i < (int) _killer_material_names_.size (); i++)
            {
              ostringstream added_material_name;
              added_material_name << _killer_material_names_[i];
              string stmp = added_material_name.str ();
              boost::algorithm::replace_all (stmp, ":", "_"); // a trick for GDML does not support ':' in (XML) names.
              G4String g4_material_name = stmp.c_str ();
              G4Material * g4_material = G4Material::GetMaterial (g4_material_name, false);
              if (g4_material == 0)
                {
                  ostringstream message;
                  message << "mctools::g4::stacking_action::ClassifyNewTrack: "
                          << "Material '" << g4_material_name << "' does not exists !";
                  throw logic_error (message.str ());
                }
              std::cerr << "DEVEL: mctools::g4::stacking::ClassifyNewTrack: Add a secondary particle killer material  '" 
                        << g4_material_name << "'" << endl;
              _killer_materials_.push_back (g4_material);
            }
        }
      

      //string g4_volume_name;
      //g4_volume_name = _g4_navigator_->LocateGlobalPointAndSetup (pos, NULL, false)->GetName ();

      // Another possibility to get the volume name:
      //   int depth=0;
      //   G4TouchableHistory * theTouchable = (G4TouchableHistory*) (a_track->GetTouchable ());
      //   if (theTouchable) 
      //     {
      //       depth = theTouchable->GetHistoryDepth ();
      //     }
      //   if (depth > 1) 
      //     {
      //       volume_name = theTouchable->GetVolume ()->GetName ();
      //       clog << volume_name << G4endl;
      //     }
      
      bool kill = false;
      if (_kill_secondary_particles_ && ( a_track->GetParentID () > 0 ))
        {
          G4ThreeVector pos = a_track->GetPosition ();  
          G4VPhysicalVolume * g4_volume_phys = _g4_navigator_->LocateGlobalPointAndSetup (pos, NULL, false);
          G4LogicalVolume * g4_volume_log = g4_volume_phys->GetLogicalVolume ();
          // std::cerr << "DEVEL: mctools::g4::stacking::ClassifyNewTrack: g4_volume_log = '" 
          //        << g4_volume_log->GetName () << "'" << endl;

          // Kill by material :
          if (! kill)
            {
              if (g4_volume_log != 0)
                {
                  G4Material * g4_material = g4_volume_log->GetMaterial();
                  if (g4_material != 0)
                    {             
                      // Killing every particle generated in some specific materials :
                      if (find (_killer_materials_.begin (), 
                                _killer_materials_.end (), 
                                g4_material) != _killer_materials_.end ())
                        {
                          kill = true;
                          /*
                            clog << "WARNING: stacking_action::ClassifyNewTrack: "
                            << "New secondary particle '" 
                            << a_track->GetDefinition()->GetParticleName () 
                            << "' in material '" << g4_material->GetName () 
                            << "' is killed by SNG4 to speed up." << endl;
                          */
                        }
                    }
                  else
                    {     
                      /*
                        clog << "WARNING: stacking_action::ClassifyNewTrack: "
                        << "No step is available from this new track !" << endl;
                      */
                    }
                }
            }

          // Kill by volume :
          if (! kill)
            {
              if (g4_volume_log != 0)
                {
                  // Killing every particle generated in some specific volumes :
                  if (find (_killer_volumes_.begin (), 
                            _killer_volumes_.end (), 
                            g4_volume_log) != _killer_volumes_.end ())
                    {
                      kill = true;
                      /*
                        clog << "WARNING: stacking_action::ClassifyNewTrack: "
                        << "New secondary particle '" 
                        << a_track->GetDefinition()->GetParticleName () 
                        << "' in logical volume '" << g4_volume_log->GetName () 
                        << "' is killed by SNG4 to speed up." << endl;
                      */
                    }
                }
            }

        }
      if (kill)
        {
          classification = fKill;
        }

      return classification;
    }

    void stacking_action::NewStage ()
    {
      return;
    }
    
    void stacking_action::PrepareNewEvent ()
    { 
      return;
    }
    
  } // end of namespace g4

} // end of namespace mctools

// end of event_action.cc
