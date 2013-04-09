// -*- mode: c++ ; -*-
/* manager_parameters.cc
 */

#include <mctools/g4/manager_parameters.h>
#include <mctools/g4/manager.h>

#include <boost/filesystem.hpp>

#include <mctools/utils.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <mygsl/random_utils.h>

namespace mctools {

  namespace g4 {

    using namespace std;

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
      this->debug                   = false;
      this->verbose                 = false;
      this->manager_config_filename = "";
      this->g4_macro                = "";
      this->g4_visu                 = false;
      this->interactive             = false;
      this->noevents                = mctools::g4::manager::constants::instance ().NO_LIMIT;
      this->noevents_modulo         = 0; // not used
      this->input_prng_states_file  = "";
      this->output_prng_states_file = "";
      this->prng_states_save_modulo = 0; // not used
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

    void manager_parameters::print_examples (ostream & a_out,
                                             const string & a_name,
                                             const string & a_title)
    {
      if (! a_title.empty ()) {
        a_out << a_title << endl;
      }
      a_out << endl;
      a_out << " 1) Interactive test:" << endl;
      a_out << "    " << a_name << " \\" << endl;
      a_out << "         --interactive \\" << endl;
      a_out << "         --g4-visu \\" << endl;
      a_out << "         --vertex-generator-name \"all_source_bulk.vg\"  \\" << endl;
      a_out << "         --vertex-generator-seed 0 \\" << endl;
      a_out << "         --event-generator-name \"bkg_K40\" \\" << endl;
      a_out << "         --event-generator-seed 0  \\" << endl;
      a_out << "         --g4-manager-seed 0       \\" << endl;
      a_out << "         --shpf-seed 0             \\" << endl;
      a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << endl;
      a_out << "         --output-prng-states-file mc_g4_production.states \\" << endl;
      a_out << "         --config ${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/manager.conf \\" << endl;
      a_out << "                  ${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/g4vis.mac" << endl;
      a_out << endl;
      a_out << " 2) Run the simulation in batch mode:" << endl;
      a_out << "    " << a_name << " \\" << endl;
      a_out << "         --batch                  \\" << endl;
      a_out << "         --using-time-statistics  \\" << endl;
      a_out << "         --vertex-generator-name \"all_source_bulk.vg\"  \\" << endl;
      a_out << "         --vertex-generator-seed 0 \\" << endl;
      a_out << "         --event-generator-name \"bkg_K40\"  \\" << endl;
      a_out << "         --event-generator-seed 0 \\" << endl;
      a_out << "         --g4-manager-seed 0      \\" << endl;
      a_out << "         --shpf-seed 0            \\" << endl;
      a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << endl;
      a_out << "         --output-prng-states-file mc_g4_production.states \\" << endl;
      a_out << "         --number-of-events 1     \\" << endl;
      a_out << "         --config ${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/manager.conf \\" << endl;
      a_out << "         --output-data-file mc_g4_sample_0.xml \\" << endl;
      a_out << "         > mc_g4_production.log 2>&1" << endl;
      a_out << endl;
      a_out << " See README for other running examples" << endl;
      a_out << endl;

      return;
    }

    void manager_parameters::setup (const manager_parameters & a_params,
                                    manager & a_manager)
    {
      /*** Setup ***/

      if (! mygsl::seed_manager::seed_is_valid (a_params.vg_seed)) {
        throw logic_error ("mctools::g4::manager_parameters::setup: Invalid vertex generator seed value !");
      }

      if (! mygsl::seed_manager::seed_is_valid (a_params.eg_seed)) {
        throw logic_error ("mctools::g4::manager_parameters::setup: Invalid event generator seed value !");
      }

      if (! mygsl::seed_manager::seed_is_valid (a_params.mgr_seed)) {
        throw logic_error ("mctools::g4::manager_parameters::setup: Invalid G4 generator seed value !");
      }
      if (! mygsl::seed_manager::seed_is_valid (a_params.shpf_seed)) {
        throw logic_error ("mctools::g4::manager_parameters::setup: Invalid SHPF generator seed value !");
      }

      a_manager.set_debug (a_params.debug);
      a_manager.set_verbose (a_params.verbose);
      a_manager.set_using_time_stat (a_params.using_time_stat);
      if (a_params.prng_states_save_modulo > 0) {
        a_manager.set_prng_state_save_modulo (a_params.prng_states_save_modulo);
      }
      if (! a_params.g4_macro.empty ()) {
        clog << datatools::io::notice
             << "g4_production: "
             << "Using G4 macro '"
             << a_params.g4_macro << "'..." << endl;
        a_manager.set_g4_macro (a_params.g4_macro);
      }
      // PRNG seeding :
      if (a_params.mgr_seed != mygsl::random_utils::SEED_INVALID)
        {
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
        string output_data_file = a_params.output_data_file;
        datatools::fetch_path_with_env (output_data_file);
        a_manager.set_output_data_file (output_data_file);
      }
      if (! a_params.vg_name.empty ()) {
        a_manager.set_vertex_generator_name (a_params.vg_name);
      }
      if (! a_params.eg_name.empty ()) {
        a_manager.set_event_generator_name (a_params.eg_name);
      }
      if (a_params.noevents <= mctools::g4::manager::constants::instance ().NO_LIMIT) {
        a_manager.set_number_of_events (a_params.noevents);
      }
      if (a_params.noevents_modulo > 0) {
        a_manager.set_number_events_modulo (a_params.noevents_modulo);
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

      if (a_params.debug) a_manager.dump_base (clog, "mctools::g4::manager: ", "DEBUG: ");

      /*** Configuration file ***/

      if (a_params.manager_config_filename.empty ()) {
        throw logic_error ("mctools::g4::manager_parameters::setup: Missing configuration filename for the simulation manager !");
      }
      string manager_config_filename = a_params.manager_config_filename;
      datatools::fetch_path_with_env (manager_config_filename);
      if (! boost::filesystem::exists (manager_config_filename)) {
        ostringstream message;
        message << "mctools::g4::manager_parameters::setup: "
                << "Simulation manager configuration filename '"
                << manager_config_filename << "' does not exist ! ";
        throw logic_error (message.str ());
        //clog << datatools::io::warning << message.str () << endl;
      }
      datatools::multi_properties
        the_configuration ("name",
                           "",
                           "Configuration for the 'g4' simulation manager");
      the_configuration.read (manager_config_filename);

      if (a_params.debug) {
        the_configuration.tree_dump (clog,
                                     "mctools::g4::manager_parameters::setup: ",
                                     "DEBUG: ");
      }

      /*** Initialization ***/

      clog << datatools::io::notice
           << "mctools::g4::manager_parameters::setup: "
           << "Initializing the simulation manager..." << endl;
      a_manager.initialize (the_configuration);

      if (a_params.debug) a_manager.dump_base (clog, "mctools::g4::manager: ", "DEBUG: ");

      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of manager_parameters.cc
