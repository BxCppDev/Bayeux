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

#include <boost/algorithm/string.hpp>

#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>

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

    void primary_generator::set_run_action (mctools::g4::run_action & run_action_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _run_action_ = &run_action_;
      return;
    }

    void primary_generator::set_event_action (mctools::g4::event_action & event_action_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _event_action_ = &event_action_;
      return;
    }

    bool primary_generator::has_vertex_generator() const
    {
      return _vertex_generator_ != 0;
    }

    void primary_generator::set_vertex_generator (::genvtx::i_vertex_generator & vertex_generator_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _vertex_generator_ = &vertex_generator_;
      return;
    }

    void primary_generator::set_event_generator (::genbb::i_genbb & event_generator_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
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
      if (_initialized_) {
        reset ();
      }
      return;
    }

    void primary_generator::_check ()
    {
      if (has_vertex_generator()) {
        if (! _vertex_generator_->is_initialized ()) {
          G4Exception ("mctools::g4::primary_generator::_check",
                       "InitializationError",
                       RunMustBeAborted,
                       "Vertex generator manager is not initialized !");
        }
      } else {
        DT_LOG_WARNING(_logprio(),"No vertex generator is provided; "
                       << "we assume the vertex will be "
                       << "generated at the event generator level...");
      }
      if (! _event_generator_->is_initialized ()) {
        G4Exception ("mctools::g4::primary_generator::_check",
                     "InitializationError",
                     RunMustBeAborted,
                     "Event generator manager is not initialized !");
      }
      return;
    }

    void primary_generator::initialize (const datatools::properties & config_)
    {
      if (is_initialized ()) {
        G4Exception ("mctools::g4::primary_generator::initialized",
                     "InitializationError",
                     RunMustBeAborted,
                     "Primary_Generator already initialized !");
      }

      loggable_support::_initialize_logging_support(config_);

      // Convert particle names from genbb's scheme to Geant4 particle naming scheme :
      if (config_.has_key("particle_names_map")) {
        std::vector<std::string> names;
        config_.fetch("particle_names_map", names);
        for (int i = 0; i < names.size(); i++) {
          const std::string & mapping_entry = names[i];
          std::vector<std::string> tokens;
          boost::split (tokens, mapping_entry, boost::is_any_of("="));
          DT_THROW_IF (tokens.size() != 2, std::logic_error,
                       "Invalid particle name mapping entry '" << mapping_entry << "' !");
          std::istringstream genbb_particle_name_iss(tokens[0]);
          std::string genbb_particle_name;
          genbb_particle_name_iss >> genbb_particle_name;
          std::istringstream g4_particle_name_iss(tokens[1]);
          std::string g4_particle_name;
          g4_particle_name_iss >> g4_particle_name;
          DT_THROW_IF (genbb_particle_name.empty(), std::logic_error,
                       "Invalid genbb particle name extracted from mapping entry '" << mapping_entry << "' !");
          DT_THROW_IF (g4_particle_name.empty(), std::logic_error,
                       "Invalid Geant4 particle name extracted from mapping entry '" << mapping_entry << "' !");
          _particle_names_map_[genbb_particle_name] = g4_particle_name;
        }

        if (_particle_names_map_.size()) {
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Genbb <-> Geant4 particle name mapping : ");
          for(std::map<std::string, std::string>::const_iterator i
                = _particle_names_map_.begin();
              i !=  _particle_names_map_.end();
              i++) {
            const std::string & genbb_name = i->first;
            const std::string & g4_name = i->second;
            DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, " '" << genbb_name << "' <-> '" << g4_name << "'");
          }
        }
      }

      // checks:
      if (_run_action_ == 0) {
        G4Exception ("mctools::g4::primary_generator::initialized",
                     "InitializationError",
                      RunMustBeAborted,
                     "Missing run action !");
      }
      if (_event_action_ == 0) {
        G4Exception ("mctools::g4::primary_generator::initialized",
                     "InitializationError",
                      RunMustBeAborted,
                     "Missing event action !");
      }
      if (_event_generator_ == 0) {
        G4Exception ("mctools::g4::primary_generator::initialized",
                     "InitializationError",
                      RunMustBeAborted,
                     "Missing event generator !");
      }
      if (_vertex_generator_ == 0) {
        DT_LOG_WARNING(_logprio(),"No vertex generator is provided !");
      }
      _check ();
      int n_particle = 1;
      _particle_gun_ = new G4ParticleGun (n_particle);
      _initialized_ = true;
      return;
    }

    void primary_generator::reset ()
    {
      if (! is_initialized ()) {
        G4Exception ("mctools::g4::primary_generator::reset",
                     "InitializationError",
                      FatalException,
                     "Primary generator is not initialized!");
      }
      // destroy the gun:
      if (_particle_gun_ != 0) {
        delete _particle_gun_;
        _particle_gun_ = 0;
      }
      _run_action_ = 0;
      _event_action_ = 0;
      _event_generator_ = 0;
      _vertex_generator_ = 0;
      _set_defaults ();
      _initialized_ = false;
      return;
    }

    void primary_generator::GeneratePrimaries (G4Event * g4_event_)
    {
      if (! is_initialized ()) {
        G4Exception ("mctools::g4::primary_generator::GeneratePrimaries",
                     "InitializationError",
                      RunMustBeAborted,
                     "Primary_Generator is not initialized!");
      }

      // THIS IS NOW INVALIDATED: 2011-04-19 FM: moved these bits to 'event_action::BeginOfEventAction' :
      // Clear event data:
      // FIX BUG: 2011-05-10  FM+XG : DO CLEAR THE EVENT DATA MODEL HERE !
      // clog << "DEVEL: primary_generator::GeneratePrimaries: "
      //      << "Clear event data..." << endl;
      _event_action_->grab_event_data ().clear ();

      // Generate the vertex:
      geomtools::invalidate (_current_vertex_);
      if (_vertex_generator_ != 0) {
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
      if (! _vertex_generator_->has_next_vertex ()) {
        DT_LOG_ERROR(_logprio(),
                     "No more generated vertex from the vertex generator after "
                     << _event_counter_ << " generated events !");
        G4RunManager::GetRunManager ()->AbortRun ();
        _event_action_->set_aborted_event (true);
        return;
      }
      if (mgr.using_time_stat ()) {
        mgr.grab_CT_map ()["VG"].start ();
      }
      _vertex_generator_->shoot_vertex (mgr.grab_vg_prng(), _current_vertex_);
      if (mgr.using_time_stat ()) {
        mgr.grab_CT_map ()["VG"].stop ();
      }

      // save current event vertex:
      _event_action_->grab_event_data ().set_vertex (_current_vertex_);
      return;
    }

    void primary_generator::_generate_event (G4Event * g4_event_)
    {
      if (! _event_generator_->has_next ()) {
        DT_LOG_ERROR(_logprio(),
                     "No more generated event from the event generator after "
                     << _event_counter_ << " generated events !");
        G4RunManager::GetRunManager ()->AbortRun ();
        _event_action_->set_aborted_event (true);
        return;
      }

      ::genbb::primary_event & current_generated_event
          = _event_action_->grab_event_data ().grab_primary_event ();

      manager & mgr = _event_action_->grab_run_action ().grab_manager ();
      if (mgr.using_time_stat ()) {
        mgr.grab_CT_map ()["EG"].start ();
      }
      _event_generator_->load_next (current_generated_event);
      if (mgr.using_time_stat ()) {
        mgr.grab_CT_map ()["EG"].stop ();
      }
      current_generated_event.set_time (0.0 * CLHEP::ns);
      double event_time = current_generated_event.get_time ();
      /*
        if (devel)
        {
        current_generated_event.dump (clog,
        "Current generated event: ",
        "DEVEL: primary_generator::_generate_event: ");
        }
      */
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
           i++) {
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
        if (g4_particle_name[0] == '?') {
          std::ostringstream message;
          message << "genbb's particle type " << genbb_particle.get_type() << " (with label='"
                  << genbb_particle.get_particle_label()
                  << "') is not recognized as a Geant4 particle !";
          G4Exception ("mctools::g4::primary_generator::_generate_event",
                       "InvalidArgument",
                       RunMustBeAborted,
                       message.str ().c_str());
        }
        G4ParticleDefinition * g4_particle = particle_table->FindParticle (g4_particle_name);
        if (g4_particle == 0) {
          std::ostringstream message;
          message << "mctools::g4::primary_generator::_generate_event: "
                  << "Particle named '" << g4_particle_name << "' is not defined within the Geant4 framework !";
          G4Exception ("mctools::g4::primary_generator::_generate_event",
                       "InvalidArgument",
                       RunMustBeAborted,
                       message.str ().c_str());
        }
        double mass = g4_particle->GetPDGMass ();
        relativistic_energy = hypot (total_momentum, mass);
        kinetic_energy = relativistic_energy - mass;

        // Extract momentum:
        G4ThreeVector momentum (genbb_particle.get_momentum ().x (),
                                genbb_particle.get_momentum ().y (),
                                genbb_particle.get_momentum ().z ());
        // Plug in G4:
        if (geomtools::is_valid(_current_vertex_)) {
          // All particles originate from an unique vertex provided by the vertex generator
          _particle_gun_->SetParticlePosition (_current_vertex_);
        } else {
          if (! genbb_particle.has_vertex ()) {
            std::ostringstream message;
            message << "Particle named '" << g4_particle_name << "' has no valid vertex provided by the event generator !";
            G4Exception ("mctools::g4::primary_generator::_generate_event",
                         "InvalidArgument",
                         RunMustBeAborted,
                         message.str ().c_str());
          }
          // Each particle originate from its own vertex provided by the event generator
          _particle_gun_->SetParticlePosition (genbb_particle.get_vertex ());
        }
        _particle_gun_->SetParticleDefinition (g4_particle);
        _particle_gun_->SetParticleMomentumDirection (momentum);
        _particle_gun_->SetParticleEnergy     (kinetic_energy);
        _particle_gun_->SetParticleTime       (event_time + particle_time);
        _particle_gun_->GeneratePrimaryVertex (g4_event_);

        if (_logprio() == datatools::logger::PRIO_TRACE) {
          const string tag           = "|-- ";
          const string skip_tag      = "|   ";
          const string last_tag      = "`-- ";
          const string last_skip_tag = "    ";
          string tag1 = tag;
          string tag2 = skip_tag;
          if (particle_counter == current_generated_event.get_particles().size ()) {
            tag1 = last_tag;
            tag2 = last_skip_tag;
          }
          DT_LOG_TRACE(_logprio(), "Particle #" << particle_counter << " : ");
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
      if (p_.is_gamma ()) {
        return "gamma";
      }
      if (p_.is_positron ()) {
        return "e+";
      }
      if (p_.is_electron ()) {
        return "e-";
      }
      if (p_.is_alpha ()) {
        return "alpha";
      }
      if (p_.get_type () == ::genbb::primary_particle::PROTON) {
        return "proton";
      }
      if (p_.get_type () == ::genbb::primary_particle::NEUTRON) {
        return "neutron";
      }
      if (p_.get_type () == ::genbb::primary_particle::MUON_MINUS) {
        return "mu-";
      }
      if (p_.get_type () == ::genbb::primary_particle::MUON_PLUS) {
        return "mu+";
      }
      if (p_.get_type () == ::genbb::primary_particle::PION_0) {
        return "pi0";
      }
      if (p_.get_type () == ::genbb::primary_particle::PION_PLUS) {
        return "pi+";
      }
      if (p_.get_type () == ::genbb::primary_particle::PION_MINUS) {
        return "pi-";
      }
      const std::string & part_label = p_.get_particle_label ();

      std::map<std::string, std::string>::const_iterator found
        = _particle_names_map_.find(part_label);
      if (found != _particle_names_map_.end()) {
        return found->second;
      }
      return "?";
    }

  } // namespace g4

} // namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::primary_generator,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::primary_generator");

  // The class terse description :
  ocd_.set_class_description ("The primary generator action");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is Geant4 simulation engine embedded primary generator action. \n"
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

  {
    // Description of the 'particle_names_map' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("particle_names_map")
      .set_terse_description("Genbb to Geant4 particle names mapping")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("Genbb to Geant4 particle names lookup table. This table\n"
                            "is provided when there is no Geant4 particle associated\n"
                            "to a ``genbb`` particle type.                          \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  particle_names_map : string[3] = \\                  \n"
                            "      \" G = geantino \"           \\                  \n"
                            "      \" neutrino_muon = nu_mu \"  \\                  \n"
                            "      \" op = optical_photon \"                        \n"
                            "                                                       \n"
                            "Note that common particles like electrons, positrons,  \n"
                            "gammas... addressed by their Genbb type through the    \n"
                            "Geant3 scheme have an immediate corresponding Geant4's \n"
                            "particle.                                              \n"
                            )
      ;
  }


  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Event action logging priority                           \n"
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

// Registration macro for class 'mctools::g4::primary_generator' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::primary_generator,"mctools::g4::primary_generator")


// end of primary_generator.cc
