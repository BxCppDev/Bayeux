// -*- mode: c++ ; -*-
/* primary_generator.cc
 */

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <cmath>
#include <fstream>
#include <string>

#include <datatools/units.h>
#include <datatools/properties.h>

#include <genbb_help/i_genbb.h>
#include <genvtx/i_vertex_generator.h>

#include <mctools/g4/primary_generator.h>
#include <mctools/g4/manager.h>
#include <mctools/g4/run_action.h>
#include <mctools/g4/event_action.h>

// G4 Stuff:
#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleMomentum.hh>
#include <Randomize.hh>

#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4Alpha.hh>

namespace mctools {

  namespace g4 {

    using namespace std;

    bool primary_generator::is_initialized () const
    {
      return _initialized_;
    }

    bool primary_generator::is_debug () const
    {
      return _debug_;
    }

    void primary_generator::set_debug (bool d_)
    {
      _debug_ = d_;
      return;
    }

    void primary_generator::_assert_lock (const string & what_)
    {
      if (_initialized_)
        {
          ostringstream message;
          message << what_ << ": Operation prohibited ! Manager is locked !";
          G4Exception (message.str ());
        }
      return;
    }

    void primary_generator::set_run_action (mctools::g4::run_action & run_action_)
    {
      _assert_lock ("primary_generator::set_run_action");
      _run_action_ = &run_action_;
      return;
    }

    void primary_generator::set_event_action (mctools::g4::event_action & event_action_)
    {
      _assert_lock ("primary_generator::set_event_action");
      _event_action_ = &event_action_;
      return;
    }
 
    bool primary_generator::has_vertex_generator() const
    {
      return _vertex_generator_ != 0;
    }

    void primary_generator::set_vertex_generator (::genvtx::i_vertex_generator & vertex_generator_)
    {
      _assert_lock ("primary_generator::set_vertex_generator");
      _vertex_generator_ = &vertex_generator_;
      return;
    }

    void primary_generator::set_event_generator (::genbb::i_genbb & event_generator_)
    {
      _assert_lock ("primary_generator::set_event_generator");
      _event_generator_ = &event_generator_;
      return;
    }

    size_t primary_generator::get_event_counter () const
    {
      return _event_counter_;
    }

    void primary_generator::reset_event_counter ()
    {
      _event_counter_ = 0;
      return;
    }

    void primary_generator::_set_defaults ()
    {
      reset_event_counter ();
      return;
    }

    // ctor:
    primary_generator::primary_generator ()
    {
      //clog << "NOTICE: primary_generator::primary_generator: Entering...." << endl;
      _debug_ = false;
      _initialized_ = false;
      _run_action_ = 0;
      _event_action_ = 0;
      _vertex_generator_ = 0;
      _event_generator_ = 0;
      _particle_gun_ = 0;
      _event_counter_ = 0;
      _set_defaults ();
      return;
    }

    // dtor:
    primary_generator::~primary_generator ()
    {
      //clog << "DEVEL: primary_generator::~primary_generator: Entering..." << endl;
      if (_initialized_)
        {
          reset ();
        }
      //clog << "DEVEL: primary_generator::~primary_generator: Exiting." << endl;
      return;
    }

    void primary_generator::_check ()
    {
      if (has_vertex_generator())
        {
          if (! _vertex_generator_->is_initialized ())
            {
              G4Exception ("primary_generator::_check: Vertex generator manager is not initialized !");
            }
        }
      else
        {
          std::clog << datatools::io::warning
                    << "mctools::g4::primary_generator::_check: "
                    << "No vertex generator is provided; we assume the vertex will be "
                    << "generated at the event generator level..." 
                    << std::endl;
        }

      if (! _event_generator_->is_initialized ())
        {
          G4Exception ("primary_generator::_check: Event generator manager is not initialized !");
        }

      return;
    }

    void primary_generator::initialize (const datatools::properties & config_)
    {
      if (is_initialized ())
        {
          G4Exception ("primary_generator::initialized: Primary_Generator already initialized !");
        }

      // properties parsing:
      if (config_.has_flag ("debug"))
        {
          _debug_ = true;
        }

      // Convert particle names from genbb's scheme to Geant4 particle naming scheme
      if (config_.has_key("particle_names_map"))
        {
          // XXX not implemented yet !
        }

      // checks:
      if (_run_action_ == 0)
        {
          G4Exception ("primary_generator::initialized: Missing run action !");
        }

      if (_event_action_ == 0)
        {
          G4Exception ("primary_generator::initialized: Missing event action !");
        }

      if (_vertex_generator_ == 0)
        {
          G4Exception ("primary_generator::initialized: Missing vertex generator !");
        }

      if (_event_generator_ == 0)
        {
          G4Exception ("primary_generator::initialized: Missing event generator !");
        }

      _check ();
      int n_particle = 1;
      _particle_gun_ = new G4ParticleGun (n_particle);

      _initialized_ = true;
      return;
    }

    void primary_generator::reset ()
    {
      //clog << "DEVEL: primary_generator::reset: Entering..." << endl;
      if (! is_initialized ())
        {
          G4Exception ("primary_generator::reset: Primary_Generator is not initialized!");
        }

      // destroy the gun:
      if (_particle_gun_ != 0)
        {
          delete _particle_gun_;
          _particle_gun_ = 0;
        }

      _run_action_ = 0;
      _event_action_ = 0;
      _event_generator_ = 0;
      _vertex_generator_ = 0;

      _set_defaults ();

      _initialized_ = false;
      //clog << "DEVEL: primary_generator::reset: Done." << endl;
      return;
    }

    void primary_generator::GeneratePrimaries (G4Event * g4_event_)
    {
      if (! is_initialized ())
        {
          G4Exception ("primary_generator::GeneratePrimaries: Primary_Generator is not initialized!");
        }

      // THIS IS NOW INVALIDATED: 2011-04-19 FM: moved these bits to 'event_action::BeginOfEventAction' :
      // Clear event data:
      // FIX BUG: 2011-05-10  FM+XG : DO CLEAR THE EVENT DATA MODEL HERE !
      // clog << "DEVEL: primary_generator::GeneratePrimaries: "
      //      << "Clear event data..." << endl;
      _event_action_->grab_event_data ().clear ();

      // Generate the vertex:
      geomtools::invalidate (_current_vertex_);
      if(_vertex_generator_ != 0) 
        {
          _generate_vertex ();
        }

      // Generate the primary event:
      _generate_event (g4_event_);
      
      // Increment the event counter:
      _event_counter_++;

      return;
    }

    void primary_generator::_generate_vertex ()
    {
      // Invoke the vertex generator:
      manager & mgr = _event_action_->grab_run_action ().grab_manager ();
      if (! _vertex_generator_->has_next_vertex ())
        {
          cerr << "ERROR: mctools::g4::primary_generator::_generate_vertex: "
               << "No more generated vertex from the vertex generator after "
               << _event_counter_ << " generated events !" << endl;
          G4RunManager::GetRunManager ()->AbortRun ();
          _event_action_->set_aborted_event (true);
          return;
        }
     if (mgr.using_time_stat ())
        {
          mgr.grab_CT_map ()["VG"].start ();
        }
     _vertex_generator_->shoot_vertex (mgr.grab_vg_prng(), _current_vertex_);
     if (mgr.using_time_stat ())
        {
          mgr.grab_CT_map ()["VG"].stop ();
        }

      // save current event vertex:
      _event_action_->grab_event_data ().set_vertex (_current_vertex_);
      return;
    }

    void primary_generator::_generate_event (G4Event * g4_event_)
    {
      bool devel = false;
      // devel = true;

      if (! _event_generator_->has_next ())
        {
          cerr << "ERROR: mctools::g4::primary_generator::_generate_event: "
               << "No more generated event from the event generator after "
               << _event_counter_ << " generated events !" << endl;
          G4RunManager::GetRunManager ()->AbortRun ();
          _event_action_->set_aborted_event (true);
          return;
        }

      ::genbb::primary_event & current_generated_event
        = _event_action_->grab_event_data ().grab_primary_event ();

      manager & mgr = _event_action_->grab_run_action ().grab_manager ();
      if (mgr.using_time_stat ())
        {
          mgr.grab_CT_map ()["EG"].start ();
        }
      _event_generator_->load_next (current_generated_event);
      if (mgr.using_time_stat ())
        {
          mgr.grab_CT_map ()["EG"].stop ();
        }
      current_generated_event.set_time (0.0 * CLHEP::ns);
      double event_time = current_generated_event.get_time ();

      if (devel)
        {
          current_generated_event.dump (clog,
                                        "Current generated event: ",
                                        "DEVEL: primary_generator::_generate_event: ");
        }

      G4ParticleTable * particle_table = G4ParticleTable::GetParticleTable ();

      // Use the recently randomized vertex:
      /*
        G4ThreeVector vertex (_current_vertex_.x (),
                            _current_vertex_.y (),
                            _current_vertex_.z ());
      */

      // Loop on particles:
      size_t particle_counter = 0;
      for (::genbb::primary_event::particles_col_type::const_iterator i
             = current_generated_event.grab_particles ().begin ();
           i != current_generated_event.grab_particles ().end ();
           i++)
        {
          const ::genbb::primary_particle & genbb_particle = *i;
          particle_counter += 1;

          string particle_label = ::genbb::primary_particle::get_label (genbb_particle.get_type());
          double particle_time       = genbb_particle.get_time ();
          double total_momentum      = genbb_particle.get_momentum ().mag ();
          double relativistic_energy = 0.;
          double kinetic_energy      = 0.;

          /* Note:
           * G4 particle not implemented:
           *  "nu_e" "nu_mu" "nu_tau"
           *  "anti_nu_e" "anti_nu_mu" "anti_nu_tau"
           *  "opticalphoton"
           *  ...
           */
          G4String g4_particle_name = get_g4_particle_name_from_genbb_particle (genbb_particle);
          if (g4_particle_name[0] == '?')
            {
              std::ostringstream message;
              message << "mctools::g4::primary_generator::_generate_event: "
                      << "genbb's particle type " << genbb_particle.get_type() << " (with label='" 
                      << genbb_particle.get_particle_label() << "') is not recognized as a Geant4 particle !";
              G4Exception (message.str ());           
            }
          G4ParticleDefinition * g4_particle = particle_table->FindParticle (g4_particle_name);
          if (g4_particle == 0)
            {
              std::ostringstream message;
              message << "mctools::g4::primary_generator::_generate_event: "
                      << "Particle named '" << g4_particle_name << "' is not defined within the Geant4 framework !";
              G4Exception (message.str ());
            }
          double mass = g4_particle->GetPDGMass ();
          relativistic_energy = hypot (total_momentum, mass);
          kinetic_energy = relativistic_energy - mass;

          // Extract momentum:
          G4ThreeVector momentum (genbb_particle.get_momentum ().x (),
                                  genbb_particle.get_momentum ().y (),
                                  genbb_particle.get_momentum ().z ());
          // Plug in G4:
          if (geomtools::is_valid(_current_vertex_))
            {
              // All particles originate from an unique vertex provided by the vertex generator
              _particle_gun_->SetParticlePosition (_current_vertex_);
            }
          else
            {
              if (! genbb_particle.has_vertex ()) 
                {
                  std::ostringstream message;
                  message << "mctools::g4::primary_generator::_generate_event: "
                          << "Particle named '" << g4_particle_name << "' has no valid vertex provided by the event generator !";
                  G4Exception (message.str ());
                }
              // Each particle originate from its own vertex provided by the event generator
              _particle_gun_->SetParticlePosition (genbb_particle.get_vertex ());
            }
          _particle_gun_->SetParticleDefinition (g4_particle);
          _particle_gun_->SetParticleMomentumDirection (momentum);
          _particle_gun_->SetParticleEnergy     (kinetic_energy);
          _particle_gun_->SetParticleTime       (event_time + particle_time);
          _particle_gun_->GeneratePrimaryVertex (g4_event_);

          if (devel)
            {
              const string tag           = "|-- ";
              const string skip_tag      = "|   ";
              const string last_tag      = "`-- ";
              const string last_skip_tag = "    ";
              string tag1 = tag;
              string tag2 = skip_tag;
              if (particle_counter == current_generated_event.get_particles().size ())
                {
                  tag1 = last_tag;
                  tag2 = last_skip_tag;
                }
              clog << tag1 << "Particle #" << particle_counter << ":" << endl;
              clog << tag2 << tag      << "Type       = "
                   << genbb_particle.get_type() << " ('" << particle_label << "')"
                   << endl;
              clog << tag2 << tag      << "Time       = "
                   << _particle_gun_->GetParticleTime () / CLHEP::ns << " ns"
                   << endl;
              clog << tag2 << tag      << "Energy     = " << kinetic_energy / CLHEP::keV << " keV" << endl;
              clog << tag2 << tag      << "Position   = " << _current_vertex_ / CLHEP::mm << " mm" << endl;
              clog << tag2 << last_tag << "Momentum   = " << genbb_particle.get_momentum() / CLHEP::keV << " keV" << endl;
            }
        }

      return;
    }

    std::string primary_generator::get_g4_particle_name_from_genbb_particle (const ::genbb::primary_particle & p_) const
    {
      if (p_.is_gamma ())
        {
          return "gamma";
        }
      if (p_.is_electron ())
        {
          return "e-";
        }
      if (p_.is_positron ())
        {
          return "e+";
        }
      if (p_.is_alpha ())
        {
          return "alpha";
        }
      if (p_.get_type () == ::genbb::primary_particle::PROTON)
        {
          return "proton";
        }
      if (p_.get_type () == ::genbb::primary_particle::NEUTRON)
        {
          return "neutron";
        }
      if (p_.get_type () == ::genbb::primary_particle::MUON_MINUS)
        {
          return "mu-";
        }
      if (p_.get_type () == ::genbb::primary_particle::MUON_PLUS)
        {
          return "mu+";
        }
      if (p_.get_type () == ::genbb::primary_particle::MUON_MINUS)
        {
          return "mu-";
        }
      if (p_.get_type () == ::genbb::primary_particle::MUON_PLUS)
        {
          return "mu+";
        }
      if (p_.get_type () == ::genbb::primary_particle::PION_0)
        {
          return "pi0";
        }
      if (p_.get_type () == ::genbb::primary_particle::PION_PLUS)
        {
          return "pi+";
        }
      if (p_.get_type () == ::genbb::primary_particle::PION_MINUS)
        {
          return "pi-";
        }
      const std::string & part_label = p_.get_particle_label ();

      std::map<std::string, std::string>::const_iterator found = _particle_names_map_.find (part_label);
      if (found != _particle_names_map_.end())
        {
          return found->second;
        }
      return "?";
   }

  } // namespace g4

} // namespace mctools

// end of primary_generator.cc
