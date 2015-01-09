// primary_generator.cc

// Ourselves:
#include <mctools/g4/primary_generator.h>

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <cmath>
#include <fstream>
#include <string>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/genbb_help:
#include <genbb_help/i_genbb.h>
// - Bayeux/genvtx:
#include <genvtx/i_vertex_generator.h>

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

// This project:
#include <mctools/g4/manager.h>
#include <mctools/g4/run_action.h>
#include <mctools/g4/event_action.h>
#include <mctools/biasing/primary_event_bias.h>

namespace mctools {

  namespace g4 {

    bool primary_generator::is_initialized() const
    {
      return _initialized_;
    }

    void primary_generator::set_run_action(mctools::g4::run_action & run_action_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _run_action_ = &run_action_;
      return;
    }

    void primary_generator::set_event_action(mctools::g4::event_action & event_action_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _event_action_ = &event_action_;
      return;
    }

    bool primary_generator::has_vertex_generator() const
    {
      return _vertex_generator_ != 0;
    }

    void primary_generator::set_vertex_generator(::genvtx::i_vertex_generator & vertex_generator_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _vertex_generator_ = &vertex_generator_;
      return;
    }

    void primary_generator::set_event_generator(::genbb::i_genbb & event_generator_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _event_generator_ = &event_generator_;
      return;
    }

    size_t primary_generator::get_event_counter() const
    {
      return _event_counter_;
    }

    void primary_generator::reset_event_counter()
    {
      _event_counter_ = 0;
      return;
    }

    void primary_generator::_set_defaults()
    {
      reset_event_counter();
      return;
    }

    primary_generator::primary_generator()
    {
      _initialized_ = false;
      _run_action_ = 0;
      _event_action_ = 0;
      _vertex_generator_ = 0;
      _event_generator_ = 0;
      _particle_gun_ = 0;
      _event_counter_ = 0;
      _set_defaults();
      return;
    }

    primary_generator::~primary_generator()
    {
      if (_initialized_) {
        reset();
      }
      return;
    }

    void primary_generator::_check()
    {
      if (has_vertex_generator()) {
        if (! _vertex_generator_->is_initialized()) {
          G4Exception("mctools::g4::primary_generator::_check",
                      "InitializationError",
                      RunMustBeAborted,
                      "Vertex generator manager is not initialized !");
        }
      } else {
        DT_LOG_WARNING(_logprio(),"No vertex generator is provided; "
                       << "we assume the vertex will be "
                       << "generated at the event generator level...");
      }
      if (! _event_generator_->is_initialized()) {
        G4Exception("mctools::g4::primary_generator::_check",
                    "InitializationError",
                    RunMustBeAborted,
                    "Event generator manager is not initialized !");
      }
      return;
    }

    void primary_generator::initialize(const datatools::properties & config_)
    {
      if (is_initialized()) {
        G4Exception("mctools::g4::primary_generator::initialize",
                    "InitializationError",
                    RunMustBeAborted,
                    "Primary_Generator already initialized !");
      }

      loggable_support::_initialize_logging_support(config_);

      // Convert particle names from genbb's scheme to Geant4 particle naming scheme :
      if (config_.has_key("particle_names_map")) {
        std::vector<std::string> names;
        config_.fetch("particle_names_map", names);
        for (size_t i = 0; i < names.size(); i++) {
          const std::string & mapping_entry = names[i];
          std::vector<std::string> tokens;
          boost::split (tokens, mapping_entry, boost::is_any_of("="));
          DT_THROW_IF(tokens.size() != 2, std::logic_error,
                      "Invalid particle name mapping entry '" << mapping_entry << "' !");
          std::istringstream genbb_particle_name_iss(tokens[0]);
          std::string genbb_particle_name;
          genbb_particle_name_iss >> genbb_particle_name;
          std::istringstream g4_particle_name_iss(tokens[1]);
          std::string g4_particle_name;
          g4_particle_name_iss >> g4_particle_name;
          DT_THROW_IF(genbb_particle_name.empty(), std::logic_error,
                      "Invalid genbb particle name extracted from mapping entry '" << mapping_entry << "' !");
          DT_THROW_IF(g4_particle_name.empty(), std::logic_error,
                      "Invalid Geant4 particle name extracted from mapping entry '" << mapping_entry << "' !");
          _particle_names_map_[genbb_particle_name] = g4_particle_name;
        }

        if (!_particle_names_map_.empty()) {
          DT_LOG_NOTICE(_logprio(), "Genbb <-> Geant4 particle name mapping : ");
          for(std::map<std::string, std::string>::const_iterator i
                = _particle_names_map_.begin();
              i !=  _particle_names_map_.end();
              i++) {
            const std::string & genbb_name = i->first;
            const std::string & g4_name = i->second;
            DT_LOG_NOTICE(_logprio(), " '" << genbb_name << "' <-> '" << g4_name << "'");
          }
        }
      }

      // Checks:
      if (_run_action_ == 0) {
        G4Exception ("mctools::g4::primary_generator::initialize",
                     "InitializationError",
                     RunMustBeAborted,
                     "Missing run action !");
      }

      // Activate primary event biasing:
      if (config_.has_key("using_bias")) {
        bool use_bias = config_.fetch_boolean("using_bias");
        if (use_bias) {
          datatools::properties bias_config;
          config_.export_and_rename_starting_with(bias_config, "bias.", "");
          _bias_.reset(new mctools::biasing::primary_event_bias);
          _bias_->set_geometry_manager(_run_action_->get_manager().get_geom_manager());
          _bias_->initialize(bias_config);
        }
      }

      // Checks:
      if (_event_action_ == 0) {
        G4Exception("mctools::g4::primary_generator::initialize",
                    "InitializationError",
                    RunMustBeAborted,
                    "Missing event action !");
      }
      if (_event_generator_ == 0) {
        G4Exception("mctools::g4::primary_generator::initialize",
                    "InitializationError",
                    RunMustBeAborted,
                    "Missing event generator !");
      }
      if (_vertex_generator_ == 0) {
        DT_LOG_WARNING(_logprio(),"No vertex generator is provided !");
      }
      _check();
      const int n_particle = 1;
      _particle_gun_ = new G4ParticleGun(n_particle);

      _initialized_ = true;
      return;
    }

    void primary_generator::reset()
    {
      if (! is_initialized()) {
        G4Exception("mctools::g4::primary_generator::reset",
                    "InitializationError",
                    FatalException,
                    "Primary generator is not initialized!");
      }
      if (_bias_) {
        if (_bias_->is_initialized()) {
          _bias_->reset();
        }
        _bias_.reset();
      }

      // Destroy the gun:
      if (_particle_gun_ != 0) {
        delete _particle_gun_;
        _particle_gun_ = 0;
      }
      _run_action_ = 0;
      _event_action_ = 0;
      _event_generator_ = 0;
      _vertex_generator_ = 0;
      _set_defaults();
      _initialized_ = false;
      return;
    }

    void primary_generator::GeneratePrimaries(G4Event * g4_event_)
    {
      if (! is_initialized()) {
        G4Exception("mctools::g4::primary_generator::GeneratePrimaries",
                    "InitializationError",
                    RunMustBeAborted,
                    "Primary_Generator is not initialized!");
      }

      // THIS IS NOW INVALIDATED: 2011-04-19 FM: moved these bits to 'event_action::BeginOfEventAction' :
      // Clear event data:
      // FIX BUG: 2011-05-10  FM+XG : DO CLEAR THE EVENT DATA MODEL HERE !
      // clog << "DEVEL: primary_generator::GeneratePrimaries: "
      //      << "Clear event data..." << endl;
      _event_action_->grab_event_data().clear();
      _event_action_->set_aborted_event(false);
      _event_action_->set_killed_event(false);

      // Generate the vertex:
      geomtools::invalidate(_current_vertex_);
      if (_vertex_generator_ != 0) {
        _generate_vertex();
      }
      // Generate the primary event:
      _generate_event(g4_event_);
      // Increment the event counter:
      _event_counter_++;
      return;
    }

    void primary_generator::_generate_vertex()
    {
      // Invoke the vertex generator:
      manager & mgr = _event_action_->grab_run_action().grab_manager();
      if (! _vertex_generator_->has_next_vertex()) {
        DT_LOG_ERROR(_logprio(),
                     "No more generated vertex from the vertex generator after "
                     << _event_counter_ << " generated events !");
        _event_action_->set_aborted_event(true);
        G4RunManager::GetRunManager()->AbortRun();
        return;
      }
      if (mgr.using_time_stat()) {
        mgr.grab_CT_map()["VG"].start();
      }
      _vertex_generator_->shoot_vertex(mgr.grab_vg_prng(), _current_vertex_);
      if (mgr.using_time_stat()) {
        mgr.grab_CT_map()["VG"].stop();
      }

      // Save current event vertex:
      _event_action_->grab_event_data().set_vertex(_current_vertex_);
      return;
    }

    void primary_generator::_generate_event(G4Event * g4_event_)
    {
      if (! _event_generator_->has_next()) {
        DT_LOG_ERROR(_logprio(),
                     "No more generated event from the event generator after "
                     << _event_counter_ << " generated events !");
        G4RunManager::GetRunManager()->AbortRun();
        _event_action_->set_aborted_event(true);
        return;
      }

      ::genbb::primary_event & current_generated_event
          = _event_action_->grab_event_data().grab_primary_event();

      manager & mgr = _event_action_->grab_run_action ().grab_manager();
      if (mgr.using_time_stat()) {
        mgr.grab_CT_map()["EG"].start();
      }
      _event_generator_->load_next(current_generated_event);
      if (mgr.using_time_stat()) {
        mgr.grab_CT_map()["EG"].stop();
      }
      current_generated_event.set_time(0.0 * CLHEP::ns);

      if (_bias_) {
        // Here the bias object is processing the primary event:
        mctools::biasing::primary_event_bias::biasing_info bi;
        if (mgr.using_time_stat()) {
          mgr.grab_CT_map()["BPE"].start();
        }
        _bias_->process(_current_vertex_, current_generated_event, bi);
        if (bi.is_killed()) {
          DT_LOG_TRACE(_logprio(), "Event is killed by the primary event biasing algorithm!");
          _event_action_->set_killed_event(true);
        }
        if (mgr.using_time_stat()) {
          mgr.grab_CT_map()["BPE"].stop();
        }
      }

      // Should we check the validity of the primary event here ?

      if (_event_action_->is_aborted_event()) {
        return;
      }

      if (_event_action_->is_killed_event()) {
        return;
      }

      // Process the primary event:
      // about to insert primary particles in the G4 particle gun.
      double event_time = current_generated_event.get_time();

      G4ParticleTable * particle_table = G4ParticleTable::GetParticleTable();

      // Loop on particles:
      size_t particle_counter = 0;
      for (::genbb::primary_event::particles_col_type::const_iterator i
             = current_generated_event.get_particles().begin();
           i != current_generated_event.get_particles().end();
           i++, particle_counter++) {
        const ::genbb::primary_particle & genbb_particle = *i;
        if (genbb_particle.get_auxiliaries().has_flag(mctools::biasing::primary_event_bias::dont_track_this_particle_flag())) {
          // After some 'primary event bias', we don't track this primary particle:
          continue;
        }
        const std::string genbb_particle_label = genbb_particle.get_particle_label();

        /* Note:
         * G4 particle not implemented:
         *  "nu_e" "nu_mu" "nu_tau"
         *  "anti_nu_e" "anti_nu_mu" "anti_nu_tau"
         *  "opticalphoton"
         *  ...
         */
        G4String g4_particle_name; // = get_g4_particle_name_from_genbb_particle(genbb_particle);
        G4ParticleDefinition * g4_particle = 0;

        double particle_mass = datatools::invalid_real();
        if (genbb_particle.mass_is_known()) {
          particle_mass = genbb_particle.get_mass();
        }

        if (genbb_particle.has_pdg_code()) {
          // Support for particle PDG encoding: NOT USABLE YET FOR NOW BECAUSE THIS INTERFACE
          // NEEDS MORE WORKS, PARTICULARLY FOR PARTIALLY IONIZED IONS.
          g4_particle = particle_table->FindParticle(genbb_particle.get_pdg_code());
          if (g4_particle == 0) {
            std::ostringstream message;
            message << "genbb's nucleus particle with PDG code='"
                    << genbb_particle.get_pdg_code()
                    << "') cannot not be processed as a Geant4 particle !";
            G4Exception("mctools::g4::primary_generator::_generate_event",
                        "InvalidArgument",
                        RunMustBeAborted,
                        message.str().c_str());
          }
          g4_particle_name = g4_particle->GetParticleName();
          _particle_gun_->SetParticleDefinition(g4_particle);
          DT_LOG_TRACE(_logprio(), "Found a PDG particle: " << g4_particle_name
                       << " with G4 PDG mass = " << g4_particle->GetPDGMass() / CLHEP::MeV << " MeV "
                       << " and PDG charge = " << g4_particle->GetPDGCharge()
                       );
        } else if (genbb_particle.has_type()) {
          // Support for traditional particle 'types' from genbb_help (extended Geant3 codes):
          if (genbb_particle.is_nucleus()) {
            /*
            // Typical Support for radioactive ion (from G4 examples):
            G4int Z = 10, A = 24;
            G4double ionCharge   = 0.*eplus;
            G4double excitEnergy = 0.*keV;
            G4ParticleDefinition* ion
            = G4ParticleTable::GetParticleTable()->GetIon(Z,A,excitEnergy);
            fParticleGun->SetParticleDefinition(ion);
            fParticleGun->SetParticleCharge(ionCharge);
            */
            int Z = -1;
            int A =  0;
            double excitation_energy = 0.0;
            // Fetch the characteristics of the nucleus from the GENBB particle label:
            bool nucleus_parsing =
              genbb::primary_particle::label_to_nucleus(genbb_particle_label,
                                                        Z,
                                                        A,
                                                        excitation_energy);
            if (! nucleus_parsing) {
              std::ostringstream message;
              message << "genbb's nucleus particle with label='"
                      << genbb_particle_label
                      << "') cannot not be processed as a Geant4 particle !";
              G4Exception("mctools::g4::primary_generator::_generate_event",
                          "InvalidArgument",
                          RunMustBeAborted,
                          message.str().c_str());
            }
            // Instantiate the G4 corresponding ion:
            g4_particle =
              G4ParticleTable::GetParticleTable()->GetIon(Z, A, excitation_energy);
            g4_particle_name = g4_particle->GetParticleName();
            _particle_gun_->SetParticleDefinition(g4_particle);
            DT_LOG_TRACE(_logprio(), "Found a nucleus: " << g4_particle_name
                         << " with G4 PDG mass = " << g4_particle->GetPDGMass() / CLHEP::MeV << " MeV "
                         << " (GENBB mass = " << particle_mass / CLHEP::MeV << " MeV)"
                         << " and PDG charge = " << g4_particle->GetPDGCharge()
                         << " (charge set in the gun = " << _particle_gun_->GetParticleCharge() / CLHEP::eplus << " e)"
                         );
          } else if (genbb_particle.is_ion()) {
            int Z = -1;
            int A =  0;
            double excitation_energy = 0.0;
            int ion_charge = 0;
            // Fetch the characteristics of the ion (partially ionized) from the GENBB particle label:
            bool ion_parsing =
              genbb::primary_particle::label_to_ion(genbb_particle_label,
                                                    Z,
                                                    A,
                                                    excitation_energy,
                                                    ion_charge);
            if (!ion_parsing) {
              std::ostringstream message;
              message << "genbb's ion particle with label='"
                      << genbb_particle_label
                      << "') cannot not be processed as a Geant4 particle !";
              G4Exception("mctools::g4::primary_generator::_generate_event",
                          "InvalidArgument",
                          RunMustBeAborted,
                          message.str().c_str());
            }
            g4_particle =
              G4ParticleTable::GetParticleTable()->GetIon(Z, A, excitation_energy);
            g4_particle_name = g4_particle->GetParticleName();
            _particle_gun_->SetParticleDefinition(g4_particle);
            // QUESTION: IS IT THE RIGHT WAY TO SET THE EFFECTIVE ION CHARGE ???
            _particle_gun_->SetParticleCharge(ion_charge * CLHEP::eplus);
            DT_LOG_TRACE(_logprio(), "Found an ion: " << g4_particle_name
                         << " with G4 PDG mass = " << g4_particle->GetPDGMass() / CLHEP::MeV << " MeV "
                         << " (GENBB mass = " << particle_mass / CLHEP::MeV << " MeV)"
                         << " and PDG charge = " << g4_particle->GetPDGCharge()
                         << " (GENBB charge set in the gun = " << _particle_gun_->GetParticleCharge() / CLHEP::eplus << " e)"
                         );
          } else {
            // Fetch the G4 particle name from the traditional GENBB particle:
            g4_particle_name = get_g4_particle_name_from_genbb_particle(genbb_particle);
            if (g4_particle_name[0] == '?') {
              std::ostringstream message;
              message << "genbb's particle type " << genbb_particle.get_type() << " (with label='"
                      << genbb_particle_label
                      << "') is not recognized as a Geant4 particle !";
              G4Exception ("mctools::g4::primary_generator::_generate_event",
                           "InvalidArgument",
                           RunMustBeAborted,
                           message.str().c_str());
            }
            // Make it a G4 particle:
            g4_particle = particle_table->FindParticle(g4_particle_name);
            if (g4_particle == 0) {
              std::ostringstream message;
              message << "mctools::g4::primary_generator::_generate_event: "
                      << "Particle named '" << g4_particle_name << "' is not defined within the Geant4 framework !";
              G4Exception ("mctools::g4::primary_generator::_generate_event",
                           "InvalidArgument",
                           RunMustBeAborted,
                           message.str().c_str());
            }
            _particle_gun_->SetParticleDefinition(g4_particle);
            g4_particle_name = g4_particle->GetParticleName();
            DT_LOG_TRACE(_logprio(), "Found the Geant4 '" << g4_particle_name << "' particle from the GENBB particle"
                         << " with G4 PDG mass = " << g4_particle->GetPDGMass() / CLHEP::MeV << " MeV "
                         << " and PDG charge = " << g4_particle->GetPDGCharge());
          }
        }

        // Plug the primary particle's vertex in the G4 particle gun:
        if (geomtools::is_valid(_current_vertex_)) {
          // All particles originate from an unique vertex provided by the vertex generator:
          _particle_gun_->SetParticlePosition(_current_vertex_);
        } else {
          // Search for a specific vertex associated to any individual particle:
          if (! genbb_particle.has_vertex()) {
            std::ostringstream message;
            message << "Particle named '" << g4_particle_name << "' has no valid vertex provided by the event generator !";
            G4Exception("mctools::g4::primary_generator::_generate_event",
                        "InvalidArgument",
                        RunMustBeAborted,
                        message.str().c_str());
          }
          // Each particle originates from its own vertex provided by the event generator:
          _particle_gun_->SetParticlePosition(genbb_particle.get_vertex());
        }

        // Fetch the mass of the particle from PDG data if mass is not known yet:
        if (! datatools::is_valid(particle_mass)) {
          particle_mass = g4_particle->GetPDGMass();
        }

        // For ultra low energy particles (i.e. UCN): may be we should use
        // other formulas (non relativistic approximation) because of roundoff errors in
        // relativistic formula (is G4 aware of that? to be checked in the G4 code)...
        const double total_momentum      = genbb_particle.get_momentum().mag();
        const double relativistic_energy = hypot(total_momentum, particle_mass);
        const double kinetic_energy      = relativistic_energy - particle_mass;
        if (! datatools::is_valid(kinetic_energy)) {
          std::ostringstream message;
          message << "Particle named '" << g4_particle_name << "' has no valid kinetic energy provided by the event generator !";
          G4Exception("mctools::g4::primary_generator::_generate_event",
                      "InvalidArgument",
                      RunMustBeAborted,
                      message.str().c_str());
        }

        // _particle_gun_->SetParticleMomentum(momentum);
        // Get back to kinetic energy calculation since Geant4 does the same
        // calculation. The following command sequence is the only one that
        // does not generate any message output from Geant4 (when setting
        // ParticleMomentum, the kinetic energy is recalculated and dump message
        // and by setting ParticleEnergy, the momentum is recalculated and then
        // dump message again).
        // Extract momentum from the GENBB particle:
        G4ThreeVector momentum(genbb_particle.get_momentum().x(),
                               genbb_particle.get_momentum().y(),
                               genbb_particle.get_momentum().z());
        _particle_gun_->SetParticleMomentumDirection(momentum);
        _particle_gun_->SetParticleEnergy(kinetic_energy);

        const double particle_time  = genbb_particle.get_time();
        _particle_gun_->SetParticleTime(event_time + particle_time);
        // And finaly, we fill the Geant4 primary event:
        _particle_gun_->GeneratePrimaryVertex(g4_event_);

        if (_logprio() == datatools::logger::PRIO_TRACE) {
          const std::string tag           = datatools::i_tree_dumpable::tags::item();
          const std::string skip_tag      = datatools::i_tree_dumpable::tags::skip_item();
          const std::string last_tag      = datatools::i_tree_dumpable::tags::last_item();
          const std::string last_skip_tag = datatools::i_tree_dumpable::tags::last_skip_item();
          std::string tag1 = tag;
          std::string tag2 = skip_tag;
          if (particle_counter == current_generated_event.get_particles().size()) {
            tag1 = last_tag;
            tag2 = last_skip_tag;
          }
          DT_LOG_TRACE(_logprio(), "Particle #" << particle_counter << " : ");
          std::clog << tag2 << tag      << "Type       = "
                    << genbb_particle.get_type() << " ('" << genbb_particle_label << "')"
                    << std::endl;
          std::clog << tag2 << tag      << "PDG code   = "
                    << genbb_particle.get_pdg_code()
                    << std::endl;
          std::clog << tag2 << tag      << "Geant4 name  = "
                    << g4_particle_name
                    << std::endl;
           std::clog << tag2 << tag      << "Time       = "
                    << _particle_gun_->GetParticleTime() / CLHEP::ns << " ns"
                    << std::endl;
          std::clog << tag2 << tag      << "Position   = " << _current_vertex_ / CLHEP::mm << " mm" << std::endl;
          std::clog << tag2 << tag << "Momentum   = " << genbb_particle.get_momentum() / CLHEP::keV << " keV" << std::endl;
          if (datatools::is_valid(kinetic_energy)) {
            std::clog << tag2 << last_tag    << "Energy     = " << kinetic_energy / CLHEP::keV << " keV (FYI)" << std::endl;
          }
        }
      } // Particle loop

      return;
    }

    std::string primary_generator::get_g4_particle_name_from_genbb_particle(const ::genbb::primary_particle & p_) const
    {
      if (p_.is_gamma()) {
        return "gamma";
      }
      if (p_.is_positron()) {
        return "e+";
      }
      if (p_.is_electron()) {
        return "e-";
      }
      if (p_.is_alpha()) {
        return "alpha";
      }
      if (p_.get_type() == ::genbb::primary_particle::PROTON) {
        return "proton";
      }
      if (p_.get_type() == ::genbb::primary_particle::NEUTRON) {
        return "neutron";
      }
      if (p_.get_type() == ::genbb::primary_particle::MUON_MINUS) {
        return "mu-";
      }
      if (p_.get_type() == ::genbb::primary_particle::MUON_PLUS) {
        return "mu+";
      }
      if (p_.get_type() == ::genbb::primary_particle::PION_0) {
        return "pi0";
      }
      if (p_.get_type() == ::genbb::primary_particle::PION_PLUS) {
        return "pi+";
      }
      if (p_.get_type() == ::genbb::primary_particle::PION_MINUS) {
        return "pi-";
      }
      if (p_.is_nucleus()) {
        // What to do here ? shoud we support it from this method
        // return "ElA[E*]"; // G4 syntax seems to be : He6[0.0]
        return "?"; // FOR NOW THIS METHOD DOES NOT WORK FOR NUCLEI
      }
      if (p_.is_ion()) {
        // What to do here ? shoud we support it from this method
        // return "ElA[E*]"; //  G4 syntax seems to be : He6[0.0] but there is no way to set the ion charge
        return "?"; // FOR NOW THIS METHOD DOES NOT WORK FOR ION
      }

      // If no GENBB supported particle is found, we try to decode
      // the GENBB particle label:
      const std::string & part_label = p_.get_particle_label();
      // and use a lookup table kindly provided by the user:
      std::map<std::string, std::string>::const_iterator found
        = _particle_names_map_.find(part_label);
      if (found != _particle_names_map_.end()) {
        return found->second;
      }
      return "?"; // This means that no G4 particle name has been found.
    }

  } // namespace g4

} // namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::primary_generator,ocd_)
{
  // The class name :
  ocd_.set_class_name("mctools::g4::primary_generator");

  // The class terse description :
  ocd_.set_class_description("The primary generator action");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is Geant4 simulation engine embedded primary generator action. \n"
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
                               " #@description Set the logging priority                           \n"
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

// Registration macro :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::primary_generator,
                               "mctools::g4::primary_generator")
