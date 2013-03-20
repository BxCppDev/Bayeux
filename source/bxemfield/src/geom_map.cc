// -*- mode: c++ ; -*- 
/* geom_map.cc
 */ 

#include <stdexcept>
#include <sstream>

#include <emfield/geom_map.h>
#include <emfield/electromagnetic_field_manager.h>

#include <geomtools/manager.h>
#include <geomtools/logical_volume.h>

#include <datatools/properties.h>
#include <datatools/units.h>

namespace emfield {

  geom_map::association_entry::association_entry()
  {
    logvol = 0;
    field = 0;
    return;
  }
  
  geom_map::geom_map()
  {
    _debug_ = false;
    _initialized_ = false;
    _geom_manager_ = 0;
    _fields_manager_ = 0;
    return;
  }

  geom_map::~geom_map()
  {
    if (_initialized_)
      {
        reset();
      }
    return;
  }

  const geom_map::association_dict_type & geom_map::get_associations() const
  {
    return _associations_map_;    
  }

  geom_map::association_dict_type & geom_map::grab_associations()
  {
    return _associations_map_;
  }


  bool geom_map::is_initialized() const
  {

    return _initialized_;
  }

  void geom_map::initialize(const datatools::properties& config_)
  {
    if (is_initialized())
      {
        throw std::logic_error("emfield::geom_map::initialize: Already initialized !");
      }

    if (config_.has_flag("debug"))
      {
        _debug_ = true;
      }

    if (! has_geometry_manager())
      {
        throw std::logic_error("emfield::geom_map::initialize: No geometry manager is available !");
       }

    if (! has_fields_manager())
      {
        throw std::logic_error("emfield::geom_map::initialize: No EM fields manager is available !");
       }

    _construct(config_);


    _initialized_ = true;
    return;
  }

  void geom_map::_construct(const datatools::properties& config_)
  {

    if (is_debug())
      {
        config_.tree_dump(std::clog, "Geometry/EM Field map: ", "DEBUG: emfield::geom_map::_construct: ");
      }
    std::vector<std::string> association_labels;
    if (config_.has_key("associations.labels"))
      {
        config_.fetch("associations.labels", association_labels);
      }
   if (is_debug())
      {
        std::clog << "DEBUG: emfield::geom_map::_construct: "
                  << "Number of geometry/field associations : " 
                  << association_labels.size()
                  << std::endl;
      }
 
    for (int i = 0; i < association_labels.size(); i++)
      {
        const std::string & association_label = association_labels[i];
        if (is_debug())
          {
            std::clog << "DEBUG: " << "emfield::geom_map::_construct: "
                      << "Processing geometry/field association labelled '"
                      << association_label << "'..."
                      << std::endl;
          }
 
        const std::string model_name_key = "associations." + association_label + ".geometry_model";
        const std::string field_name_key = "associations." + association_label + ".field_name";

        if (! config_.has_key(model_name_key))
          {
            std::ostringstream message;
            message << "emfield::geom_map::_construct: "
                    << "Missing '" << model_name_key << "' property !";
            throw std::logic_error(message.str());
          }

        if (! config_.has_key(field_name_key))
          {
            std::ostringstream message;
            message << "emfield::geom_map::_construct: "
                    << "Missing '" << field_name_key << "' property !";
            throw std::logic_error(message.str());
          }
        const std::string model_name = config_.fetch_string(model_name_key);
        const std::string field_name = config_.fetch_string(field_name_key);

        geomtools::models_col_type::const_iterator model_found 
          = _geom_manager_->get_factory ().get_models ().find(model_name);
        if (model_found == _geom_manager_->get_factory ().get_models ().end ())
          {
            std::ostringstream message;
            message << "emfield::geom_map::_construct: "
                    << "Cannot find geometry model named '" << model_name << "' from the geometry manager !";
            throw std::logic_error(message.str());          
          }
        const geomtools::i_model * geom_model = model_found->second;
        const geomtools::logical_volume & logvol = geom_model->get_logical ();

        if (! _fields_manager_->has_field(field_name))
          {
            std::ostringstream message;
            message << "emfield::geom_map::_construct: "
                    << "Cannot find EM field named '" << field_name << "' from the EM fields manager !";
            throw std::logic_error(message.str());          
          }
        const base_electromagnetic_field & field = _fields_manager_->get_field(field_name);
        {
          association_entry dummy;
          _associations_map_[association_label] = dummy;
        }
        association_entry & fe = _associations_map_[association_label]; 
        fe.label = association_label;
        fe.geom_model_name = model_name;
        fe.logvol = &logvol;
        fe.field_name = field_name;
        fe.field = &field;
        std::clog << datatools::io::notice
                  << "emfield::geom_map::_construct: "
                  << "Add the EM association entry '" <<  association_label << "' with EM field '" << field_name << "' associated "
                  << "to the logical volume '" << logvol.get_name() << "'."
                  << std::endl;
      }

    return;
  }

  void geom_map::reset()
  {
    if (! is_initialized())
      {
        throw std::logic_error("emfield::geom_map::reset: Not initialized !");
      }

    _associations_map_.clear();
    _geom_manager_ = 0;
    _fields_manager_ = 0;

    _initialized_ = false;
    return;
  }

  bool geom_map::is_debug() const
  {
    return _debug_;
  }
    
  void geom_map::set_debug(bool d_)
  {
    _debug_ = d_;
    return;
  }

  bool geom_map::has_fields_manager() const
  {
    return _fields_manager_ != 0;
  }

  void geom_map::set_fields_manager(const electromagnetic_field_manager & emfmgr_)
  {
    if (is_initialized())
      {
        throw std::logic_error("emfield::geom_map::set_fields_manager: Already initialized !");
      }
    _fields_manager_ = &emfmgr_;
    return;
  }

  const electromagnetic_field_manager & geom_map::get_fields_manager() const
  {
    if (! has_fields_manager())
      {
        throw std::logic_error("emfield::geom_map::get_fields_manager: No EM fields manager is available !");
      }
    return *_fields_manager_;
  }

  bool geom_map::has_geometry_manager() const
  {
    return _geom_manager_ != 0;
  }

  void geom_map::set_geometry_manager(const geomtools::manager & gmgr_)
  {
    if (is_initialized())
      {
        throw std::logic_error("emfield::geom_map::set_geometry_manager: Already initialized !");
      }
    _geom_manager_ = &gmgr_;
    return;
  }

  const geomtools::manager & geom_map::get_geometry_manager() const
  {
    if (! has_geometry_manager())
      {
        throw std::logic_error("emfield::geom_map::get_geometry_manager: No geometry manager is available !");
      }
    return *_geom_manager_;
  }

  void geom_map::tree_dump (std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_, 
                            bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << std::endl;
      }
    
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized  : " <<  (_initialized_? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Debug  : " <<  (_debug_? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry manager  : " <<  (has_geometry_manager() ? "Yes": "No");
    if (has_geometry_manager())
      {
        out_ << " [setup='" << _geom_manager_->get_setup_label() << "-" << _geom_manager_->get_setup_version() << "'";
        out_ << ']';
      }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "EM fields manager  : " <<  (has_fields_manager() ? "Yes": "No");
    out_ << std::endl;
 
    // Fields' geometry associations : 
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Associations : ";
      if ( _associations_map_.size () == 0)
        {
          out_ << "<empty>";
        }
      else
        {
          out_ << "[" << _associations_map_.size () << "]";
        }
      out_ << std::endl;
      for (association_dict_type::const_iterator i = _associations_map_.begin ();
           i != _associations_map_.end ();
           i++)
        {
          const std::string & key = i->first;
          const association_entry & fe = i->second;
          std::ostringstream indent_oss;
          out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          association_dict_type::const_iterator j = i;
          j++;
          if (j == _associations_map_.end ())
            {
              out_ << datatools::i_tree_dumpable::inherit_tag (inherit_);
              //indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
            }
          else
            {
              out_ << datatools::i_tree_dumpable::tag;
              //indent_oss << datatools::i_tree_dumpable::skip_tag;
            }
          out_ //<< indent_oss.str() 
               << "Association : '" << key << "' : "
               << "Logical volume='" << fe.logvol->get_name () 
               << "' is associated to EM field='" << fe.field_name << "'"
               << std::endl;
        }
    }
   
    return;
  }
  
} // end of namespace emfield

// end of geom_map.cc
