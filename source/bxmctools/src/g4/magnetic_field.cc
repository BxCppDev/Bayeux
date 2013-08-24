// -*- mode: c++ ; -*-
/* magnetic_field.cc
 */

#include <mctools/g4/magnetic_field.h>

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>

#include <geomtools/utils.h>
#include <geomtools/manager.h>

#include <emfield/base_electromagnetic_field.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/emfield_geom_plugin.h>

#include <globals.hh>

namespace mctools {

  namespace g4 {

    bool magnetic_field::has_name() const
    {
      return ! _name_.empty();
    }

    void magnetic_field::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    const std::string & magnetic_field::get_name() const
    {
      return _name_;
    }

    void magnetic_field::set_mag_field_check_pos_time(bool c_)
    {
      _mag_field_check_pos_time_ = c_;
      return;
    }

    bool magnetic_field::is_mag_field_check_pos_time() const
    {
      return _mag_field_check_pos_time_;
    }

    bool magnetic_field::is_initialized() const
    {
      return _initialized_;
    }

    bool magnetic_field::has_mag_field() const
    {
      return _mag_field_ != 0;
    }

    void magnetic_field::set_mag_field(const emfield::base_electromagnetic_field & mf_)
    {
      _mag_field_ = &mf_;
      return;
    }

    const emfield::base_electromagnetic_field &
    magnetic_field::get_mag_field() const
    {
      DT_THROW_IF (! has_mag_field(), std::logic_error, "No magnetic field !");
      return *_mag_field_;
    }

    bool magnetic_field::is_active() const
    {
      return has_mag_field();
    }
    magnetic_field::magnetic_field()
    {
      _initialized_ = false;
      _mag_field_ = 0;
      _mag_field_check_pos_time_ = 0;
      geomtools::invalidate(_standalone_constant_mag_field_);
      return;
    }

    magnetic_field::~magnetic_field()
    {
      return;
    }

    void magnetic_field::initialize()
    {
      datatools::properties empty;
      initialize(empty);
      return;
    }

    void magnetic_field::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF (_initialized_,std::logic_error,"Geant4 magnetic field '" << get_name() << "' is a lready initialized !");

      loggable_support::_initialize_logging_support(config_);

      if (_name_.empty()) {
        if (config_.has_key("name")) {
          std::string name = config_.fetch_string("name");
          set_name(name);
        }
      }

      if (! has_mag_field()) {
        double magnetic_field_unit = CLHEP::tesla;

        int mode = 0;
        if (config_.has_key("mode")) {
          std::string mode_str = config_.fetch_string("mode");
          if (mode_str == "standalone") {
            mode = 1;
          } else {
            DT_THROW_IF (true, std::logic_error,
                         "Invalid value '" << mode_str << "' for 'mode' property in Geant4 magnetic field '" << get_name() << "' !");
          }
        } else {
          std::ostringstream message;
          DT_THROW_IF (true, std::logic_error, "Missing 'mode' property in Geant4 magnetic field '" << get_name() << "' !");
        }

        if (mode == 1) {
          if (config_.has_key("magnetic_field_unit")) {
            std::string mfu = config_.fetch_string("magnetic_field_unit");
            magnetic_field_unit = datatools::units::get_magnetic_field_unit_from(mfu);
          }

          if (! geomtools::is_valid(_standalone_constant_mag_field_)) {
            if (config_.has_key("standalone_constant_mag_field")) {
              std::vector<double> smg;
              config_.fetch("standalone_constant_mag_field", smg);
              DT_THROW_IF (smg.size() != 3, std::logic_error,
                           "Invalid dimension for vector of constant magnetic field coordinates in Geant4 magnetic field '" << get_name() << "' !");
              _standalone_constant_mag_field_.set(smg[0], smg[1], smg[2]);
              if (! config_.has_explicit_unit("standalone_constant_mag_field")) {
                _standalone_constant_mag_field_ *= magnetic_field_unit;
                DT_LOG_NOTICE(_logprio(), "Forcing unit for standalone constant magnetic field in Geant4 magnetic field '" << get_name() << "'...");
              }
            }
          }
        }

        /*
          if (mode == 2) {
          //! geomtools::is_valid(_standalone_constant_mag_field_))
          if (! has_mag_field_manager()) {
          if (_geom_manager_ == 0) {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "Missing geometry manager !";
          throw std::logic_error(message.str());
          }
          std::string mag_field_plugin_name;
          if (config_.has_key("magnetic_field.geom_plugin")) {
          mag_field_plugin_name = config_.fetch_string("magnetic_field.geom_plugin");
          }
          else {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "Missing field plugin name property '" << "magnetic_field.geom_plugin" << "' !";
          throw std::logic_error(message.str());
          }

          if (_geom_manager_->has_plugin(mag_field_plugin_name)
          && _geom_manager_->is_plugin_a<emfield::emfield_geom_plugin>(mag_field_plugin_name)) {
          std::clog << "NOTICE: " << "Found EM field geometry plugin named '"
          << mag_field_plugin_name
          << "'" << std::endl;
          const emfield::emfield_geom_plugin & EMFGP
          = _geom_manager_->get_plugin<emfield::emfield_geom_plugin>(mag_field_plugin_name);
          _mag_field_manager_ = &EMFGP.get_manager();
          }
          else {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "No EM field geometry plugin named '"
          << mag_field_plugin_name << "' !";
          throw std::logic_error(message.str());
          }
          }

          if (config_.has_key("magnetic_field.name")) {
          _mag_field_name_ = config_.fetch_string("magnetic_field.name");
          }
          else {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "Missing field name property '" << "magnetic_field.name" << "' !";
          throw std::logic_error(message.str());
          }

          if (! _mag_field_manager_->has_field(_mag_field_name_)) {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "No field named '" << _mag_field_name_ << "' !";
          throw std::logic_error(message.str());
          }
          _mag_field_ = &_mag_field_manager_->get_field(_mag_field_name_);

          if (! _mag_field_->is_magnetic_field()) {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
          << "Field '" << _mag_field_name_ << "' is not a magnetic field !";
          throw std::logic_error(message.str());
          }
          }
        */

      }

      if (config_.has_flag("magnetic_field.check_pos_time")) {
        _mag_field_check_pos_time_ = true;
      }

      _initialized_ = true;
      return;
    }

    // G4 interface:
    void magnetic_field::GetFieldValue(const double position_[4],
                                       double * b_field_) const
    {
      DT_LOG_TRACE(_logprio(), "Entering GetFieldValue for Geant4 magnetic field '" << get_name() << "'...");
      DT_THROW_IF (! _initialized_, std::logic_error,
                   "Geant4 magnetic field '" << get_name() << "' is not initialized !");
      b_field_[0] = 0.0;
      b_field_[1] = 0.0;
      b_field_[2] = 0.0;
      if (_mag_field_ != 0) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Compute magnetic field for Geant4 magnetic field '" << get_name() << "'...");
        geomtools::vector_3d pos(position_[0], position_[1], position_[2]);
        double time = position_[3];
        geomtools::vector_3d the_b_field;
        if (_mag_field_check_pos_time_) {
          DT_THROW_IF(! _mag_field_->position_and_time_are_valid(pos, time),
                      std::logic_error,
                      "Position and time at "
                      << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] "
                      << " are not valid for magnetic field named '" << _name_
                      << "' !");
        }
        int status = _mag_field_->compute_magnetic_field(pos, time, the_b_field);
        DT_THROW_IF (status != 0,  std::logic_error,
                     "Magnetic field named '" << _name_
                     << "' cannot compute magnetic field value at "
                     << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] !");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Geant4 magnetic field '" << get_name() << "' is : " << the_b_field / CLHEP::gauss << " gauss");
        b_field_[0] = the_b_field.x();
        b_field_[1] = the_b_field.y();
        b_field_[2] = the_b_field.z();
      } else if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        b_field_[0] = _standalone_constant_mag_field_.x();
        b_field_[1] = _standalone_constant_mag_field_.y();
        b_field_[2] = _standalone_constant_mag_field_.z();
      }
      DT_LOG_TRACE(_logprio(), "Exiting GetFieldValue for Geant4 magnetic field '" << get_name() << "'.");
      return;
    }

    void magnetic_field::dump(std::ostream & out_) const
    {
      out_ << "magnetic_field:" << std::endl;
      out_ << "|-- Name                   : '" << _name_ << "'" << std::endl;
      if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        out_ << "|-- Standalone constant magnetic field : "
             << _standalone_constant_mag_field_ / CLHEP::gauss
             << " gauss" << std::endl;
      } else {
        out_ << "|-- Magnetic field         : " << _mag_field_ << std::endl;
      }
      out_ << "`-- Check field pos/time   : " << _mag_field_check_pos_time_ << std::endl;
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of magnetic_field.cc
