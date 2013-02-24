// -*- mode: c++ ; -*- 
/* linear_combination_field.cc
 */ 

#include <emfield/linear_combination_field.h>

#include <sstream>
#include <stdexcept>
#include <datatools/utils.h>
#include <datatools/properties.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(linear_combination_field, "emfield::linear_combination_field");

  linear_combination_field::combined_field_entry::combined_field_entry ()
  {
    datatools::invalidate (weight);
    return;
  }

  linear_combination_field::combined_field_entry::~combined_field_entry ()
  {
    field_handle.reset ();
    return;
  }
  
  // Constructor :
  EMFIELD_CONSTRUCTOR_IMPLEMENT_HEAD(linear_combination_field,flags_)     
  {
    _set_electric_field (true);
    _set_electric_field_can_be_combined (false);
    _set_magnetic_field (false);
    _set_magnetic_field_can_be_combined (false);
    return;
  }  
  
  // Destructor :
  EMFIELD_DEFAULT_DESTRUCTOR_IMPLEMENT(linear_combination_field);
    
  void linear_combination_field::add_combined_field (const std::string & label_,
                                                     base_electromagnetic_field::handle_type & field_handle_, 
                                                     double weight_,
                                                     bool force_combined_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::linear_combination_field::add_combined_field: "
                << "Cannot add any additional combined EM field !";
        throw std::logic_error (message.str ());
      }
    if (! force_combined_)
      {
        // Check if the field can be combined :
        if (field_handle_.get ().is_electric_field () 
            && ! field_handle_.get ().electric_field_can_be_combined ())
          {
            std::ostringstream message;
            message << "emfield::linear_combination_field::add_combined_field: "
                    << "Electric field cannot be combined !";
            throw std::logic_error (message.str ());        
          }
        if (field_handle_.get ().is_magnetic_field () 
            && ! field_handle_.get ().magnetic_field_can_be_combined ())
          {
            std::ostringstream message;
            message << "emfield::linear_combination_field::add_combined_field: "
                    << "Magnetic field cannot be combined !";
            throw std::logic_error (message.str ());        
          }
      }
    if (field_handle_.get ().is_electric_field ())
      {
        _set_electric_field (true);
      }
    if (field_handle_.get ().is_magnetic_field ())
      {
        _set_magnetic_field (true);
      }
    if (_combined_fields_.find (label_) != _combined_fields_.end ())
      {
        std::ostringstream message;
        message << "emfield::linear_combination_field::add_combined_field: "
                << "Combined EM field labelled '" << label_ << "' already exists !";
        throw std::logic_error (message.str ());                
      }
    combined_field_entry etmp;
    etmp.label = label_;
    etmp.field_handle = field_handle_;
    etmp.weight = weight_; 
    _combined_fields_[label_] = etmp;
    return;
  }
  
  EMFIELD_COMPUTE_EFIELD_IMPLEMENT_HEAD(linear_combination_field,
                                        position_, 
                                        time_, 
                                        electric_field_)
  {
    geomtools::invalidate (electric_field_);
    if (! is_electric_field ())
      {
        return STATUS_ERROR;
      }
    electric_field_.set (0., 0., 0.);
    for (combined_field_dict_type::iterator i = _combined_fields_.begin ();
         i != _combined_fields_.end ();
         i++)
      {
        combined_field_entry & cfe = i->second;
        base_electromagnetic_field & emf = cfe.field_handle.grab ();
        if (emf.is_electric_field ())
          {
            geomtools::vector_3d etmp;
            int status = emf.compute_electric_field (position_, time_, etmp);
            if (status != STATUS_SUCCESS)
              {
                return status;
              }
            electric_field_ += cfe.weight * etmp;
          }
      }
    return STATUS_SUCCESS;
  } 
  
  EMFIELD_COMPUTE_BFIELD_IMPLEMENT_HEAD(linear_combination_field,
                                        position_, 
                                        time_, 
                                        magnetic_field_)
  {
    geomtools::invalidate (magnetic_field_);
    if (! is_magnetic_field ())
      {
        return STATUS_ERROR;
      }
    magnetic_field_.set (0., 0., 0.);
    for (combined_field_dict_type::iterator i = _combined_fields_.begin ();
         i != _combined_fields_.end ();
         i++)
      {
        combined_field_entry & cfe = i->second;
        base_electromagnetic_field & emf = cfe.field_handle.grab ();
        if (emf.is_magnetic_field ())
          {
            geomtools::vector_3d btmp;
            int status = emf.compute_magnetic_field (position_, time_, btmp);
            if (status != STATUS_SUCCESS)
              {
                return status;
              }
            magnetic_field_ += cfe.weight * btmp;
          }
      }
    return STATUS_SUCCESS;
   } 

  EMFIELD_RESET_IMPLEMENT_HEAD(linear_combination_field)
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::linear_combination_field::reset: "
                << "Cannot reset the linear combination EM field !";
        throw std::logic_error (message.str ());
      }

    _combined_fields_.clear ();

    _set_initialized (false);
    return;
  } 
  
  EMFIELD_INITIALIZE_IMPLEMENT_HEAD(linear_combination_field,setup_,service_manager_,fields_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::linear_combination_field::initialized: "
                << "Field is already initialized !";
        throw std::logic_error (message.str ());
      }

    base_electromagnetic_field::_parse_basic_parameters (setup_, service_manager_, fields_);

    std::vector<std::string> combined_field_labels;
    if (setup_.has_key ("combined_fields.labels"))
      {
        setup_.fetch ("combined_fields.labels", combined_field_labels);
      }    

    for (int i = 0; i < combined_field_labels.size (); i++)
      {
        const std::string & combined_field_label = combined_field_labels[i];
        
        std::string combined_field_name_key;
        std::string combined_field_weight_key;
        {
          std::ostringstream combined_field_name_oss;
          combined_field_name_oss << "combined_fields." << combined_field_label << ".name";
          combined_field_name_key = combined_field_name_oss.str ();
          if (! setup_.has_key (combined_field_name_key))
            {
              std::ostringstream message;
              message << "emfield::linear_combination_field::initialized: "
                      << "Missing field name for combined field labelled '" << combined_field_label << "' !";
              throw std::logic_error (message.str ());
            }
        }

        {
          std::ostringstream combined_field_weight_oss;
          combined_field_weight_oss << "combined_fields." << combined_field_label << ".weight";
          combined_field_weight_key = combined_field_weight_oss.str ();
          if (! setup_.has_key (combined_field_weight_key))
            {
              std::ostringstream message;
              message << "emfield::linear_combination_field::initialized: "
                      << "Missing field weight for combined field labelled '" << combined_field_label << "' !";
              throw std::logic_error (message.str ());
            }
        }

        bool force_combined = false;
        {
          std::ostringstream combined_field_force_combined_oss;
          combined_field_force_combined_oss << "combined_fields." << combined_field_label << ".force_combined";
          std::string combined_field_force_combined_key = combined_field_force_combined_oss.str ();
          if (setup_.has_flag (combined_field_force_combined_key))
            {
              force_combined = true;
            }
        }

        std::string combined_field_name = setup_.fetch_string (combined_field_name_key);
        double combined_field_weight = setup_.fetch_real (combined_field_weight_key);
        {
          base_electromagnetic_field::field_dict_type::iterator field_found = fields_.find (combined_field_name);
          if (field_found == fields_.end ())
            {
              std::ostringstream message;
              message << "emfield::linear_combination_field::initialized: "
                      << "Cannot find a EM field with name '" << combined_field_name 
                      << "' for the combined field labelled '" << combined_field_label << "' !";
              throw std::logic_error (message.str ());
            }
          add_combined_field (combined_field_label, 
                              field_found->second, 
                              combined_field_weight, 
                              force_combined);
        }
        
      }

    _set_initialized (true);
    return;
  }

} // end of namespace emfield

// end of linear_combination_field.h
