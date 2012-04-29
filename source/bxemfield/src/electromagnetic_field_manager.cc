// -*- mode: c++ ; -*- 
/* electromagnetic_field_manager.cc
 */ 

#include <emfield/electromagnetic_field_manager.h>

#include <datatools/utils/properties.h>
#include <datatools/services/service_manager.h>
#include <datatools/utils/utils.h>
#include <geomtools/utils.h>

namespace emfield {

  bool electromagnetic_field_manager::is_initialized () const
  {
    return _initialized_;
  }


  void electromagnetic_field_manager::_set_initialized (bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }


  bool electromagnetic_field_manager::is_debug () const
  {
    return _debug_;
  }


  void electromagnetic_field_manager::set_debug (bool debug_)
  {
    _debug_ = debug_;
    return;
  }

  bool electromagnetic_field_manager::has_service_manager () const
  {
    return _service_manager_ != 0;
  }

  void electromagnetic_field_manager::set_service_manager (datatools::service::service_manager & sm_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::set_service_manager: "
                << "Cannot change the service manager ! EM field manager is locked !";
        throw std::logic_error (message.str ());        
      }
    _service_manager_ = &sm_;
    return;
  }

  void electromagnetic_field_manager::reset_service_manager ()
  {
    if (is_initialized ())
      {
       std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::reset_service_manager: "
                << "Cannot reset the service manager ! EM field manager is locked !";
        throw std::logic_error (message.str ());        
      }
    _service_manager_ = 0;
    return;
  }

  void electromagnetic_field_manager::load (const std::string & rules_filename_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::load: "
                << "Cannot load a rules file ! EM field manager is locked !";
        throw std::logic_error (message.str ());        
      }
    std::string rules_filename = rules_filename_;
    datatools::utils::fetch_path_with_env (rules_filename);
    _rules_.read (rules_filename);
    return;
  }

  void electromagnetic_field_manager::_construct_ ()
  {
    bool devel = false;
    if (getenv ("EMFIELD_MANAGER_DEVEL") != NULL)
      {
        devel = true;
      }
    for (datatools::utils::multi_properties::entries_ordered_col_t::const_iterator i
           = _rules_.ordered_entries ().begin ();
         i != _rules_.ordered_entries ().end ();
         i++)
      {
        const datatools::utils::multi_properties::entry * ptr_entry = *i;
        const datatools::utils::multi_properties::entry & e = *ptr_entry;
        std::string field_name = e.get_key ();
        if (_fields_.find (field_name) != _fields_.end ())
          {
            std::ostringstream message;
            message << "emfield::electromagnetic_field_manager::_construct_: " 
                    << "A field named '" << field_name << "' already exists !";
            throw std::logic_error (message.str ());
          }

        std::string field_type = e.get_meta ();
        if (! _factory_register_.has (field_type))
          {
            std::ostringstream message;
            message << "emfield::electromagnetic_field_manager::_construct_: " 
                    << "No registered EM field class with ID '" 
                    << field_type << "' for field named '" << field_name << " !";
            throw std::logic_error (message.str ());
          }
        const base_electromagnetic_field::factory_register_type::factory_type & the_factory 
          = _factory_register_.get (field_type);
        if (devel)
          {
            std::clog << "DEVEL: emfield::electromagnetic_field_manager::_construct_: "
                 << "About to create a new field of type \"" << field_type
                 << "\" with name \"" << field_name << "\"..." << std::endl;
          }
        base_electromagnetic_field * ptr_field = the_factory ();
        if (has_service_manager ())
          {
            ptr_field->initialize (e.get_properties (), *_service_manager_, _fields_);
          }
        else
          {
            ptr_field->initialize_with_dictionary_only (e.get_properties (), _fields_);
         }
        base_electromagnetic_field::handle_type new_handle (ptr_field);
        _fields_[field_name] = new_handle;
        
      }
    _rules_.reset ();
    return;
  }

  electromagnetic_field_manager::electromagnetic_field_manager (uint32_t flags_)
    : _factory_register_ ("emfield::base_electromagnetic_field/manager", 
                          flags_ & base_electromagnetic_field::DEBUG ? 
                          base_electromagnetic_field::factory_register_type::verbose : 0)

  {
    _initialized_ = false;
    _debug_ = flags_ & base_electromagnetic_field::DEBUG;
    _service_manager_ = 0;
    bool preload = true;
    if (preload)
      {
        _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::emfield::base_electromagnetic_field));
     }
    return;
  }
  
  electromagnetic_field_manager::~electromagnetic_field_manager ()
  {
    if (is_initialized ())
      {
        reset ();
      }
    return;
  }

  void electromagnetic_field_manager::initialize (const datatools::utils::properties & setup_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::initialize: "
                << "Cannot reset the service manager ! EM field manager is already initialized !";
        throw std::logic_error (message.str ());        
      }

    bool needs_service_manager = false;

    // Parse configuration parameters :

    if (setup_.has_flag ("debug"))
      {
        set_debug (true);
      }

    if (setup_.has_flag ("needs_service_manager"))
      {
        needs_service_manager = true;
      }

    if (setup_.has_key ("field_rules_filenames"))
      {
        std::vector<std::string> field_rules_filenames;
        setup_.fetch ("field_rules_filenames", field_rules_filenames);
        for (int i = 0; i < field_rules_filenames.size (); i++)
          {
            load (field_rules_filenames[i]);
          }
      }

    // Checks :

    if (needs_service_manager)
      {
        if (! has_service_manager ())
          {
            std::ostringstream message;
            message << "emfield::electromagnetic_field_manager::initialize: "
                    << "Cannot find mandatory service manager !";
            throw std::logic_error (message.str ());        
          }
      }

    // Initialization :
  
    _construct_ ();

    _set_initialized (true);
    return;
  }

  void electromagnetic_field_manager::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::reset: "
                << "Cannot reset the service manager ! EM field manager is not initialized !";
        throw std::logic_error (message.str ());        
      }
    _fields_.clear ();
    _rules_.reset ();
    _set_initialized (false);
    reset_service_manager ();
    return;
  }

  const base_electromagnetic_field::field_dict_type & 
  electromagnetic_field_manager::get_fields () const
  {
    return _fields_;
  }
  
  bool electromagnetic_field_manager::has_field (const std::string & field_name_) const
  {
    return _fields_.find (field_name_) != _fields_.end ();
  }

  base_electromagnetic_field & 
  electromagnetic_field_manager::grab_field (const std::string & field_name_)
  {
    base_electromagnetic_field::field_dict_type::iterator found 
      = _fields_.find (field_name_);
    if (found == _fields_.end ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::grab_field: "
                << "Cannot find EM field named '" << field_name_ << "' !";
        throw std::logic_error (message.str ());        
      }
    return found->second.grab ();
  }

  const base_electromagnetic_field & 
  electromagnetic_field_manager::get_field (const std::string & field_name_) const
  {
    electromagnetic_field_manager * mutable_this 
      = const_cast<electromagnetic_field_manager *> (this);
    return mutable_this->get_field (field_name_);
  }

  void electromagnetic_field_manager::tree_dump (std::ostream & out_,
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

    if (! _initialized_) 
      {
        out_ << indent << du::i_tree_dumpable::tag
             << "Rules : ";
        if ( _rules_.entries ().size () == 0)
          {
            out_ << "<empty>";
          }
        out_ << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          _rules_.tree_dump (out_, "", indent_oss.str ());
        }
      }
    
    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
           << "EM fields : ";
      if ( _fields_.size () == 0)
        {
          out_ << "<empty>";
        }
      else
        {
          out_ << "[" << _fields_.size () << "]";
        }
      out_ << endl;
      for (base_electromagnetic_field::field_dict_type::const_iterator i 
             = _fields_.begin ();
           i != _fields_.end ();
           i++)
        {
          const std::string & field_name = i->first;
          const base_electromagnetic_field & a_field = i->second.get ();
          ostringstream indent_oss;
          out_ << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
          indent_oss << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
          base_electromagnetic_field::field_dict_type::const_iterator j = i;
          j++;
          if (j == _fields_.end ())
            {
              out_ << du::i_tree_dumpable::inherit_tag (inherit_);
              indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
            }
          else
            {
              out_ << du::i_tree_dumpable::tag;
              indent_oss << du::i_tree_dumpable::skip_tag;
            }
          out_ << "EM field : " << '"' << field_name << '"' << endl;
          a_field.tree_dump (out_, "", indent_oss.str ());
        }
    }    
    return;
  }

} // end of namespace emfield

// end of electromagnetic_field_manager.cc
