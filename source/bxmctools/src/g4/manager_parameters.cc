// -*- mode: c++ ; -*-
/* manager_parameters.cc
 */

#include <mctools/g4/manager_parameters.h>
#include <mctools/g4/manager.h>

#include <boost/filesystem.hpp>

#include <mctools/utils.h>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

#include <mygsl/random_utils.h>

namespace mctools {

  namespace g4 {

    // ctor:
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

    void manager_parameters::set_defaults ()
    {
      this->logging                 = "warning";
      this->manager_config_filename = "";
      this->interactive             = false;
      this->g4_macro                = "";
      this->g4_visu                 = false;
      this->number_of_events        = mctools::g4::manager::constants::instance ().NO_LIMIT;
      this->number_of_events_modulo = 0; // 0 == not used
      this->input_prng_states_file  = "";
      this->output_prng_states_file = "";
      this->prng_states_save_modulo = 0; // 0 == not used
      this->input_prng_seeds_file   = "";
      this->output_prng_seeds_file  = "";
      this->output_data_file        = "";
      this->vg_name                 = "";
      this->vg_seed                 = mygsl::random_utils::SEED_INVALID;
      this->eg_name                 = "";
      this->eg_seed                 = mygsl::random_utils::SEED_INVALID;
      this->mgr_seed                = mygsl::random_utils::SEED_INVALID;
      this->shpf_seed               = mygsl::random_utils::SEED_INVALID;
      this->using_time_stat                   = false;
      this->forbid_private_hits               = false;
      this->dont_save_no_sensitive_hit_events = false;
      this->use_run_header_footer             = false;
      return;
    }

    void manager_parameters::print_examples (std::ostream & a_out,
                                             const std::string & a_name,
                                             const std::string & a_title)
    {
      if (! a_title.empty ()) {
        a_out << a_title << std::endl;
      }
      a_out << std::endl;
      a_out << " 1) Interactive test:" << std::endl;
      a_out << "    " << a_name << " \\" << std::endl;
      a_out << "         --interactive \\" << std::endl;
      a_out << "         --g4-visu \\" << std::endl;
      a_out << "         --vertex-generator-name \"source_bulk.vg\"  \\" << std::endl;
      a_out << "         --vertex-generator-seed 0 \\" << std::endl;
      a_out << "         --event-generator-name \"K40\" \\" << std::endl;
      a_out << "         --event-generator-seed 0  \\" << std::endl;
      a_out << "         --g4-manager-seed 0       \\" << std::endl;
      a_out << "         --shpf-seed 0             \\" << std::endl;
      a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << std::endl;
      a_out << "         --output-prng-states-file mc_g4_production.states \\" << std::endl;
      a_out << "         --config ${CONFIG_DIR}/simulation/manager.conf \\" << std::endl;
      a_out << "                  ${CONFIG_DIR}/simulation/geant4_visualization.macro" << std::endl;
      a_out << std::endl;
      a_out << " 2) Run the simulation in batch mode:" << std::endl;
      a_out << "    " << a_name << " \\" << std::endl;
      a_out << "         --batch                  \\" << std::endl;
      a_out << "         --using-time-statistics  \\" << std::endl;
      a_out << "         --vertex-generator-name \"source_bulk.vg\"  \\" << std::endl;
      a_out << "         --vertex-generator-seed 0 \\" << std::endl;
      a_out << "         --event-generator-name \"K40\"  \\" << std::endl;
      a_out << "         --event-generator-seed 0 \\" << std::endl;
      a_out << "         --g4-manager-seed 0      \\" << std::endl;
      a_out << "         --shpf-seed 0            \\" << std::endl;
      a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << std::endl;
      a_out << "         --output-prng-states-file mc_g4_production.states \\" << std::endl;
      a_out << "         --number-of-events 1     \\" << std::endl;
      a_out << "         --config ${CONFIG_DIR}/simulation/manager.conf \\" << std::endl;
      a_out << "         --output-data-file mc_g4_sample_0.xml \\" << std::endl;
      a_out << "         > g4_production.log 2>&1" << std::endl;
      a_out << std::endl;
      a_out << std::endl;

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

      datatools::logger::priority mlogprio = datatools::logger::PRIO_WARNING;
      mlogprio = datatools::logger::get_priority(a_params.logging);
      DT_THROW_IF (mlogprio == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority label '"
                   << a_params.logging << "' !");
      a_manager.set_logging_priority (mlogprio);
      a_manager.set_using_time_stat (a_params.using_time_stat);
      if (a_params.prng_states_save_modulo > 0) {
        a_manager.set_prng_state_save_modulo (a_params.prng_states_save_modulo);
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

// end of manager_parameters.cc
