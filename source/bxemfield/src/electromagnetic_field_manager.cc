// -*- mode: c++ ; -*- 
/* electromagnetic_field_manager.cc
 */ 

#include <emfield/electromagnetic_field_manager.h>

#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/utils.h>

#include <geomtools/utils.h>
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
//#include <geomtools/manager.h>

#include <emfield/geom_map.h>

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

  bool  electromagnetic_field_manager::has_geometry_manager() const
  {
    return _geom_manager_ != 0;
  }

  void electromagnetic_field_manager::set_geometry_manager(const geomtools::manager & geomgr_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::set_geometry_manager: "
                << "Cannot set the geometry manager ! EM field manager is locked !";
        throw std::logic_error (message.str ());        
      }
    _geom_manager_ = &geomgr_;
    return;
  }

  const geomtools::manager & electromagnetic_field_manager::get_geometry_manager() const
  {
    if (! has_geometry_manager())
      {
        throw std::logic_error("emfield::electromagnetic_field_manager::get_geometry_manager: No geometry manager is available !");
      }
    return *_geom_manager_;
  }


  bool electromagnetic_field_manager::has_geom_map () const
  {
    return _geom_map_.get() != 0; //is_initialized();
  }


  bool electromagnetic_field_manager::has_service_manager () const
  {
    return _service_manager_ != 0;
  }

  void electromagnetic_field_manager::set_service_manager (datatools::service_manager & sm_)
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
    datatools::fetch_path_with_env (rules_filename);
    _rules_.read (rules_filename);
    return;
  }

  void electromagnetic_field_manager::_construct_geomap_ (const datatools::properties & setup_)
  {
    // Extract geomap setup
    datatools::properties geom_map_setup;
    setup_.export_and_rename_starting_with(geom_map_setup, "geom_map.", "");
    _geom_map_.reset(new geom_map);
    _geom_map_.get()->set_debug(is_debug());
    _geom_map_.get()->set_fields_manager(const_cast<electromagnetic_field_manager&>(*this));
    if (has_geometry_manager())
      {
        if (is_debug())
          {
            std::clog << "DEBUG: " << "emfield::electromagnetic_field_manager::_construct_geomap_: "
                      << "Setup the geometry manager in the geometry/field map..."
                      << std::endl;
          }
        _geom_map_.get()->set_geometry_manager(*_geom_manager_);
      }
    if (is_debug())
      {
        geom_map_setup.tree_dump(std::clog, "Geometry map setup:", "DEBUG: ");
        std::clog << "DEBUG: " << "emfield::electromagnetic_field_manager::_construct_geomap_: "
                  << "Initialize geometry/field map..."
                  << std::endl;
      }
    _geom_map_.get()->initialize(geom_map_setup);

    return;
  } 
 
  void electromagnetic_field_manager::_construct_ ()
  {
    bool devel = false;
    if (getenv ("EMFIELD_MANAGER_DEVEL") != NULL)
      {
        devel = true;
      }
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = _rules_.ordered_entries ().begin ();
         i != _rules_.ordered_entries ().end ();
         i++)
      {
        const datatools::multi_properties::entry * ptr_entry = *i;
        const datatools::multi_properties::entry & e = *ptr_entry;
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
    _geom_manager_ = 0;
    _factory_preload_ =  true;
 
    bool preload = _factory_preload_;
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

  void electromagnetic_field_manager::initialize (const datatools::properties & setup_)
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

    /*
      if (setup_.has_flag ("factory_no_preload"))
      {
      _factory_preload_ = false;
      }
    */

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

    // Search for a handle to a geometry manager :
    if (setup_.has_key("services.geometry"))
      {
        std::string geo_service_name = setup_.fetch_string("services.geometry");
        
        if (! has_service_manager ())
          {
            throw std::logic_error("emfield::electromagnetic_field_manager::initialize: No service manager is available !");
          }
        
        if (_service_manager_->has (geo_service_name) 
            && _service_manager_->is_a<geomtools::geometry_service> (geo_service_name))
          {
            const geomtools::geometry_service & Geo 
              = _service_manager_->get<geomtools::geometry_service> (geo_service_name);
            const geomtools::manager & geomgr = Geo.get_geom_manager();
            _geom_manager_ = &geomgr;
            if (is_debug())
              {
                std::clog << "DEBUG: " << "emfield:electromagnetic_field_manager::initialize: "
                          << "Found geometry manager '" << _geom_manager_->get_setup_label() << "-"
                          << _geom_manager_->get_setup_version() << "'"
                          << std::endl;
              }
          }
        else
          {
            std::ostringstream message;
            message << "emfield::electromagnetic_field_manager::_construct_geomap_: "
                    << "Cannot find a geometry service named '" << geo_service_name << "' "
                    << "from the service manager !";
            throw std::logic_error(message.str());
          }
        
      }
 
    // Initialization :
    _construct_ ();

    if (setup_.has_flag("build_geom_map"))
      {
        _construct_geomap_ (setup_);
      }

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
    _set_initialized (false);
    _fields_.clear ();
    _rules_.reset ();
    _geom_manager_ = 0;     
    if (_geom_map_.get() != 0)
      {
        if (_geom_map_.get()->is_initialized())
          {
            _geom_map_.get()->reset();
          }
        _geom_map_.reset(0);
      }
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
    base_electromagnetic_field::field_dict_type::const_iterator found 
      = _fields_.find (field_name_);
    if (found == _fields_.end ())
      {
        std::ostringstream message;
        message << "emfield::electromagnetic_field_manager::grab_field: "
                << "Cannot find EM field named '" << field_name_ << "' !";
        throw std::logic_error (message.str ());        
      }
    return found->second.get ();
  }

  /*
  geom_map & electromagnetic_field_manager::grab_geom_map()
  {
    if (!has_geom_map())
      {
        throw std::logic_error("emfield::electromagnetic_field_manager::grab_geom_map: No geom map is available !");
      }
    return *_geom_map_.get();
  }
  */

  const geom_map & electromagnetic_field_manager::get_geom_map() const
  {
    if (! has_geom_map())
      {
        throw std::logic_error("emfield::electromagnetic_field_manager::get_geom_map: No geom map is available !");
      }
    return *_geom_map_.get();
  }

  void electromagnetic_field_manager::tree_dump (std::ostream & out_,
                                                 const std::string & title_,
                                                 const std::string & indent_,
                                                 bool inherit_) const
  {
    using namespace std;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized  : " <<  (_initialized_? "Yes": "No") << endl;

    if (! _initialized_) 
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Rules : ";
        if ( _rules_.entries ().size () == 0)
          {
            out_ << "<empty>";
          }
        out_ << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _rules_.tree_dump (out_, "", indent_oss.str ());
        }
      }
 
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Service manager : "  << has_service_manager()
           << std::endl;
    }
  
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Geometry manager : "  << has_geometry_manager()
           << std::endl;
    }
    
    {
      out_ << indent << datatools::i_tree_dumpable::tag
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
          out_ << indent << datatools::i_tree_dumpable::skip_tag;
          indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
          base_electromagnetic_field::field_dict_type::const_iterator j = i;
          j++;
          if (j == _fields_.end ())
            {
              out_ << datatools::i_tree_dumpable::last_tag;
              indent_oss << datatools::i_tree_dumpable::last_skip_tag;
            }
          else
            {
              out_ << datatools::i_tree_dumpable::tag;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
            }
          out_ << "EM field : " << '"' << field_name << '"' << endl;
          a_field.tree_dump (out_, "", indent_oss.str ());
        }
    }    
 
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Geometry field map : "
           << has_geom_map()
           << std::endl;
      if (has_geom_map())
        {
          std::ostringstream indent_oss;
          indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
          _geom_map_.get()->tree_dump(out_, "", indent_oss.str(), inherit_);
        }
    }
    return;
  }

} // end of namespace emfield

// end of electromagnetic_field_manager.cc
