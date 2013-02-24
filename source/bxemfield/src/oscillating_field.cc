// -*- mode: c++ ; -*- 
/* oscillating_field.cc
 */ 

#include <emfield/oscillating_field.h>
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
#include <geomtools/utils.h>
#include <cmath>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(oscillating_field, "emfield::oscillating_field");
  
  // Constructor :
  EMFIELD_CONSTRUCTOR_IMPLEMENT_HEAD(oscillating_field,flags_)     
  {
    _set_electric_field (true);
    _set_electric_field_can_be_combined (false);
    _set_magnetic_field (false);
    _set_magnetic_field_can_be_combined (false);
    _set_electric_field_is_time_dependent (true);
    _set_magnetic_field_is_time_dependent (true);
    datatools::invalidate (_frequency_);
    datatools::invalidate (_phase_);
    return;
  }  
  
  // Destructor :
  EMFIELD_DEFAULT_DESTRUCTOR_IMPLEMENT(oscillating_field);
    
  double oscillating_field::get_frequency () const
  {
    return _frequency_;
  }
    
  double oscillating_field::get_period () const
  {
    return 1. / _frequency_;
  }
   
  double oscillating_field::get_phase () const
  {
    return _phase_;
  }

  void oscillating_field::set_frequency (double f_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::oscillating_field::set_frequency: "
                << "Cannot change the frequency value !";
        throw std::logic_error (message.str ());
      }
    _frequency_ = f_;
    return;
  }

  void oscillating_field::set_phase (double p_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::oscillating_field::set_phase: "
                << "Cannot change the phase value !";
        throw std::logic_error (message.str ());
      }
    _phase_ = p_;
    return;
  }

  void oscillating_field::set_field (base_electromagnetic_field::handle_type & f_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::oscillating_field::set_field: "
                << "Cannot change the EM field !";
        throw std::logic_error (message.str ());
      }
    _field_ = f_;
    return;
  }
  
  EMFIELD_COMPUTE_EFIELD_IMPLEMENT_HEAD(oscillating_field,
                                        position_, 
                                        time_, 
                                        electric_field_)
  {
    geomtools::invalidate (electric_field_);
    if (! _field_.get ().is_electric_field ())
      {
        return STATUS_ERROR;
      }
    int status = _field_.grab ().compute_electric_field (position_, 
                                                         time_, 
                                                         electric_field_);
    if (status != STATUS_SUCCESS)
      {
        return status;
      }
    electric_field_ *= std::cos ( 2.* M_PI * _frequency_ * time_ + _phase_);
    return STATUS_SUCCESS;
  } 

  EMFIELD_COMPUTE_BFIELD_IMPLEMENT_HEAD(oscillating_field,
                                        position_, 
                                        time_, 
                                        magnetic_field_)
  {
    geomtools::invalidate (magnetic_field_);
    if (! _field_.get ().is_magnetic_field ())
      {
        return STATUS_ERROR;
      }
    int status = _field_.grab ().compute_magnetic_field (position_, 
                                                         time_, 
                                                         magnetic_field_);
    if (status != STATUS_SUCCESS)
      {
        return status;
      }
    magnetic_field_ *= std::cos ( 2.* M_PI * _frequency_ * time_ + _phase_);
    return STATUS_SUCCESS;
  } 

  EMFIELD_RESET_IMPLEMENT_HEAD(oscillating_field)
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::oscillating_field::reset: "
                << "Cannot reset the electromagnetic field !";
        throw std::logic_error (message.str ());
      }

    datatools::invalidate (_frequency_);
    datatools::invalidate (_phase_);
    _field_.reset ();

    _set_initialized (false);
    return;
  }

  EMFIELD_INITIALIZE_IMPLEMENT_HEAD(oscillating_field,setup_,service_manager_,fields_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::oscillating_field::initialized: "
                << "Field is already initialized !";
        throw std::logic_error (message.str ());
      }

    double e_unit = CLHEP::volt / CLHEP::meter;

    base_electromagnetic_field::_parse_basic_parameters (setup_, service_manager_, fields_);

    double frequency;
    double phase;
    double phase_unit = CLHEP::degree;
    double frequency_unit = CLHEP::hertz;
    
    if (setup_.has_key ("phase.unit"))
      {
        std::string phase_unit_str = setup_.fetch_string ("phase.unit");
        phase_unit = datatools::units::get_angle_unit_from (phase_unit_str);
      }
    
    if (setup_.has_key ("frequency.unit"))
      {
        std::string frequency_unit_str = setup_.fetch_string ("frequency.unit");
        frequency_unit = datatools::units::get_frequency_unit_from (frequency_unit_str);
      }

    // parameters of the placement field:
    std::string field_name;

    if (setup_.has_key ("field.name"))
      {
        field_name = setup_.fetch_string ("field.name");
      }
 
    if (setup_.has_key ("frequency"))
      {
        frequency = setup_.fetch_real ("frequency");
      }
    
    if (setup_.has_key ("phase"))
      {
        phase = setup_.fetch_real ("phase");
      }

    frequency *= frequency_unit; 
    phase *= phase_unit; 
 
    {
      base_electromagnetic_field::field_dict_type::iterator field_found 
        = fields_.find (field_name);
      if (field_found == fields_.end ())
        {
          std::ostringstream message;
          message << "emfield::oscillating_field::initialized: "
                  << "Cannot find a EM field with name '" << field_name << "' !";
          throw std::logic_error (message.str ());
        }
      set_field (field_found->second);
    }
    _set_electric_field (_field_.get().is_electric_field ());
    _set_magnetic_field (_field_.get().is_magnetic_field ());
    set_frequency (frequency);
    set_phase (phase);
 
    _set_initialized (true);
    return;
  } 
   
} // end of namespace emfield

// end of oscillating_field.cc
