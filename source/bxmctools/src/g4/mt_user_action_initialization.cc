/// \file mctools/g4/mt_user_action_initialization.cc
/*
 * Copyright (C) 2022 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */
// Ourselves:
#include <mctools/g4/mt_user_action_initialization.h>

// This project:
#include <mctools/g4/manager.h>
#include <mctools/g4/run_action.h>
#include <mctools/g4/event_action.h>
#include <mctools/g4/primary_generator.h>
#include <mctools/g4/tracking_action.h>
#include <mctools/g4/stepping_action.h>
#include <mctools/g4/stacking_action.h>

namespace mctools {
  namespace g4 {

    mt_user_action_initialization::mt_user_action_initialization(manager & mgr_)
    {
      _manager_ = &mgr_;
      _multi_config_ = &mgr_.get_multi_config();
      return;
    }

    void mt_user_action_initialization::Build() const
    {
      auto mutableThis = const_cast<mt_user_action_initialization*>(this);
      mutableThis->_init_run_action_();
      mutableThis->_init_event_action_();
      mutableThis->_init_primary_generator_action_();
      mutableThis->_init_tracking_action_();
      mutableThis->_init_stepping_action_();
      mutableThis->_init_stacking_action_();
      return;
    }
  
    void mt_user_action_initialization::BuildForMaster() const
    {
      auto mutableThis = const_cast<mt_user_action_initialization*>(this);
      mutableThis->_init_run_action_();
      return;
    }
     
    void mt_user_action_initialization::_init_run_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Run action initialization...");
      DT_THROW_IF(! _multi_config_->has_section("run_action"),
                  std::logic_error,
                  "Missing run action configuration !");
      const datatools::properties & run_action_config
        = _multi_config_->get("run_action").get_properties();
      _user_run_action_ = new run_action(*_manager_);
      if (_manager_->get_output_data_format() == io_utils::DATA_FORMAT_INVALID) {
        _manager_->set_output_data_format(io_utils::DATA_FORMAT_PLAIN);
      }
      _user_run_action_->set_output_data_format(_manager_->get_output_data_format());
      if (_manager_->get_output_data_format() == io_utils::DATA_FORMAT_BANK) {
        _user_run_action_->set_output_data_bank_label(_manager_->get_output_data_bank_label());
      }
      if (! _manager_->get_output_data_file().empty()) {
        _user_run_action_->set_output_file(_manager_->get_output_data_file());
      }
      if (_manager_->has_number_of_events_modulo()) {
        _user_run_action_->set_number_of_events_modulo(_manager_->get_number_of_events_modulo());
      }
      _user_run_action_->set_use_run_header_footer(_manager_->using_run_header_footer());
      _user_run_action_->initialize(run_action_config);
      SetUserAction(_user_run_action_);
      return;
    }
    
    void mt_user_action_initialization::_init_event_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Event action initialization...");
      DT_THROW_IF(! _multi_config_->has_section("event_action"),
                  std::logic_error,
                  "Missing event action configuration !");
      const datatools::properties & event_action_config
        = _multi_config_->get("event_action").get_properties();
      _user_event_action_ = new event_action(*_user_run_action_,
                                             _manager_->get_user_detector_construction());
      _user_event_action_->initialize(event_action_config);
      SetUserAction(_user_event_action_);
      return;
    }

    void mt_user_action_initialization::_init_primary_generator_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Primary generator action initialization...");
      DT_THROW_IF(!_multi_config_->has_section("primary_generator_action"),
                  std::logic_error,
                  "Missing primary event generator action configuration !");
      const datatools::properties & primary_generator_config
        = _multi_config_->get("primary_generator_action").get_properties();

      _user_primary_generator_ = new primary_generator;
      _user_primary_generator_->set_run_action(*_user_run_action_);
      _user_primary_generator_->set_event_action(*_user_event_action_);
      if (_manager_->has_vertex_generator()) {
        _user_primary_generator_->set_vertex_generator(_manager_->grab_vertex_generator());
      }
      _user_primary_generator_->set_event_generator(_manager_->grab_event_generator());
      _user_primary_generator_->initialize(primary_generator_config);
      SetUserAction(_user_primary_generator_);
      return;
    }
     
    void mt_user_action_initialization::_init_tracking_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Tracking action initialization...");
      if (_multi_config_->has_section("tracking_action")) {
        const datatools::properties & tracking_action_config  = _multi_config_->get("tracking_action").get_properties();
        _user_tracking_action_ = new tracking_action;
        _user_tracking_action_->initialize(tracking_action_config);
        SetUserAction(_user_tracking_action_);
      }
      return;
    }
     
    void mt_user_action_initialization::_init_stepping_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Stepping action initialization...");
      if (_multi_config_->has_section("stepping_action")) {
        const datatools::properties & stepping_action_config = _multi_config_->get("stepping_action").get_properties();
        _user_stepping_action_ = new stepping_action;
        _user_stepping_action_->initialize(stepping_action_config);
        SetUserAction(_user_stepping_action_);
      }
      return;
    }
     
    void mt_user_action_initialization::_init_stacking_action_()
    {
      DT_LOG_NOTICE(_manager_->get_logging_priority(), "Stacking action initialization...");
      if (_multi_config_->has_section("stacking_action")) {
        const datatools::properties & stacking_action_config = _multi_config_->get("stacking_action").get_properties();
        _user_stacking_action_ = new stacking_action;
        _user_stacking_action_->initialize(stacking_action_config);
        SetUserAction(_user_stacking_action_);
      }
      return;
    }
  
  } // end of namespace g4
} // end of namespace mctools
