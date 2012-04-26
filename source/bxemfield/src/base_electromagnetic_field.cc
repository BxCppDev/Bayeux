// -*- mode: c++ ; -*- 
/* base_electromagnetic_field.cc
 */ 

#include <emfield/base_electromagnetic_field.h>
#include <datatools/utils/properties.h>
#include <datatools/services/service_manager.h>
#include <datatools/utils/utils.h>
#include <geomtools/utils.h>

namespace emfield {

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_electromagnetic_field,"emfield::base_electromagnetic_field/__system__");

  void base_electromagnetic_field::tree_dump (std::ostream & out_,
                                              const std::string & title_,
                                              const std::string & indent_,
                                              bool inherit_) const
  {
    using namespace std;
    namespace du = datatools::utils;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << du::i_tree_dumpable::tag
         << "Initialized  : " <<  (_initialized_? "Yes": "No") << endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Debug  : " <<  (_debug_? "Yes": "No") << endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Error  : " <<  (_error_? "Yes": "No") << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Electric field : " <<  (_electric_field_? "Yes": "No") << endl;
      
      out_ << indent << du::i_tree_dumpable::skip_tag << du::i_tree_dumpable::last_tag
           << "Can be combined : " <<  (_electric_field_can_be_combined_? "Yes": "No") << endl;
    }

    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
           << "Magnetic field : " <<  (_magnetic_field_? "Yes": "No") << endl;
      
      out_ << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_)
           << du::i_tree_dumpable::last_tag
           << "Can be combined : " <<  (_magnetic_field_can_be_combined_? "Yes": "No") << endl;
    }

    return;
  }

  bool base_electromagnetic_field::is_initialized () const
  {
    return _initialized_;
  }


  void base_electromagnetic_field::_set_initialized (bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }


  bool base_electromagnetic_field::is_error () const
  {
    return _error_;
  }


  void base_electromagnetic_field::_set_error (bool error_)
  {
    _error_ = error_;
    return;
  }


  void base_electromagnetic_field::_set_electric_field (bool ef_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::_set_electric_field: "
                << "Cannot change the electric field flag !";
        throw std::logic_error (message.str ());
      }
    _electric_field_ = ef_;
    return;
  }


  bool base_electromagnetic_field::electric_field_can_be_combined () const
  {
    return _electric_field_can_be_combined_;
  }

  bool base_electromagnetic_field::magnetic_field_can_be_combined () const
  {
    return _magnetic_field_can_be_combined_;
  }

  void base_electromagnetic_field::_set_electric_field_can_be_combined (bool efcbc_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::_set_electric_field_can_be_combined: "
                << "Cannot change the electric field combination flag !";
        throw std::logic_error (message.str ());
      }
    _electric_field_can_be_combined_ = efcbc_;
    return;
  }

  void base_electromagnetic_field::_set_magnetic_field (bool mf_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::_set_magnetic_field: "
                << "Cannot change the magnetic field flag !";
        throw std::logic_error (message.str ());
      }
    _magnetic_field_ = mf_;
    return;
  }


  void base_electromagnetic_field::_set_magnetic_field_can_be_combined (bool efcbc_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::_set_magnetic_field_can_be_combined: "
                << "Cannot change the magnetic field combination flag !";
        throw std::logic_error (message.str ());
      }
    _magnetic_field_can_be_combined_ = efcbc_;
    return;
  }

  bool base_electromagnetic_field::is_debug () const
  {
    return _debug_;
  }


  void base_electromagnetic_field::set_debug (bool debug_)
  {
    _debug_ = debug_;
    return;
  }


  bool base_electromagnetic_field::is_electric_field () const
  {
    return _electric_field_;
  }


  bool base_electromagnetic_field::is_magnetic_field () const
  {
    return _magnetic_field_;
  }


  void base_electromagnetic_field::_parse_basic_parameters (const datatools::utils::properties & setup_,
                                                            datatools::service::service_manager & service_manager_,
                                                            field_dict_type & dictionary_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::_parse_basic_parameters: "
                << "Cannot setup configuration parameters at this stage ! Object is already initialized !";
        throw std::logic_error (message.str ());
      }
    
    if (setup_.has_flag ("debug"))
      {
        set_debug (true);
      }
    
    if (setup_.has_flag ("electric_field.can_be_combined"))
      {
        _set_electric_field_can_be_combined (true);
      }
    
    if (setup_.has_flag ("magnetic_field.can_be_combined"))
      {
        _set_magnetic_field_can_be_combined (true);
      }

    return;
  }

  // Reset :
  void base_electromagnetic_field::_terminate_ ()
  {
    _initialized_ = false;
    _debug_ = false;
    _error_ = false;
    _electric_field_ = false;
    _magnetic_field_ = false;
    _electric_field_can_be_combined_ = false;
    _magnetic_field_can_be_combined_ = false;
    return;
  }


  // Constructor :
  base_electromagnetic_field::base_electromagnetic_field (uint32_t flags_)
  {
    _initialized_ = false;
    _debug_ = false;
    _error_ = false;
    _electric_field_ = false;
    _magnetic_field_ = false;
    _electric_field_can_be_combined_ = false;
    _magnetic_field_can_be_combined_ = false;
    // Special values :
    _debug_ = flags_ & DEBUG;
    _electric_field_ = flags_ & ELECTRIC_FIELD;
    _magnetic_field_ = flags_ & MAGNETIC_FIELD;
    _electric_field_can_be_combined_ = flags_ & ELECTRIC_FIELD_CAN_BE_COMBINED;
    _magnetic_field_can_be_combined_ = flags_ & MAGNETIC_FIELD_CAN_BE_COMBINED;
    return;
  }

  
  // Destructor :
  base_electromagnetic_field::~base_electromagnetic_field ()
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::base_electromagnetic_field::~base_electromagnetic_field: "
                << "EM field still has its 'initialized' flag on ! The '::reset' method has not been invoked from the daughter class' destructor !"
                << "Possible bug !";
        throw std::logic_error (message.str ());
      }
    _terminate_ ();
    return;
  }

  bool base_electromagnetic_field::position_and_time_are_valid (const geomtools::vector_3d & position_, 
                                                                double time_) const
  {
    return geomtools::is_valid (position_) && datatools::utils::is_valid (time_);
  }


  int base_electromagnetic_field::compute_electromagnetic_field (const geomtools::vector_3d & position_,
                                                                 double time_,
                                                                 geomtools::vector_3d & electric_field_, 
                                                                 geomtools::vector_3d & magnetic_field_)
  {
    geomtools::invalidate (electric_field_);
    geomtools::invalidate (magnetic_field_);
    if (! is_initialized ())
      {
        return STATUS_NOT_INITIALIZED;
      }
    if (! is_error ())
      {
        return STATUS_ERROR;
      }
    if (! is_electric_field ())
      {
        return STATUS_NO_ELECTRIC_FIELD;
      }

    int se = compute_electric_field (position_, time_, electric_field_);
    if (se != STATUS_SUCCESS) return se;

    if (! is_magnetic_field ())
      {
        return STATUS_NO_MAGNETIC_FIELD;
      }

    int sb = compute_magnetic_field (position_, time_, magnetic_field_);
    return sb;
  }


  int base_electromagnetic_field::compute_electric_field (const geomtools::vector_3d & position_, 
                                                          double time_, 
                                                          geomtools::vector_3d & electric_field_) 
  {
    geomtools::invalidate (electric_field_);
    if (! is_initialized ())
      {
        return STATUS_NOT_INITIALIZED;
      }
    if (! is_error ())
      {
        return STATUS_ERROR;
      }
    if (! is_electric_field ()) 
      {
        return STATUS_NO_ELECTRIC_FIELD;
      }
    if (! position_and_time_are_valid (position_, time_))
      {
        return STATUS_INVALID_POSITION_TIME;
      }
    electric_field_.set (0., 0., 0.);
    return STATUS_SUCCESS;
  }
  

  int base_electromagnetic_field::compute_magnetic_field (const geomtools::vector_3d & position_, 
                                                          double time_, 
                                                          geomtools::vector_3d & magnetic_field_) 
  {
    geomtools::invalidate (magnetic_field_);
    if (! is_initialized ())
      {
        return STATUS_NOT_INITIALIZED;
      }
    if (! is_error ())
      {
        return STATUS_ERROR;
      }
    if (! is_magnetic_field ()) return STATUS_NO_MAGNETIC_FIELD;
    if (! position_and_time_are_valid (position_, time_))
      {
        return STATUS_INVALID_POSITION_TIME;
      }
    magnetic_field_.set (0., 0., 0.);
    return STATUS_SUCCESS;
  }


  int base_electromagnetic_field::compute_field (char what_, 
                                                 const geomtools::vector_3d & position_,
                                                 double time_,
                                                 geomtools::vector_3d & field_)
  {
    /*
    if (! position_and_time_are_valid (position_, time_))
      {
        return invalid_position_time;
      }
    */
    if (what_ == ELECTRIC_FIELD_LABEL)
      {
        if (! is_electric_field ())
          {
            return STATUS_NO_ELECTRIC_FIELD;
          }
        return compute_electric_field (position_, time_, field_); 
      }
    else if (what_ == MAGNETIC_FIELD_LABEL)
      {
        if (! is_magnetic_field ())
          {
            return STATUS_NO_MAGNETIC_FIELD;
          }
        return compute_magnetic_field (position_, time_, field_); 
      }
    geomtools::invalidate (field_);
    return false;
  }
  

  void base_electromagnetic_field::initialize_standalone (const datatools::utils::properties & setup_)
  {
    datatools::service::service_manager dummy_srvcmgr;
    field_dict_type dummy_dict;
    initialize (setup_, dummy_srvcmgr, dummy_dict);
    return;
  }
   
  void base_electromagnetic_field::initialize_with_dictionary_only (const datatools::utils::properties & setup_,
                                                                    field_dict_type & dictionary_)
  {
    datatools::service::service_manager dummy_srvcmgr;
    initialize (setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void base_electromagnetic_field::initialize_with_service_only (const datatools::utils::properties & setup_,
                                                                 datatools::service::service_manager & service_manager_)
  {
    field_dict_type dummy_dict;
    initialize (setup_, service_manager_, dummy_dict);
    return;
  }

} // end of namespace emfield

// end of base_electromagnetic_field.cc
