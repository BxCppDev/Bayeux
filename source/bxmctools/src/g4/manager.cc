// -*- mode: c++ ; -*-
/* manager.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#include <mctools/g4/manager.h>

#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

#include <mygsl/random_utils.h>

#include <genvtx/i_vertex_generator.h>

#include <genbb_help/manager.h>
#include <genbb_help/i_genbb.h>

#include <mctools/g4/run_action.h>
#include <mctools/g4/event_action.h>
#include <mctools/g4/detector_construction.h>
#include <mctools/g4/primary_generator.h>
#include <mctools/g4/physics_list.h>
#include <mctools/g4/tracking_action.h>
#include <mctools/g4/stepping_action.h>
#include <mctools/g4/stacking_action.h>
#include <mctools/g4/simulation_ctrl.h>

#include <CLHEP/Random/Random.h>

// G4 stuff:
#include <globals.hh>
#include <G4ParticleDefinition.hh>
#include <G4UIterminal.hh>
#include <G4UImanager.hh>
#include <G4RunManager.hh>

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"      // For terminal tcsh use
#endif // G4UI_USE_TCSH

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif // G4VIS_USE

namespace mctools {

  namespace g4 {

    using namespace std;

    const std::string manager::DEFAULT_PRNG_ID = "taus2";

    // static
    const manager::constants &
    manager::constants::instance()
    {
      static boost::scoped_ptr<manager::constants> g_global_constants(0);
      if ( g_global_constants.get() == 0) {
        g_global_constants.reset(new manager::constants);
      }
      return *g_global_constants.get();
    }

    manager::constants::constants()
    {
      NO_LIMIT                 = manager::NUMBER_OF_EVENTS_UPPER_LIMIT;
      G4_MANAGER_LABEL         = "MGR";
      VERTEX_GENERATOR_LABEL   = "VG";
      EVENT_GENERATOR_LABEL    = "EG";
      SHPF_LABEL               = "SHPF";
      DEFAULT_PRNG_STATES_FILE = "mctools_g4_prng_states.data";
      DEFAULT_PRNG_STATES_SAVE_MODULO = 100;
      return;
    }

    bool manager::has_simulation_ctrl() const
    {
      return _simulation_ctrl_ != 0;
    }

    void manager::set_simulation_ctrl(simulation_ctrl & a_simulation_ctrl)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF (has_simulation_ctrl(),
                   std::logic_error,
                   "Operation prohibited ! Manager already got a 'simulation ctrl' object !");
      _simulation_ctrl_ = &a_simulation_ctrl;
      return;
    }

    void manager::reset_simulation_ctrl ()
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _simulation_ctrl_ = 0;
      return;
    }

    const simulation_ctrl & manager::get_simulation_ctrl() const
    {
      DT_THROW_IF (! has_simulation_ctrl(),
                   std::logic_error,
                   "Manager has no 'simulation ctrl' object !");
      return *_simulation_ctrl_;
    }

    simulation_ctrl & manager::grab_simulation_ctrl()
    {
      DT_THROW_IF (! has_simulation_ctrl(),
                   std::logic_error,
                   "Manager has no 'simulation ctrl' object !");
      return *_simulation_ctrl_;
    }

    bool manager::is_initialized() const
    {
      return _initialized_;
    }

    bool manager::using_time_stat() const
    {
      return _use_time_stat_;
    }

    void manager::set_using_time_stat(bool new_value_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _use_time_stat_ = new_value_;
      return;
    }

    bool manager::has_service_manager() const
    {
      return _service_manager_ != 0;
    }

    void manager::set_service_manager(datatools::service_manager & smgr_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _service_manager_ = &smgr_;
      return;
    }

    datatools::service_manager & manager::grab_service_manager()
    {
      DT_THROW_IF (_service_manager_ == 0, std::logic_error,
                   "No service manager is available !");
      return *_service_manager_;
    }

    const datatools::service_manager & manager::get_service_manager() const
    {
      DT_THROW_IF (_service_manager_ == 0, std::logic_error,
                   "No service manager is available !");
      return *_service_manager_;
    }

    bool manager::has_external_geom_manager() const
    {
      return _external_geom_manager_ != 0;
    }

    void manager::set_external_geom_manager(const geomtools::manager & a_geometry_manager)
    {
      DT_THROW_IF (_geom_manager_.is_initialized(),
                   std::logic_error,
                   "Embedded geometry manager is already initialized ! "
                   << "Cannot substitute another geometry manager !");
      DT_THROW_IF (! a_geometry_manager.is_initialized(),
                   std::logic_error,
                   "External geometry manager is not initialized !");
      _external_geom_manager_ = &a_geometry_manager;
      return;
    }

    const geomtools::manager & manager::get_geom_manager() const
    {
      if (has_external_geom_manager()) {
        DT_THROW_IF (! _external_geom_manager_->is_initialized(),
                     std::logic_error,
                     "External geometry manager is not initialized !");
        return *_external_geom_manager_;
      }
      DT_THROW_IF (! _geom_manager_.is_initialized(),
                   std::logic_error, "Geometry manager is not initialized !");
      return _geom_manager_;
    }


    bool manager::forbids_private_hits() const
    {
      return _forbid_private_hits_;
    }


    void manager::set_forbid_private_hits(bool a_forbid)
    {
      _forbid_private_hits_ = a_forbid;
      return;
    }


    bool manager::dont_save_no_sensitive_hit_events() const
    {
      return _dont_save_no_sensitive_hit_events_;
    }


    void manager::set_dont_save_no_sensitive_hit_events(bool a_dont_save)
    {
      _dont_save_no_sensitive_hit_events_ = a_dont_save;
      return;
    }


    const manager::CT_map & manager::get_CT_map() const
    {
      return _CTs_;
    }


    manager::CT_map & manager::grab_CT_map()
    {
      return _CTs_;
    }


    bool manager::is_interactive() const
    {
      return _interactive_;
    }

    bool manager::is_batch() const
    {
      return ! is_interactive();
    }

    void manager::set_interactive(bool new_value_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _interactive_ = new_value_;
      if (! _interactive_) _g4_visualization_ = false;
      return;
    }

    bool manager::has_g4_visualization() const
    {
      return is_interactive() && _g4_visualization_;
    }

    const string & manager::get_g4_macro() const
    {
      return _g4_macro_;
    }

    bool manager::has_g4_macro() const
    {
      return ! _g4_macro_.empty();
    }

    void manager::set_g4_macro(const string & g4_macro_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _g4_macro_ = g4_macro_;
      return;
    }

    void manager::set_g4_visualization(bool new_value_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
#ifdef G4VIS_USE
      _g4_visualization_ = new_value_;
      if (_g4_visualization_) _interactive_ = true;
#else // ! G4VIS_USE
      _g4_visualization_ = false;
      if (new_value_) {
        DT_LOG_WARNING(_logprio(), "G4 visualization is not available !");
      }
#endif // G4VIS_USE
      return;
    }

    bool manager::is_automatic() const
    {
      return is_batch() && _g4_macro_.empty();
    }

    void manager::set_number_of_events(uint32_t nevents_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF (nevents_ < NUMBER_OF_EVENTS_LOWER_LIMIT,
                   std::domain_error,
                   "Invalid 'zero' number of events !");
      DT_THROW_IF (nevents_ > NUMBER_OF_EVENTS_UPPER_LIMIT,
                   std::domain_error,
                   "Invalid 'too large' number of events !");
      _number_of_events_ = nevents_;
      if (_number_of_events_ > NUMBER_OF_EVENTS_WARNING_LIMIT) {
        DT_LOG_WARNING(_logprio(), "Number of events is high = " << _number_of_events_);
      }
      DT_LOG_DEBUG(_logprio(), "Number of events = " << _number_of_events_);
      return;
    }

    const mygsl::seed_manager & manager::get_seed_manager() const
    {
      return _seed_manager_;
    }

    mygsl::seed_manager & manager::grab_seed_manager()
    {
      return _seed_manager_;
    }

    const mygsl::prng_state_manager & manager::get_state_manager() const
    {
      return _prng_state_manager_;
    }

    mygsl::prng_state_manager & manager::grab_state_manager()
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      return _prng_state_manager_;
    }

    void manager::set_event_generator_seed(int seed_)
    {
      if (_eg_prng_.is_initialized()) {
        DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      }
      _eg_prng_seed_ = seed_;
      return;
    }

    void manager::set_event_generator_name(const std::string & eg_name_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _eg_name_ = eg_name_;
      return;
    }

    void manager::set_vertex_generator_seed(int seed_)
    {
      if (_vg_prng_.is_initialized()) {
        DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      }
      _vg_prng_seed_ = seed_;
      return;
    }

    void manager::set_vertex_generator_name(const std::string & vg_name_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _vg_name_ = vg_name_;
      return;
    }

    void manager::set_shpf_prng_seed(int seed_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _shpf_prng_seed_ = seed_;
      return;
    }


    void manager::set_use_track_history(const bool use_)
    {
      _use_track_history_ = use_;
      return;
    }

    bool manager::has_track_history() const
    {
      return _use_track_history_;
    }

    const track_history & manager::get_track_history() const
    {
      return _track_history_;
    }

    track_history & manager::grab_track_history()
    {
      return _track_history_;
    }

    void manager::_init_defaults()
    {
      // pointers:
      _g4_stepping_verbosity_   = 0;
      _g4_run_manager_          = 0;
      _g4_UI_                   = 0;
      _user_physics_list_       = 0;
      _user_primary_generator_  = 0;
      _user_run_action_         = 0;
      _user_event_action_       = 0;
      _user_tracking_action_    = 0;
      _user_stepping_action_    = 0;
      _user_stacking_action_    = 0;
#ifdef G4VIS_USE
      _g4_vis_manager_          = 0;
#endif // G4VIS_USE

      _vg_prng_seed_             = mygsl::random_utils::SEED_INVALID;
      _eg_prng_seed_             = mygsl::random_utils::SEED_INVALID;
      _mgr_prng_seed_            = mygsl::random_utils::SEED_INVALID;
      _shpf_prng_seed_           = mygsl::random_utils::SEED_INVALID;

      _number_of_events_         = constants::instance().NO_LIMIT;
      _number_of_events_modulo_  = run_action::NUMBER_OF_EVENTS_MODULO_NONE;
      _g4_macro_                 = "";
      _g4_tracking_verbosity_    = 0;
      _use_event_number_as_seed_ = false;
      _input_prng_states_file_   = "";
      _output_prng_states_file_  = "";
      _prng_state_save_modulo_   = constants::instance().DEFAULT_PRNG_STATES_SAVE_MODULO;

      _input_prng_seeds_file_    = "";
      _output_prng_seeds_file_   = "";

      _multi_config_     = 0;
      _interactive_      = false;
      _g4_visualization_ = false;

      _forbid_private_hits_               = false;
      _dont_save_no_sensitive_hit_events_ = false;
      _use_run_header_footer_             = false;

      _use_track_history_ = false;
      return;
    }

    void manager::set_use_run_header_footer(bool a_use_run_header_footer)
    {
      _use_run_header_footer_ = a_use_run_header_footer;
      return;
    }

    bool manager::using_run_header_footer() const
    {
      return _use_run_header_footer_;
    }

    bool manager::has_number_of_events_modulo() const
    {
      return _number_of_events_modulo_ > run_action::NUMBER_OF_EVENTS_MODULO_NONE;
    }

    void manager::set_number_of_events_modulo(int a_m)
    {
      if (a_m <= run_action::NUMBER_OF_EVENTS_MODULO_NONE) {
        _number_of_events_modulo_ = run_action::NUMBER_OF_EVENTS_MODULO_NONE;
      } else {
        _number_of_events_modulo_ = a_m;
      }
      DT_LOG_DEBUG(_logprio(), "Number of events modulo = " << _number_of_events_modulo_);
      return;
    }

    int manager::get_number_of_events_modulo() const
    {
      return _number_of_events_modulo_;
    }

    void manager::set_prng_state_save_modulo(int a_modulo)
    {
      if (a_modulo < 1) {
        _prng_state_save_modulo_ = 0;
        return;
      }
      _prng_state_save_modulo_ = a_modulo;
      return;
    }

    int manager::get_prng_state_save_modulo() const
    {
      return _prng_state_save_modulo_;
    }

    bool manager::has_prng_state_save_modulo() const
    {
      return _prng_state_save_modulo_ > 0;
    }

    /********************
     * vertex generator *
     ********************/

    const genvtx::manager & manager::get_vg_manager() const
    {
      return _vg_manager_;
    }

    genvtx::manager & manager::grab_vg_manager()
    {
      return _vg_manager_;
    }

    bool manager::has_vertex_generator() const
    {
      return _vertex_generator_ != 0;
    }

    const genvtx::i_vertex_generator & manager::get_vertex_generator() const
    {
      DT_THROW_IF (! has_vertex_generator(),
                   std::logic_error, "No vertex generator is available !");
      return *_vertex_generator_;
    }

    /*******************
     * event generator *
     *******************/

    const genbb::manager & manager::get_eg_manager() const
    {
      return _eg_manager_;
    }

    genbb::manager & manager::grab_eg_manager()
    {
      return _eg_manager_;
    }

    bool manager::has_event_generator() const
    {
      return _event_generator_ != 0;
    }

    const genbb::i_genbb & manager::get_event_generator() const
    {
      DT_THROW_IF (! has_event_generator(), std::logic_error,
                   "No event generator is available !");
      return *_event_generator_;
    }

    bool manager::using_event_number_as_seed() const
    {
      return _use_event_number_as_seed_;
    }

    void manager::reset_output_prng_seeds_file()
    {
      _output_prng_seeds_file_ = "";
      return;
    }

    void manager::reset_input_prng_seeds_file()
    {
      _input_prng_seeds_file_ = "";
      return;
    }

    void manager::_init_prngs_states()
    {
      // Load the PRNG internal states buffers :
      if (has_input_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(),  "Loading PRNG internal states from file '"
                      << get_input_prng_states_file() << "'...");
        _prng_state_manager_.load(get_input_prng_states_file());
      }

      // Ensure the creation of all useful buffers :
      // G4 PRNG :
      if (! _prng_state_manager_.has_state(constants::instance().G4_MANAGER_LABEL)) {
        _prng_state_manager_.add_state(constants::instance().G4_MANAGER_LABEL,
                                       _mgr_prng_.get_internal_state_size());
      }
      // Vertex generator PRNG :
      if (! _prng_state_manager_.has_state(constants::instance().VERTEX_GENERATOR_LABEL)) {
        _prng_state_manager_.add_state(constants::instance().VERTEX_GENERATOR_LABEL,
                                       _vg_prng_.get_internal_state_size());
      }
      // Event generator PRNG :
      if (! _prng_state_manager_.has_state(constants::instance().EVENT_GENERATOR_LABEL)) {
        _prng_state_manager_.add_state(constants::instance().EVENT_GENERATOR_LABEL,
                                       _eg_prng_.get_internal_state_size());
      }
      // PRNG for Step Hit Processors :
      if (! _prng_state_manager_.has_state(constants::instance().SHPF_LABEL)) {
        _prng_state_manager_.add_state(constants::instance().SHPF_LABEL,
                                       _shpf_prng_.get_internal_state_size());
      }

      // Setup the output file name for PRNG internal states backup :
      if (has_output_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(), "Setting the file '" << get_output_prng_states_file()
                      << "'for storing the PRNG internal states...");
        _prng_state_manager_.set_filename(get_output_prng_states_file());
      }

      if (has_input_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(), "Initializing the PRNG internal states from the 'state manager'...");

        // G4 PRNG :
        _mgr_prng_.from_buffer(_prng_state_manager_.get_state(constants::instance().G4_MANAGER_LABEL).state_buffer);

        // Vertex generator PRNG :
        _vg_prng_.from_buffer(_prng_state_manager_.get_state(constants::instance().VERTEX_GENERATOR_LABEL).state_buffer);

        // Event generator PRNG :
        _eg_prng_.from_buffer(_prng_state_manager_.get_state(constants::instance().EVENT_GENERATOR_LABEL).state_buffer);

        // PRNG for Step Hit Processors :
        _shpf_prng_.from_buffer(_prng_state_manager_.get_state(constants::instance().SHPF_LABEL).state_buffer);

      }

      return;
    }

    void manager::record_current_prng_states()
    {
      // G4 PRNG :
      _mgr_prng_.to_buffer(_prng_state_manager_.get_state(constants::instance().G4_MANAGER_LABEL).state_buffer);

      // Vertex generator PRNG :
      _vg_prng_.to_buffer(_prng_state_manager_.get_state(constants::instance().VERTEX_GENERATOR_LABEL).state_buffer);

      // Event generator PRNG :
      _eg_prng_.to_buffer(_prng_state_manager_.get_state(constants::instance().EVENT_GENERATOR_LABEL).state_buffer);

      // PRNG of the Step Hit Processor Factory :
      _shpf_prng_.to_buffer(_prng_state_manager_.get_state(constants::instance().SHPF_LABEL).state_buffer);

      _prng_state_manager_.increment_counter(get_prng_state_save_modulo());
      if (is_debug()) _prng_state_manager_.dump(std::clog);
      _prng_state_manager_.store();
      return;
    }

    bool manager::has_input_prng_seeds_file() const
    {
      return ! _input_prng_seeds_file_.empty();
    }

    const string & manager::get_input_prng_seeds_file() const
    {
      return _input_prng_seeds_file_;
    }

    void manager::set_input_prng_seeds_file(const string & fn_)
    {
      _input_prng_seeds_file_ = fn_;
      return;
    }

    bool manager::has_output_prng_seeds_file() const
    {
      return ! _output_prng_seeds_file_.empty();
    }

    const string & manager::get_output_prng_seeds_file() const
    {
      return _output_prng_seeds_file_;
    }

    void manager::set_output_prng_seeds_file(const string & fn_)
    {
      _output_prng_seeds_file_ = fn_;
      return;
    }

    void manager::reset_output_prng_states_file()
    {
      _output_prng_states_file_ = "";
      return;
    }

    const string & manager::get_output_prng_states_file() const
    {
      return _output_prng_states_file_;
    }

    bool manager::has_output_prng_states_file() const
    {
      return ! _output_prng_states_file_.empty();
    }

    void manager::set_output_prng_states_file(const string & fn_)
    {
      _output_prng_states_file_ = fn_;
      return;
    }

    void manager::reset_input_prng_states_file()
    {
      _input_prng_states_file_ = "";
      return;
    }


    const string & manager::get_input_prng_states_file() const
    {
      return _input_prng_states_file_;
    }

    bool manager::has_input_prng_states_file() const
    {
      return ! _input_prng_states_file_.empty();
    }

    void manager::set_input_prng_states_file(const string & fn_)
    {
      _input_prng_states_file_ = fn_;
      return;
    }

    void manager::set_output_data_file(const string & fn_)
    {
      _output_data_file_ = fn_;
      return;
    }

    void manager::set_mgr_prng_seed(int rseed_)
    {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      if (rseed_ < 0) {
        _mgr_prng_seed_ = mygsl::random_utils::SEED_INVALID;
      } else {
        _mgr_prng_seed_ = rseed_;
      }
      return;
    }

    bool manager::has_mgr_prng_seed() const
    {
      return _mgr_prng_seed_ != mygsl::random_utils::SEED_INVALID;
    }

    int manager::get_mgr_prng_seed() const
    {
      return _mgr_prng_seed_;
    }

    const mygsl::rng & manager::get_mgr_prng() const
    {
      return _mgr_prng_;
    }

    mygsl::rng & manager::grab_vg_prng()
    {
      return _vg_prng_;
    }

    mygsl::rng & manager::grab_eg_prng()
    {
      return _eg_prng_;
    }

    mygsl::rng & manager::grab_shpf_prng()
    {
      return _shpf_prng_;
    }

    mygsl::rng & manager::grab_mgr_prng()
    {
      return _mgr_prng_;
    }

    const g4_prng & manager::get_g4_prng() const
    {
      return _g4_prng_;
    }

    g4_prng & manager::grab_g4_prng()
    {
      return _g4_prng_;
    }

    // ctor:
    manager::manager()
    {
      _initialized_ = false;
      _simulation_ctrl_label_ = "";
      _simulation_ctrl_ = 0;
      _service_manager_ = 0;
      _external_geom_manager_ = 0;
      _vertex_generator_ = 0;
      _event_generator_ = 0;
      _init_defaults();
      return;
    }

    // dtor:
    manager::~manager()
    {
      if (_initialized_) {
        reset();
      }
      return;
    }

    void manager::_init_seeds()
    {
      // All seeds being 'SEED_TIME'(see mygsl::seed_manager)
      // are initialized with the current time :
      // 2012-06-08 FM : fix to make all seeds different(and
      // automatically randomize 'SEED_TIME'):
      _seed_manager_.ensure_different_seeds();
      if (is_debug()) {
        _seed_manager_.dump(std::clog);
      }

      // Then pickup the seed associated to each embedded component :

      // the G4 manager itself :
      if (_seed_manager_.has_seed(constants::instance().G4_MANAGER_LABEL)) {
        int seed = _seed_manager_.get_seed(constants::instance().G4_MANAGER_LABEL);
        set_mgr_prng_seed(seed);
        DT_LOG_NOTICE(_logprio(),"Using registered seed for '"
                      << constants::instance().G4_MANAGER_LABEL << "' : "
                      << seed);
      }

      // the vertex generator :
      if (_seed_manager_.has_seed(constants::instance().VERTEX_GENERATOR_LABEL)) {
        int seed = _seed_manager_.get_seed(constants::instance().VERTEX_GENERATOR_LABEL);
        set_vertex_generator_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << constants::instance().VERTEX_GENERATOR_LABEL << "' : "
                      << seed);
      }

      // the event generator :
      if (_seed_manager_.has_seed(constants::instance().EVENT_GENERATOR_LABEL)) {
        int seed = _seed_manager_.get_seed(constants::instance().EVENT_GENERATOR_LABEL);
        set_event_generator_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << constants::instance().EVENT_GENERATOR_LABEL << "' : "
                      << seed);
      }

      // the step hit processor factory :
      if (_seed_manager_.has_seed(constants::instance().SHPF_LABEL)) {
        int seed = _seed_manager_.get_seed(constants::instance().SHPF_LABEL);
        set_shpf_prng_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << constants::instance().SHPF_LABEL << "' : "
                      << seed);
      }

      // Save a file with the content of the embedded 'seed_manager' :
      if (has_output_prng_seeds_file()) {
        std::string seeds_filename = get_output_prng_seeds_file();
        datatools::fetch_path_with_env(seeds_filename);
        if (boost::filesystem::exists(seeds_filename)) {
          std::string backup_seeds_filename = seeds_filename + ".~backup~";
          DT_LOG_NOTICE(_logprio(),
                        "Backuping the former Random Number Generators seeds file '"
                        << seeds_filename << "' to '" << backup_seeds_filename << "'...");
          if (boost::filesystem::exists(backup_seeds_filename)) {
            DT_LOG_NOTICE(_logprio(),
                          "Deleting old backup Random Number Generators seeds file '"
                          << backup_seeds_filename << "'...");
            boost::filesystem::remove(backup_seeds_filename);
          }
          boost::filesystem::rename(seeds_filename, backup_seeds_filename);
        }
        DT_LOG_NOTICE(_logprio(),
                      "Saving the Random Number Generator seeds in file '"
                      << seeds_filename << "'...");
        std::ofstream seeds_file(seeds_filename.c_str());
        seeds_file << _seed_manager_ << std::endl;
      }

      return;
    }

    void manager::initialize(const datatools::multi_properties & multi_config_)
    {
      DT_THROW_IF (_initialized_, std::logic_error, "Manager is already initialized !");
      _multi_config_ = &multi_config_;
      _at_init();
      _initialized_ = true;
      return;
    }

    void manager::reset()
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Manager is not initialized !");
      _initialized_ = false;
      _at_reset();
      return;
    }

    void manager::_init_core()
    {
      // Main manager:
      const datatools::properties & manager_config
        = _multi_config_->get("manager").get_properties();

      loggable_support::_initialize_logging_support(manager_config);

      DT_LOG_DEBUG(_logprio(), "Manager config :");
      if (is_debug()) {
        manager_config.tree_dump(std::clog);
      }

      DT_LOG_NOTICE(_logprio(),"Main manager settings...");

      // Load a file with the content of the embedded 'seed_manager' :
      if (has_input_prng_seeds_file()) {
        DT_LOG_NOTICE(_logprio(),
                      "PRNG seeds dictionary from '" << get_input_prng_seeds_file() << "'...");
        if (has_output_prng_seeds_file()) {
          // Ensure the input/output seed files are different :
          DT_THROW_IF (get_input_prng_seeds_file() == get_output_prng_seeds_file(),
                       std::logic_error,
                       "Input and output seeds dictionary file have the same path '"
                       << get_input_prng_seeds_file() << "' !");
        }

        std::string seeds_filename = get_input_prng_seeds_file();
        datatools::fetch_path_with_env(seeds_filename);
        if (boost::filesystem::exists(seeds_filename)) {
          DT_LOG_NOTICE(_logprio(),
                        "Loading seeds dictionary from '" << seeds_filename << "'...");
          std::ifstream seeds_file(seeds_filename.c_str());
          seeds_file >> _seed_manager_;
          DT_THROW_IF (! seeds_file,
                       std::runtime_error,
                       "Invalid input or format ! Cannot load the seeds dictionary from '" << seeds_filename << "' !");
        } else {
          DT_LOG_NOTICE(_logprio(),
                        "No seeds dictionary file to be loaded.");
        }
      }

      /*** begin of property parsing ***/

      // 2011-02-26 FM : only search for the 'number_of_events' property
      // if '_number_of_events' has not been set yet :
      if (_number_of_events_ == manager::constants::instance().NO_LIMIT) {
        if (manager_config.has_key("number_of_events")) {
          int noevts = manager_config.fetch_integer("number_of_events");
          uint32_t unoevts;
          if (noevts <= 0) {
            unoevts = manager::constants::instance().NO_LIMIT;
          } else {
            unoevts = noevts;
          }
          set_number_of_events(unoevts);
        }
      }

      // 2011-02-26 FM : only search for the 'g4_macro' property if '_g4_macro_' is not set yet :
      if (! _g4_macro_.empty()) {
        if (manager_config.has_key("g4_macro")) {
          std::string g4m = manager_config.fetch_string( "g4_macro");
          set_g4_macro(g4m);
        }
      }

      if (_g4_tracking_verbosity_ == 0) {
        if (manager_config.has_key("g4_tracking_verbosity")) {
          int tracking_verbosity = manager_config.fetch_integer("g4_tracking_verbosity");
          if (tracking_verbosity <= 0) {
            tracking_verbosity = 0;
          }
          if (tracking_verbosity >= 10) {
            tracking_verbosity = 10;
          }
          _g4_tracking_verbosity_ = tracking_verbosity;
        }
      }

      /*** end of property parsing ***/

      /********************************************************************/

      /*** begin of special configuration ***/

      // init the seed of PRNG embedded in components :
      _init_seeds();

      // some specific init for the G4 PRNG :
      _init_prngs();

      // some specific init for the G4 PRNG :
      if (_use_time_stat_) {
        _init_time_stat();
      }

      /*** end of special configuration ***/

      return;
    }

    void manager::_init_geometry()
    {
      // Geometry manager:
      DT_LOG_NOTICE(_logprio(), "Geometry manager settings...");
      if (has_external_geom_manager()) {
        DT_LOG_NOTICE(_logprio(), "Use external geometry manager...");
        DT_THROW_IF (! _external_geom_manager_->is_initialized(),
                     std::logic_error,
                     "External geometry manager is not initialized !");
      } else {
        DT_LOG_NOTICE(_logprio(), "Use embeded geometry manager...");
        const datatools::properties & geometry_config
          = _multi_config_->get("geometry").get_properties();
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(), "Geometry configuration : ");
          geometry_config.tree_dump(std::clog);
        }
        DT_THROW_IF (! geometry_config.has_key("manager.config"),
                     logic_error, "Missing geometry configuration !");
        std::string geom_mgr_prop_filename = geometry_config.fetch_string("manager.config");
        datatools::fetch_path_with_env(geom_mgr_prop_filename);
        datatools::properties geom_mgr_config;
        datatools::properties::read_config(geom_mgr_prop_filename, geom_mgr_config);
        _geom_manager_.set_mapping_requested(true);
        if (_use_time_stat_) {
          _CTs_["GB"].start();
        }
        _geom_manager_.initialize(geom_mgr_config);
        if (_use_time_stat_) {
          _CTs_["GB"].stop();
        }
      }
      return;
    }

    void manager::_init_vertex_generator()
    {

      // Vertex generator:
      DT_LOG_NOTICE(_logprio(),"Vertex generator settings...");
      if (_multi_config_->has_section("vertex_generator")) {
        const datatools::properties & vertex_generator_config
          = _multi_config_->get("vertex_generator").get_properties();
        _vg_manager_.set_external_random(_vg_prng_);
        if (has_service_manager()) {
          _vg_manager_.set_service_manager(*_service_manager_);
        }
        _vg_manager_.set_geometry_manager(get_geom_manager());
        _vg_manager_.set_generator_name(_vg_name_);
        if (vertex_generator_config.has_key("manager.config")) {
          // using an external configuration file:
          std::string vtx_gtor_prop_filename
            = vertex_generator_config.fetch_string("manager.config");
          datatools::fetch_path_with_env(vtx_gtor_prop_filename);
          datatools::properties vtx_gtor_config;
          datatools::properties::read_config(vtx_gtor_prop_filename,
                                             vtx_gtor_config);
          _vg_manager_.initialize(vtx_gtor_config);
        } else {
          _vg_manager_.initialize(vertex_generator_config);
        }

        DT_THROW_IF(!_vg_manager_.is_initialized(),
                    std::logic_error,
                    "Vertex generator manager is not initialized !");
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(),"Vertex generator manager : ");
          _vg_manager_.tree_dump(std::clog);
        }
        DT_THROW_IF (! _vg_manager_.has_generator(_vg_name_),
                     std::logic_error,
                     "Cannot find vertex generator named '"
                     + _vg_name_ + "' !");
        _vertex_generator_ = &_vg_manager_.grab(_vg_name_);
      } else {
        DT_LOG_WARNING(_logprio(), "No vertex generator settings.");
      }
      return;
    }

    void manager::_init_event_generator()
    {
      // Event generator:
      DT_LOG_NOTICE(_logprio(), "Primary event generator settings...");
      datatools::properties primary_generator_config;
      DT_THROW_IF (!_multi_config_->has_section("event_generator"),
                   std::logic_error,
                   "Missing primary event generator configuration !");
      primary_generator_config  = _multi_config_->get("event_generator").get_properties();
      _eg_manager_.set_external_prng(_eg_prng_);
      if (primary_generator_config.has_key("manager.config")) {
        // using an external configuration file:
        std::string event_gtor_prop_filename
          = primary_generator_config.fetch_string("manager.config");
        datatools::fetch_path_with_env(event_gtor_prop_filename);
        datatools::properties event_gtor_config;
        datatools::properties::read_config(event_gtor_prop_filename,
                                           event_gtor_config);
        _eg_manager_.initialize(event_gtor_config);
      } else {
        _eg_manager_.initialize(primary_generator_config);
      }
      DT_THROW_IF(!_eg_manager_.is_initialized(),
                  std::logic_error,
                  "Primary event generator manager is not initialized !");
      DT_THROW_IF (! _eg_manager_.has(_eg_name_),
                   std::logic_error,
                   "Cannot find primary event generator named '" << _eg_name_ << "' !");
      _event_generator_ = &_eg_manager_.grab(_eg_name_);
      return;
    }

    void manager::_init_detector_construction ()
    {
      // Detector construction:
      DT_LOG_NOTICE(_logprio(), "Detector construction...");
      DT_THROW_IF (! _multi_config_->has_section("detector_construction"),
                   std::logic_error,
                   "Missing detector construction configuration !");
      const datatools::properties & detector_construction_config
        = _multi_config_->get("detector_construction").get_properties();
      _user_detector_construction_ = new detector_construction(*this);
      _user_detector_construction_->grab_step_hit_processor_factory().set_external_prng(_shpf_prng_);
      _user_detector_construction_->set_geometry_manager(get_geom_manager());
      _user_detector_construction_->initialize(detector_construction_config);
      _g4_run_manager_->SetUserInitialization(_user_detector_construction_);
      return;
    }

    void manager::_init_physics_list ()
    {
      DT_LOG_NOTICE(_logprio(), "Physics list initialization...");
      DT_THROW_IF (! _multi_config_->has_section("physics_list"),
                   std::logic_error,
                   "Missing physics list configuration !");
      const datatools::properties & physics_list_config
        = _multi_config_->get("physics_list").get_properties();
      _user_physics_list_ = new physics_list;
      _user_physics_list_->initialize(physics_list_config);
      _g4_run_manager_->SetUserInitialization(_user_physics_list_);
      DT_LOG_DEBUG(_logprio(), "Physics list: ");
      if (is_debug()) _user_physics_list_->tree_dump(std::clog);
      return;
    }

    void manager::_init_run_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Run action initialization...");
      DT_THROW_IF (! _multi_config_->has_section("run_action"),
                   std::logic_error,
                   "Missing run action configuration !");
      const datatools::properties & run_action_config
        = _multi_config_->get("run_action").get_properties();
      _user_run_action_ = new run_action(*this);
      if (! _output_data_file_.empty()) {
        _user_run_action_->set_output_file(_output_data_file_);
      }
      if (has_number_of_events_modulo()) {
        _user_run_action_->set_number_of_events_modulo(get_number_of_events_modulo());
      }
      _user_run_action_->set_use_run_header_footer(using_run_header_footer());
      _user_run_action_->initialize(run_action_config);
      _g4_run_manager_->SetUserAction(_user_run_action_);
      return;
    }

    void manager::_init_event_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Event action initialization...");
      DT_THROW_IF (! _multi_config_->has_section("event_action"),
                   std::logic_error,
                   "Missing event action configuration !");
      const datatools::properties & event_action_config
        = _multi_config_->get("event_action").get_properties();
      _user_event_action_ = new event_action(*_user_run_action_,
                                             *_user_detector_construction_);
      _user_event_action_->initialize(event_action_config);
      _g4_run_manager_->SetUserAction(_user_event_action_);
      return;
    }


    void manager::_init_primary_generator_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Primary generator action initialization...");
      DT_THROW_IF (!_multi_config_->has_section("primary_generator_action"),
                   std::logic_error,
                   "Missing primary event generator action configuration !");
      const datatools::properties & primary_generator_config
        = _multi_config_->get("primary_generator_action").get_properties();

      _user_primary_generator_ = new primary_generator;
      _user_primary_generator_->set_run_action(*_user_run_action_);
      _user_primary_generator_->set_event_action(*_user_event_action_);
      if (has_vertex_generator()) {
        _user_primary_generator_->set_vertex_generator(*_vertex_generator_);
      }
      _user_primary_generator_->set_event_generator(*_event_generator_);
      _user_primary_generator_->initialize(primary_generator_config);
      _g4_run_manager_->SetUserAction(_user_primary_generator_);
      return;
    }

    void manager::_init_tracking_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Tracking action initialization...");
      if (_multi_config_->has_section("tracking_action")) {
        const datatools::properties &  tracking_action_config  = _multi_config_->get("tracking_action").get_properties();
        _user_tracking_action_ = new tracking_action;
        _user_tracking_action_->initialize(tracking_action_config);
        _g4_run_manager_->SetUserAction(_user_tracking_action_);
      }
      return;
    }

    void manager::_init_stepping_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Stepping action initialization...");
      if (_multi_config_->has_section("stepping_action")) {
        const datatools::properties & stepping_action_config = _multi_config_->get("stepping_action").get_properties();
        _user_stepping_action_ = new stepping_action;
        _user_stepping_action_->initialize(stepping_action_config);
        _g4_run_manager_->SetUserAction(_user_stepping_action_);
      }
      return;
    }


    void manager::_init_stacking_action ()
    {
      DT_LOG_NOTICE(_logprio(), "Stacking action initialization...");
      if (_multi_config_->has_section("stacking_action")) {
        const datatools::properties & stacking_action_config = _multi_config_->get("stacking_action").get_properties();
        _user_stacking_action_ = new stacking_action;
        _user_stacking_action_->initialize(stacking_action_config);
        _g4_run_manager_->SetUserAction(_user_stacking_action_);
      }
      return;
    }

    void manager::_init_time_stat()
    {
      {
        // Time statistics for "Event Action" :
        CT_type ct;
        _CTs_["EA"] = ct;
      }
      {
        // Time statistics for "Run Action" :
        CT_type ct;
        _CTs_["RA"] = ct;
      }
      {
        // Time statistics for "I/O operations" :
        CT_type ct;
        _CTs_["IO"] = ct;
      }
      {
        // Time statistics for "Sensitive detectors" :
        CT_type ct;
        _CTs_["SD"] = ct;
      }
      {
        // Time statistics for "Hit processors" :
        CT_type ct;
        _CTs_["HP"] = ct;
      }
      {
        // Time statistics for "Geometry building" :
        CT_type ct;
        _CTs_["GB"] = ct;
      }
      {
        // Time statistics for "Vertex generation" :
        CT_type ct;
        _CTs_["VG"] = ct;
      }
      {
        // Time statistics for "Event generation" :
        CT_type ct;
        _CTs_["EG"] = ct;
      }
      return;
    }

    void manager::_init_prngs()
    {

      /****************************
       * RANDOM NUMBER GENERATORS *
       ****************************/

      _mgr_prng_.initialize(DEFAULT_PRNG_ID, _mgr_prng_seed_);
      _vg_prng_.initialize(DEFAULT_PRNG_ID, _vg_prng_seed_);
      _eg_prng_.initialize(DEFAULT_PRNG_ID, _eg_prng_seed_);
      _shpf_prng_.initialize(DEFAULT_PRNG_ID, _shpf_prng_seed_);

      bool needs_seed = true;
      if (has_input_prng_states_file()) {
        std::string rsf = get_input_prng_states_file();
        datatools::fetch_path_with_env(rsf);
        if (boost::filesystem::exists(rsf.c_str())) {
          _prng_state_manager_.load();
          DT_LOG_NOTICE(_logprio(), "Random Number Generator internal state was restored from file '"
                        << rsf << "'...");
          needs_seed = false;
          if (_mgr_prng_seed_ != mygsl::random_utils::SEED_INVALID) {
            DT_LOG_NOTICE(_logprio(),
                          "Ignoring the seed for the Random Number Generator initialization !");
          }
          _mgr_prng_seed_ = mygsl::random_utils::SEED_INVALID;
        }
      }

      // if no status file was loaded, use a seed :
      if (needs_seed) {
        DT_THROW_IF (_mgr_prng_seed_ == mygsl::random_utils::SEED_INVALID,
                     std::logic_error,
                     "No seed was provided for the Random Number Generator !");
        if (_mgr_prng_seed_ == mygsl::random_utils::SEED_TIME) {
          int32_t time_seed =(int)(time(0) & 0xFFFFFFFF);
          _mgr_prng_.set_seed(time_seed);
        }
        DT_LOG_NOTICE(_logprio(),
                      "Setting the seed for the GEANT4 Random Number Generator : "
                      << _mgr_prng_seed_);
        _mgr_prng_.set_seed(_mgr_prng_seed_);
      }

      _g4_prng_.set_random(_mgr_prng_);
      CLHEP::HepRandom::setTheEngine(&_g4_prng_);

      DT_LOG_NOTICE(_logprio(),
                    "Setting the HepRandomEngine to '" << _g4_prng_.name() << "'...");

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void manager::_at_init()
    {
      //_g4_stepping_verbosity = new stepping_verbose;
      //G4VSteppingVerbose::SetInstance(_g4_stepping_verbosity);

      /****************
       * MAIN MANAGER *
       ****************/

      // Main manager:
      // const datatools::properties & manager_config
      //   = _multi_config_->get("manager").get_properties();
      // loggable_support::_initialize_logging_support(manager_config);

      _init_core();


      /********************
       * GEOMETRY MANAGER *
       ********************/

      _init_geometry();

      /*
      // Geometry manager:
      DT_LOG_NOTICE(_logprio(), "Geometry manager settings...");
      if (has_external_geom_manager()) {
        DT_LOG_NOTICE(_logprio(), "Use external geometry manager...");
        DT_THROW_IF (! _external_geom_manager_->is_initialized(),
                     std::logic_error,
                     "External geometry manager is not initialized !");
      } else {
        DT_LOG_NOTICE(_logprio(), "Use embeded geometry manager...");
        const datatools::properties & geometry_config
          = _multi_config_->get("geometry").get_properties();
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(), "Geometry configuration : ");
          geometry_config.tree_dump(std::clog);
        }
        DT_THROW_IF (! geometry_config.has_key("geometry.config"),
                     logic_error, "Missing geometry configuration !");
        std::string geom_mgr_prop_filename = geometry_config.fetch_string("geometry.config");
        datatools::fetch_path_with_env(geom_mgr_prop_filename);
        datatools::properties geom_mgr_config;
        datatools::properties::read_config(geom_mgr_prop_filename, geom_mgr_config);
        _geom_manager_.set_mapping_requested(true);
        if (_use_time_stat_) {
          _CTs_["GB"].start();
        }
        _geom_manager_.initialize(geom_mgr_config);
        if (_use_time_stat_) {
          _CTs_["GB"].stop();
        }
      }
      */

      /********************
       * VERTEX GENERATOR *
       ********************/

      _init_vertex_generator();

      /*
      // Vertex generator:
      DT_LOG_NOTICE(_logprio(),"Vertex generator settings...");
      if (_multi_config_->has_section("vertex_generator")) {
        const datatools::properties & vertex_generator_config
          = _multi_config_->get("vertex_generator").get_properties();
        _vg_manager_.set_external_random(_vg_prng_);
        if (has_service_manager()) {
          _vg_manager_.set_service_manager(*_service_manager_);
        }
        _vg_manager_.set_geometry_manager(get_geom_manager());
        _vg_manager_.set_generator_name(_vg_name_);
        if (vertex_generator_config.has_key("manager_config")) {
          // using an external configuration file:
          std::string vtx_gtor_prop_filename
            = vertex_generator_config.fetch_string("manager_config");
          datatools::fetch_path_with_env(vtx_gtor_prop_filename);
          datatools::properties vtx_gtor_config;
          datatools::properties::read_config(vtx_gtor_prop_filename,
                                             vtx_gtor_config);
          _vg_manager_.initialize(vtx_gtor_config);
        } else {
          _vg_manager_.initialize(vertex_generator_config);
        }
        // if (vertex_generator_config.has_key("config")) {
        //   // using an external configuration file:
        //   std::string vtx_gtor_prop_filename
        //     = vertex_generator_config.fetch_string("config");
        //   datatools::fetch_path_with_env(vtx_gtor_prop_filename);
        //   datatools::properties vtx_gtor_config;
        //   datatools::properties::read_config(vtx_gtor_prop_filename, vtx_gtor_config);
        //   _vg_manager_.initialize(vtx_gtor_config);
        // }
        // else {
        //   // using properties:
        //   _vg_manager_.initialize(vertex_generator_config);
        // }
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(),"Vertex generator manager : ");
          _vg_manager_.tree_dump(std::clog);
        }
        DT_THROW_IF (! _vg_manager_.has_generator(_vg_name_),
                     std::logic_error,
                     "Cannot find vertex generator named '"
                     + _vg_name_ + "' !");
        _vertex_generator_ = &_vg_manager_.grab(_vg_name_);
      } else {
        DT_LOG_WARNING(_logprio(), "No vertex generator settings.");
      }
      */

      /*******************
       * EVENT GENERATOR *
       *******************/

      _init_event_generator();

      /*
      // Event generator:
      DT_LOG_NOTICE(_logprio(), "Event generator settings...");
      DT_THROW_IF (!_multi_config_->has_section("primary_generator"),
                   std::logic_error,
                   "Missing primary event generator configuration !");
      const datatools::properties & primary_generator_config
        = _multi_config_->get("primary_generator").get_properties();
      _eg_manager_.set_external_prng(_eg_prng_);
      if (primary_generator_config.has_key("manager_config")) {
        // using an external configuration file:
        std::string event_gtor_prop_filename
          = primary_generator_config.fetch_string("manager_config");
        datatools::fetch_path_with_env(event_gtor_prop_filename);
        datatools::properties event_gtor_config;
        datatools::properties::read_config(event_gtor_prop_filename,
                                           event_gtor_config);
        _eg_manager_.initialize(event_gtor_config);
      } else {
        _eg_manager_.initialize(primary_generator_config);
      }
      // if (primary_generator_config.has_key("config")) {
      //   // using an external configuration file:
      //   std::string event_gtor_prop_filename = primary_generator_config.fetch_string("config");
      //   datatools::fetch_path_with_env(event_gtor_prop_filename);
      //   datatools::multi_properties event_gtor_mconfig;
      //   event_gtor_mconfig.read(event_gtor_prop_filename);
      //   _eg_manager_.initialize(event_gtor_mconfig);
      // }
      // else {
      //   throw std::logic_error("mctools::g4::manager::reset: Missing primary event generator configuration !");
      // }
      DT_THROW_IF (! _eg_manager_.has(_eg_name_),
                   std::logic_error,
                   "Cannot find primary event generator named '" << _eg_name_ << "' !");
      _event_generator_ = &_eg_manager_.grab(_eg_name_);
      */

      /****************************
       * USER ACTIONS FOR GEANT 4 *
       ****************************/

      // Run manager:
      DT_LOG_NOTICE(_logprio(), "Run manager...");
      _g4_run_manager_ = new G4RunManager;

      /*** User initializations ***/
      DT_LOG_NOTICE(_logprio(), "User initializations...");

      // Detector construction:
      _init_detector_construction ();

      /*
      DT_LOG_NOTICE(_logprio(), "Detector construction...");
      const datatools::properties & detector_construction_config
        = _multi_config_->get("detector_construction").get_properties();
      _user_detector_construction_ = new detector_construction(*this);
      // the SHPF generator :

        // if (_seed_manager_.has_seed(constants::instance().SHPF_LABEL))
        // {
        // int seed = _seed_manager_.get_seed(constants::instance().SHPF_LABEL);
        // _user_detector_construction_->set_SHPF_random_seed(seed);
        // std::clog << datatools::io::notice
        // << "mctools::g4::manager::_at_init: "
        // << "Using registered seed for '"
        // << constants::instance().SHPF_LABEL << "' : "
        // << seed
        // << std::endl;
        // }

      _user_detector_construction_->grab_step_hit_processor_factory().set_external_prng(_shpf_prng_);
      _user_detector_construction_->set_geometry_manager(get_geom_manager());
      _user_detector_construction_->initialize(detector_construction_config);
      _g4_run_manager_->SetUserInitialization(_user_detector_construction_);
      */

      // Physics list:
      _init_physics_list ();
      /*
      DT_LOG_NOTICE(_logprio(), "Physics list...");
      const datatools::properties & physics_list_config
        = _multi_config_->get("physics_list").get_properties();
      _user_physics_list_ = new physics_list;
      _user_physics_list_->initialize(physics_list_config);
      _g4_run_manager_->SetUserInitialization(_user_physics_list_);
      DT_LOG_DEBUG(_logprio(), "Physics list: ");
      if (is_debug()) _user_physics_list_->tree_dump(std::clog);
      */

#ifdef G4VIS_USE
      // G4 visualization:
      _g4_vis_manager_ = 0;
      if (_g4_visualization_) {
        _g4_vis_manager_ = new G4VisExecutive;
        _g4_vis_manager_->Initialize();
      }
#endif

      // User actions:

      // Run action:
      _init_run_action ();
      // DT_LOG_NOTICE(_logprio(), "Run action construction...");
      // const datatools::properties & run_action_config
      //   = _multi_config_->get("run_action").get_properties();
      // _user_run_action_ = new run_action(*this);
      // if (! _output_data_file_.empty()) {
      //   _user_run_action_->set_output_file(_output_data_file_);
      // }
      // if (has_number_of_events_modulo()) {
      //   _user_run_action_->set_number_of_events_modulo(get_number_of_events_modulo());
      // }
      // _user_run_action_->set_use_run_header_footer(using_run_header_footer());
      // _user_run_action_->initialize(run_action_config);
      // _g4_run_manager_->SetUserAction(_user_run_action_);

      // Event action:
      _init_event_action ();
      // DT_LOG_NOTICE(_logprio(), "Event action construction...");
      // const datatools::properties & event_action_config
      //   = _multi_config_->get("event_action").get_properties();
      // _user_event_action_ = new event_action(*_user_run_action_,
      //                                        *_user_detector_construction_);
      // _user_event_action_->initialize(event_action_config);
      // _g4_run_manager_->SetUserAction(_user_event_action_);

      // Primary generator:
      _init_primary_generator_action ();

      // Tracking action:
      _init_tracking_action ();
      // DT_LOG_NOTICE(_logprio(), "Tracking action...");
      // const datatools::properties & tracking_action_config
      //   = _multi_config_->get("tracking_action").get_properties();
      // _user_tracking_action_ = new tracking_action;
      // _user_tracking_action_->initialize(tracking_action_config);
      // _g4_run_manager_->SetUserAction(_user_tracking_action_);

      // Stepping action:
      bool use_stepping_action = false;
      if (use_stepping_action) {
        _init_stepping_action ();
        // DT_LOG_NOTICE(_logprio(), "Stepping action...");
        // const datatools::properties & stepping_action_config
        //   = _multi_config_->get("stepping_action").get_properties();
        // _user_stepping_action_ = new stepping_action;
        // _user_stepping_action_->initialize(stepping_action_config);
        // _g4_run_manager_->SetUserAction(_user_stepping_action_);
      }

      // Stacking action:
      _init_stacking_action ();
      /*
      DT_LOG_NOTICE(_logprio(), "Stacking action...");
      const datatools::properties & stacking_action_config
        = _multi_config_->get("stacking_action").get_properties();
      _user_stacking_action_ = new stacking_action;
      _user_stacking_action_->initialize(stacking_action_config);
      _g4_run_manager_->SetUserAction(_user_stacking_action_);
      */

      // G4 kernel initialization:
      DT_LOG_NOTICE(_logprio(), "G4 kernel initialization...");
      _g4_run_manager_->Initialize();

      _g4_UI_ = G4UImanager::GetUIpointer();

      /*****************
       * FINAL ACTIONS *
       *****************/

      // init the dictionary of PRNG internal state records :
      _init_prngs_states();

      return;
    } // end of manager::_at_init()


    void manager::_at_reset()
    {
      if (_use_time_stat_) {
        for (CT_map::iterator i = _CTs_.begin();
             i != _CTs_.end();
             i++) {
          i->second.tree_dump(std::clog, i->first);
          i->second.reset();
        }
      }

#ifdef G4VIS_USE
      if (_g4_vis_manager_) {
        delete _g4_vis_manager_;
        _g4_vis_manager_ = 0;
      }
#endif
      if (_g4_run_manager_) {
        delete _g4_run_manager_;
        _g4_run_manager_ = 0;
      }
      if (_g4_stepping_verbosity_) {
        delete _g4_stepping_verbosity_;
        _g4_stepping_verbosity_ = 0;
      }

      if (has_event_generator()) {
        _event_generator_ = 0;
      }

      if (has_vertex_generator()) {
        _vertex_generator_ = 0;
      }

      _vg_prng_.reset();
      _eg_prng_.reset();
      _shpf_prng_.reset();
      _mgr_prng_.reset();


      reset_simulation_ctrl();

      _track_history_.reset();
      _init_defaults();
      return;
    }

    event_action & manager::grab_user_event_action()
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Manager is not initialized !");
      return * _user_event_action_;
    }

    void manager::run_simulation()
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Manager is not initialized !");
      DT_LOG_NOTICE(_logprio(),"Starting the simulation...");
      _at_run_simulation();
      DT_LOG_NOTICE(_logprio(),"Simulation stops !");
      return;
    } // end of manager::run_simulation()


    void manager::_at_run_simulation()
    {
      std::string g4_macro;
      if (has_g4_macro()) {
        g4_macro = _g4_macro_;
        datatools::fetch_path_with_env(g4_macro);
        if (! boost::filesystem::exists(g4_macro)) {
          std::ostringstream message;
          message << "Macro '" << g4_macro << "' does not exist !";
          G4Exception("mctools::g4::manager::_at_run_simulation",
                      "FileError",
                      RunMustBeAborted,
                      message.str().c_str());
        }
      }

      // Interactive mode:
      if (is_interactive()) {
        DT_LOG_NOTICE(_logprio(), "Entering interactive mode...");
#ifdef G4VIS_USE
        if (has_g4_visualization()) {
          if (_g4_vis_manager_) {
            //_g4_vis_manager_->SetVerboseLevel("quiet");
          }
        }
#endif

        // G4UIterminal is a(dumb) terminal.
        G4UIsession * g4_session = 0;

#ifdef G4UI_USE_TCSH
        g4_session = new G4UIterminal(new G4UItcsh);
#else
        g4_session = new G4UIterminal();
#endif
        if (! g4_macro.empty()) {
          std::ostringstream command_oss;
          command_oss << "/control/execute ";
          command_oss << g4_macro;
          _g4_UI_->ApplyCommand(command_oss.str());
        }
        g4_session->SessionStart();
        delete g4_session;
      } else { // Batch mode:
        DT_LOG_NOTICE(_logprio(),"Entering batch mode...");

#ifdef G4VIS_USE
        if (has_g4_visualization()) {
          if (_g4_vis_manager_) {
            //_g4_vis_manager_->SetVerboseLevel("quiet");
          }
        }
#endif

        // pure automatic mode: no macro is executed, only beam on
        if (is_automatic()) {
          DT_LOG_NOTICE(_logprio(), "Entering automatic mode...");

          /*    command = "/random/setSavingFlag true";
                UI->ApplyCommand(command);
                cout << "APPLYING RESET" << std::endl;
                command = "/random/resetEngineFrom currentRun.rndm";
                UI->ApplyCommand(command);
          */

          // set tracking verbosity:
          {
            std::ostringstream command_oss;
            command_oss << "/tracking/verbose ";
            command_oss << _g4_tracking_verbosity_;
            _g4_UI_->ApplyCommand(command_oss.str());
          }

          // shoot events:
          {
            std::ostringstream command_oss;
            command_oss << "/run/beamOn ";
            uint32_t effective_number_of_events = constants::instance().NO_LIMIT;
            /*
              if (_number_of_events == constants::instance().NO_LIMIT)
              {
              //command_oss << _number_of_events;
              }
            */
            if (! has_simulation_ctrl()) {
              effective_number_of_events = _number_of_events_;
            }
            DT_THROW_IF ((effective_number_of_events == 0)
                         ||(effective_number_of_events > constants::instance().NO_LIMIT),
                         std::logic_error,
                         "Invalid number of events for Geant4 !");
            command_oss << effective_number_of_events;
            DT_LOG_NOTICE(_logprio(), "G4 command = " << command_oss.str());
            _g4_UI_->ApplyCommand(command_oss.str());
          }
        } else {
          std::ostringstream command_oss;
          command_oss << "/control/execute ";
          command_oss << g4_macro;
          _g4_UI_->ApplyCommand(command_oss.str());
        }
      }

      {
        // 2011-03-03 FM :
        DT_LOG_NOTICE(_logprio(),
                      "Record the final PRNG internal states after the session has stopped normaly.");
        this->record_current_prng_states();
      }

      return;
    } // end of manager::_at_run_simulation()

    void manager::dump_base(std::ostream & out_,
                            const std::string & title_ ,
                            const std::string & indent_) const
    {
      std::string indent;
      if (! indent_.empty()) indent = indent_;
      if (! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }
      out_ << indent << "|-- Debug:        " << is_debug() << std::endl;
      out_ << indent << "|-- Batch:        " << is_batch() << std::endl;
      out_ << indent << "|-- Interactive:  " << is_interactive() << std::endl;
      out_ << indent << "|-- Automatic:    " << is_automatic() << std::endl;
      out_ << indent << "|-- G4 visu.:     " << has_g4_visualization() << std::endl;
      out_ << indent << "|-- G4 macro:     '" << get_g4_macro() << "'" << std::endl;
      out_ << indent << "|-- No events:    " << get_number_of_events() << std::endl;
      out_ << indent << "|-- G4 PRNG seed: " << _mgr_prng_seed_ << " " << std::endl;
      out_ << indent << "|-- G4 PRNG name: '"
           <<(get_mgr_prng().is_initialized()? get_mgr_prng().name(): "[none]")
           << "' " << std::endl;
      out_ << indent << "|-- PRNG seeds input file:   '"
           << _input_prng_seeds_file_ << "' " << std::endl;
      out_ << indent << "|-- PRNG seeds output file:  '"
           << _output_prng_seeds_file_ << "' " << std::endl;
      out_ << indent << "|-- PRNG states input file:  '"
           << _input_prng_states_file_ << "' " << std::endl;
      out_ << indent << "|-- PRNG states output file: '"
           << _output_prng_states_file_ << "' " << std::endl;
      out_ << indent << "`-- end" << std::endl;
      return;
    }

    void manager::dump(std::ostream & out_) const
    {
      dump_base(out_, "mctools::g4::manager: ", "");
      return;
    }

    uint32_t manager::get_number_of_events() const
    {
      return _number_of_events_;
    }

  } // end of namespace g4

} // end of namespace mctools


/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::manager,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::manager");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation manager class");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The Geant4 simulation manager class embedes \n"
                                "a full Geant4 based simulation engine. It can be\n"
                                "configured through a ``datatools::multi_properties`` object.\n"
                                "Several embedded components are used :                  \n"
                                "                                                        \n"
                                " * a service manager (optional)                         \n"
                                " * a geometry manager                                   \n"
                                " * a vertex generator manager (optional)                \n"
                                " * a primary event generator manager                    \n"
                                " * a PRNG seed manager                                  \n"
                                " * a PRNG internal state manager                        \n"
                                " * a Geant4 run manager with embedded components:       \n"
                                "                                                        \n"
                                "   - detector construction                              \n"
                                "   - physics list                                       \n"
                                "   - run action                                         \n"
                                "   - event action                                       \n"
                                "   - primary generator action                           \n"
                                "   - tracking action (optional)                         \n"
                                "   - stepping action (optional)                         \n"
                                "   - stacking action (optional)                         \n"
                                "                                                        \n"
                                " * a Geant4 user interface (UI) manager                 \n"
                                " * a Geant4 visualization manager (optional)            \n"
                                "                                                        \n"
                               );

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_section("manager")
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
                            "  [name=\"manager\"]                                   \n"
                            "  logging.priority : string = \"warning\"              \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'number_of_events' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("number_of_events")
      .set_section("manager")
      .set_terse_description("Number of events to be simulated")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Allowed value: from ``1`` to ``100000000``                              \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "  [name=\"manager\"]                                                    \n"
                            "  number_of_events : integer = 100000                                   \n"
                            "                                                                        \n"
                            "This property is not taken into account if the                          \n"
                            "*number of events* attributes has been set previously through           \n"
                            "the ``mctools::g4::manager::set_number_of_events(...)`` method.         \n"
                            "                                                                        \n"
                            )
      ;
  }

  {
    // Description of the 'g4_macro' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("g4_macro")
      .set_section("manager")
      .set_terse_description("Geant4 macro to be executed")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_long_description("Example::                                                               \n"
                            "                                                                        \n"
                            "  [name=\"manager\"]                                                    \n"
                            "  g4_macro : string as path = \"g4.mac\"                                \n"
                            "                                                                        \n"
                            "This property is not taken into account if a Geant4 *macro*             \n"
                            "has already been set by the simulation manager, through                 \n"
                            "the ``mctools::g4::manager::set_g4_macro(...)`` method.                 \n"
                            "                                                                        \n"
                            )
      ;
  }

  {
    // Description of the geometry 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("geometry")
      .set_terse_description("Geometry manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                               \n"
                            "                                                                        \n"
                            "  [name=\"geometry\"]                                                   \n"
                            "  manager.config : string as path = \"config/geometry/manager.conf\"   \n"
                            "                                                                        \n"
                            "This property is not taken into account if an external                  \n"
                            "*geometry manager* is used by the simulation manager, through           \n"
                            "the ``mctools::g4::manager::set_external_geom_manager(...)`` method.    \n"
                            "                                                                        \n"
                            )
      ;
  }

  {
    // Description of the vertex generator 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("vertex_generator")
      .set_terse_description("Vertex generator manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                                     \n"
                            "                                                                              \n"
                            "  [name=\"vertex_generator\"]                                                 \n"
                            "  manager.config : string as path = \"config/vertex_generator/manager.conf\"  \n"
                            "                                                                              \n"
                            )
      ;
  }

  {
    // Description of the event generator 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("event_generator")
      .set_terse_description("Event generator manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                                     \n"
                            "                                                                              \n"
                            "  [name=\"event_generator\"]                                                  \n"
                            "  manager.config : string as path = \"config/event_generator/manager.conf\"   \n"
                            "                                                                              \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("The simulation manager can be parameterized and initialized           \n"
                               "through a ``datatools::multi_properties`` object.                     \n"
                               "The configuration contains several sections, each of them is          \n"
                               "related to a specific sub-task. A typical layout is::                 \n"
                               "                                                                      \n"
                               "  #@description Configuration of the mctools Geant4 simulation engine \n"
                               "  #@key_label   \"name\"                                              \n"
                               "  #@meta_label  \"\"                                                  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"manager\"]                                                  \n"
                               "  #@config Parameters for the main simulation engine                  \n"
                               "                                                                      \n"
                               "  #@description Manager logging priority                              \n"
                               "  logging.priority : string = \"warning\"                             \n"
                               "                                                                      \n"
                               "  #@description Number of events to be simulated                      \n"
                               "  number_of_events :integer = 10000                                   \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"geometry\"]                                                 \n"
                               "  #@config Parameters for the geometry modelling                      \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``geomtools::manager``                  \n"
                               "  manager.config : string as path = \"config/geometry/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"vertex_generator\"]                                         \n"
                               "  #@config Parameters for the vertex generator                        \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``genvtx::manager``                     \n"
                               "  manager.config : string as path = \"config/vertex_generator/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"event_generator\"]                                          \n"
                               "  #@config Parameters for the GENBB based event generator             \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``genbb::manager``                      \n"
                               "  manager.config : string as path = \"config/event_generator/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"detector_construction\"]                                    \n"
                               "  #@config Parameters for the detector construction                   \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::detector_construction``  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"physics_list\"]                                             \n"
                               "  #@config Parameters for the physics list                            \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::physics_list``           \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"run_action\"]                                               \n"
                               "  #@config Parameters for the run action                              \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::run_action``             \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"event_action\"]                                             \n"
                               "  #@config Parameters for the event action                            \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::event_action``           \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"primary_generator_action\"]                                 \n"
                               "  #@config Parameters for the primary generator action                \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::primary_generator``      \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"tracking_action\"]                                          \n"
                               "  #@config Parameters for the tracking action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::tracking_action``        \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"stepping_action\"]                                          \n"
                               "  #@config Parameters for the stepping action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::stepping_action``        \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"stacking_action\"]                                          \n"
                               "  #@config Parameters for the stacking action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::stacking_action``        \n"
                               "                                                                      \n"
                             );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::manager,"mctools::g4::manager")


// end of manager.cc
