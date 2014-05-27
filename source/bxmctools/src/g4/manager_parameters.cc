/// \file mctools/g4/manager_parameters.cc

// Ourselves:
#include <mctools/g4/manager_parameters.h>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/random_utils.h>

// This project:
#include <mctools/utils.h>
#include <mctools/g4/manager.h>

namespace mctools {

  namespace g4 {

    manager_parameters::manager_parameters ()
    {
      set_defaults ();
      return;
    }

    void manager_parameters::reset ()
    {
      set_defaults ();
      return;
    }

    void manager_parameters::print(std::ostream & out_) const
    {
      out_ << "Geant4 simulation manager manager : " << std::endl;
      out_ << "|-- " << "interactive = " << interactive << std::endl;
      out_ << "|-- " << "g4_visu     = " << g4_visu << std::endl;
      out_ << "|-- " << "g4_macro    = '" << g4_macro << "'" << std::endl;
      out_ << "|-- " << "logging     = '" << logging<< "'" << std::endl;
      out_ << "|-- " << "dlls        = " << dlls.size() << std::endl;
      out_ << "|-- " << "manager_config_filename = '" << manager_config_filename<< "'" << std::endl;
      out_ << "|-- " << "number_of_events        = " << number_of_events << std::endl;
      out_ << "|-- " << "number_of_events_modulo = " << number_of_events_modulo << std::endl;
      out_ << "|-- " << "mgr_seed                = " << mgr_seed << std::endl;
      out_ << "|-- " << "input_prng_states_file  =' " << input_prng_states_file << "'" << std::endl;
      out_ << "|-- " << "output_prng_states_file = '" << output_prng_states_file << "'" << std::endl;
      out_ << "|-- " << "prng_states_save_modulo = " << prng_states_save_modulo << std::endl;
      out_ << "|-- " << "input_prng_seeds_file   = '" << input_prng_seeds_file << "'" << std::endl;
      out_ << "|-- " << "output_prng_seeds_file  = '" << output_prng_seeds_file << "'" << std::endl;
      out_ << "|-- " << "output_data_file        = '" << output_data_file << "'" << std::endl;
      out_ << "|-- " << "vg_name   = '" << vg_name << "'" << std::endl;
      out_ << "|-- " << "vg_seed   = " << vg_seed << std::endl;
      out_ << "|-- " << "eg_name   = '" << eg_name << "'" << std::endl;
      out_ << "|-- " << "eg_seed   = " << eg_seed << std::endl;
      out_ << "|-- " << "shpf_seed = " << shpf_seed << std::endl;
      out_ << "|-- " << "output_profiles_activation_rule = '" << output_profiles_activation_rule << "'" << std::endl;
      out_ << "|-- " << "using_time_stat     = " << using_time_stat << std::endl;
      out_ << "|-- " << "forbid_private_hits = " << forbid_private_hits << std::endl;
      out_ << "|-- " << "dont_save_no_sensitive_hit_events = " << dont_save_no_sensitive_hit_events << std::endl;
      out_ << "|-- " << "use_run_header_footer = " << use_run_header_footer << std::endl;
      out_ << "`-- " << "The end." << std::endl;
      return;
    }

    void manager_parameters::set_defaults ()
    {
      this->logging = "fatal";
      this->dlls.clear();
      this->dll_loader_config.clear();
      this->manager_config_filename.clear();
      this->interactive = false;
      this->g4_macro.clear();
      this->g4_visu = false;
      this->number_of_events = mctools::g4::manager::constants::instance ().NO_LIMIT;
      this->number_of_events_modulo = 0; // 0 == not used
      this->input_prng_states_file.clear();
      this->output_prng_states_file.clear();
      this->prng_states_save_modulo = 0; // 0 == not used
      this->input_prng_seeds_file.clear();
      this->output_prng_seeds_file.clear();
      this->output_data_file.clear();
      this->vg_name.clear();
      this->vg_seed = mygsl::random_utils::SEED_INVALID;
      this->eg_name.clear();
      this->eg_seed = mygsl::random_utils::SEED_INVALID;
      this->mgr_seed = mygsl::random_utils::SEED_INVALID;
      this->shpf_seed = mygsl::random_utils::SEED_INVALID;
      this->output_profiles_activation_rule.clear();
      this->using_time_stat = false;
      this->forbid_private_hits = false;
      this->dont_save_no_sensitive_hit_events = false;
      this->use_run_header_footer = false;
      return;
    }

    void manager_parameters::setup (const manager_parameters & a_params,
                                    manager & a_manager)
    {
      /*** Setup ***/

      DT_THROW_IF (! mygsl::seed_manager::seed_is_valid (a_params.vg_seed),
                   std::logic_error,
                   "Invalid vertex generator seed value !");
      DT_THROW_IF (! mygsl::seed_manager::seed_is_valid (a_params.eg_seed),
                   std::logic_error,
                   "Invalid event generator seed value !");
      DT_THROW_IF (! mygsl::seed_manager::seed_is_valid (a_params.mgr_seed),
                   std::logic_error,
                   "Invalid G4 generator seed value !");
      DT_THROW_IF (! mygsl::seed_manager::seed_is_valid (a_params.shpf_seed),
                   std::logic_error,
                   "Invalid SHPF generator seed value !");

      // DEVEL:
      a_params.print(std::cerr);

      datatools::logger::priority mlogprio = datatools::logger::PRIO_WARNING;
      mlogprio = datatools::logger::get_priority(a_params.logging);
      DT_THROW_IF (mlogprio == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority label '"
                   << a_params.logging << "' !");
      a_manager.set_logging_priority (mlogprio);
      a_manager.set_using_time_stat (a_params.using_time_stat);
      if (a_params.prng_states_save_modulo > 0) {
        a_manager.set_prng_state_save_modulo(a_params.prng_states_save_modulo);
      }
      if (! a_params.g4_macro.empty ()) {
        DT_LOG_NOTICE(a_manager.get_logging_priority (),
                      "Using G4 macro '"
                      << a_params.g4_macro << "'...");
        a_manager.set_g4_macro (a_params.g4_macro);
      }
      // PRNG seeding :
      if (a_params.mgr_seed != mygsl::random_utils::SEED_INVALID) {
        // register the G4 manager seed :
        a_manager.grab_seed_manager ().update_seed (mctools::g4::manager::constants::instance ().G4_MANAGER_LABEL, a_params.mgr_seed);
      }
      if (a_params.vg_seed != mygsl::random_utils::SEED_INVALID) {
        // register the vertex generator's seed :
        a_manager.grab_seed_manager ().update_seed (mctools::g4::manager::constants::instance ().VERTEX_GENERATOR_LABEL, a_params.vg_seed);
      }
      if (a_params.eg_seed != mygsl::random_utils::SEED_INVALID) {
        // register the event generator's seed :
        a_manager.grab_seed_manager ().update_seed (mctools::g4::manager::constants::instance ().EVENT_GENERATOR_LABEL, a_params.eg_seed);
      }
      if (a_params.shpf_seed != mygsl::random_utils::SEED_INVALID) {
        // register the SHPF PRNG's seed :
        a_manager.grab_seed_manager ().update_seed (mctools::g4::manager::constants::instance ().SHPF_LABEL, a_params.shpf_seed);
      }
      if (! a_params.input_prng_seeds_file.empty ()) {
        a_manager.set_input_prng_seeds_file (a_params.input_prng_seeds_file);
      }
      if (! a_params.output_prng_seeds_file.empty ()) {
        a_manager.set_output_prng_seeds_file (a_params.output_prng_seeds_file);
      }
      if (! a_params.input_prng_states_file.empty ()) {
        a_manager.set_input_prng_states_file (a_params.input_prng_states_file);
      }
      if (! a_params.output_prng_states_file.empty ()) {
        a_manager.set_output_prng_states_file (a_params.output_prng_states_file);
      }
      if (! a_params.output_data_file.empty ()) {
        std::string output_data_file = a_params.output_data_file;
        datatools::fetch_path_with_env (output_data_file);
        a_manager.set_output_data_file (output_data_file);
      }
      if (! a_params.vg_name.empty ()) {
        a_manager.set_vertex_generator_name (a_params.vg_name);
      }
      if (! a_params.eg_name.empty ()) {
        a_manager.set_event_generator_name (a_params.eg_name);
      }
      if (a_params.number_of_events <= mctools::g4::manager::constants::instance ().NO_LIMIT) {
        a_manager.set_number_of_events (a_params.number_of_events);
      }

      //if (a_params.number_of_events_modulo >= NUMBER_OF_EVENTS_MODULO_NONE) {
      a_manager.set_number_of_events_modulo (a_params.number_of_events_modulo);
      //}

      // Support for output profiles:
      if (! a_params.output_profiles_activation_rule.empty ()) {
        // Defered activation rule for output profiles
        a_manager.set_output_profiles_activation_rule(a_params.output_profiles_activation_rule);
      }

      if (a_params.forbid_private_hits) {
        a_manager.set_forbid_private_hits (true);
      }
      if (a_params.dont_save_no_sensitive_hit_events) {
        a_manager.set_dont_save_no_sensitive_hit_events (true);
      }
      a_manager.set_use_run_header_footer (a_params.use_run_header_footer);
      a_manager.set_interactive (a_params.interactive);
      a_manager.set_g4_visualization (a_params.g4_visu);

      //if (a_params.debug) a_manager.dump_base (clog, "mctools::g4::manager: ", "DEBUG: ");

      /*** Configuration file ***/

      DT_THROW_IF (a_params.manager_config_filename.empty (),
                   std::logic_error,
                   "Missing configuration filename for the simulation manager !");
      std::string manager_config_filename = a_params.manager_config_filename;
      datatools::fetch_path_with_env (manager_config_filename);
      DT_THROW_IF (! boost::filesystem::exists (manager_config_filename),
                   std::runtime_error,
                   "Simulation manager configuration filename '"
                   << manager_config_filename << "' does not exist ! ");
      datatools::multi_properties the_configuration ("name",
                                                     "",
                                                     "Configuration for the 'g4' simulation manager");
      the_configuration.read (manager_config_filename);

      /*** Initialization ***/

      DT_LOG_NOTICE(a_manager.get_logging_priority (),
                    "Initializing the simulation manager...");
      a_manager.initialize (the_configuration);
      DT_LOG_NOTICE(a_manager.get_logging_priority (),
                    "Simulation manager has been configured and initialized.");
      return;
    }

  } // end of namespace g4

} // end of namespace mctools
