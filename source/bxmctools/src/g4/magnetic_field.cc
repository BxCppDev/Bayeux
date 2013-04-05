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
#include <geomtools/utils.h>
#include <geomtools/manager.h>

#include <emfield/base_electromagnetic_field.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/emfield_geom_plugin.h>

#include <globals.hh>

namespace mctools {

  namespace g4 {
  
    using namespace std;

    /*
      bool magnetic_field::has_geometry_manager() const
      {
      return _geom_manager_ != 0;
      }

      bool magnetic_field::has_mag_field_manager() const
      {
      return _mag_field_manager_ != 0;
      }
    */

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
      if (! has_mag_field()) {
        throw std::logic_error("mctools::g4::magnetic_field::get_mag_field: No magnetic field !");
      }
      return *_mag_field_;
    }

    bool magnetic_field::is_active() const
    {
      return has_mag_field();
    }

    /*
      void magnetic_field::set_geometry_manager(const geomtools::manager & gmgr_)
      {
      if (_initialized_) {
      std::ostringstream message;
      message << "mctools::g4::magnetic_field::set_geometry_manager: "
      << "Already initialized !";
      throw std::logic_error(message.str());
      }
      _geom_manager_ = &gmgr_;
      return;
      }
    */

    /*
      void magnetic_field::set_mag_field_manager(const emfield::electromagnetic_field_manager &emf_mgr_) 
      {
      if (_initialized_) {
      std::ostringstream message;
      message << "mctools::g4::magnetic_field::set_mag_field_manager: "
      << "Already initialized !";
      throw std::logic_error(message.str());
      }
      _mag_field_manager_ = &emf_mgr_;
      return;
      }
    */

    magnetic_field::magnetic_field()
    {
      _initialized_ = false;
      //_geom_manager_ = 0;
      //_mag_field_manager_ = 0;
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
      if (_initialized_) {
        std::ostringstream message;
        message << "mctools::g4::magnetic_field::initialize: "
                << "Already initialized !";
        throw std::logic_error(message.str());
      }
 
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
          }
          // else if (mode_str == "manager") {
          //   mode = 2;
          // }
          else {
            std::ostringstream message;
            message << "mctools::g4::magnetic_field::initialize: "
                    << "Invalid value '" << mode_str << "' for 'mode' property !";
            throw std::logic_error(message.str());
            
          }
        }
        else {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::initialize: "
                  << "Missing 'mode' property !";
          throw std::logic_error(message.str());
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
              if (smg.size() != 3) {
                std::ostringstream message;
                message << "mctools::g4::magnetic_field::initialize: "
                        << "Invalid dimension for vector of constant magnetic field coordinates !";
                throw std::logic_error(message.str());        
              }
              _standalone_constant_mag_field_.set(smg[0], smg[1], smg[2]);
              if (! config_.has_explicit_unit("standalone_constant_mag_field")) {
                _standalone_constant_mag_field_ *= magnetic_field_unit;
                std::clog << "NOTICE: " 
                          << "mctools::g4::magnetic_field::initialize: "
                          << "Forcing unit for standalone constant magnetic field..." 
                          << std::endl;
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

      if (config_.has_key("magnetic_field.check_pos_time")) {
        _mag_field_check_pos_time_ = true;
      }
    
      _initialized_ = true;
      return;
    }

    // G4 interface:
    void magnetic_field::GetFieldValue(const double position_[3],
                                        double * b_field_) const
    {
      if (! _initialized_) {
        std::ostringstream message;
        message << "mctools::g4::magnetic_field::GetFieldValue: "
                << "Not initialized !";
        throw std::logic_error(message.str());
      }
      b_field_[0] = 0.0;
      b_field_[1] = 0.0;
      b_field_[2] = 0.0;
      double time = 0.0;
      if (_mag_field_ != 0) {
        geomtools::vector_3d pos(position_[0], position_[1], position_[2]);
        geomtools::vector_3d b_field;
        if (_mag_field_check_pos_time_ 
            && ! _mag_field_->position_and_time_are_valid(pos, time)) {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::GetFieldValue: "
                  << "Position and time at " 
                  << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] "
                  << " are not valid for magnetic field named '" << _name_ 
                  << "' !";
          throw std::logic_error(message.str());        
        }
        int status = _mag_field_->compute_magnetic_field(pos, time, b_field);
        if (status != 0) {
          std::ostringstream message;
          message << "mctools::g4::magnetic_field::GetFieldValue: "
                  << "Magnetic field named '" << _name_ 
                  << "' cannot compute magnetic field value at " 
                  << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] !";
          throw std::logic_error(message.str());        
        }
        b_field_[0] = b_field.x();
        b_field_[1] = b_field.y();
        b_field_[2] = b_field.z();
      }
      else if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        b_field_[0] = _standalone_constant_mag_field_.x();
        b_field_[1] = _standalone_constant_mag_field_.y();
        b_field_[2] = _standalone_constant_mag_field_.z();      
      }
      return;
    }

    void magnetic_field::dump(ostream & out_) const
    {
      out_ << "magnetic_field:" << endl;
      out_ << "|-- Name                   : '" << _name_ << "'" << endl;
      if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        out_ << "|-- Standalone constant magnetic field : " 
             << _standalone_constant_mag_field_ / CLHEP::gauss 
             << " gauss" << endl;
      }
      //out_ << "|-- Geometry manager       : " << _geom_manager_ << endl;
      //out_ << "|-- Magnetic field manager : " << _mag_field_manager_ << endl;
      else {
        out_ << "|-- Magnetic field         : " << _mag_field_ << endl;
      }
      out_ << "`-- Check field pos/time   : " << _mag_field_check_pos_time_ << endl;
      return;
    }
  
  } // end of namespace g4

} // end of namespace mctools

// end of magnetic_field.cc

