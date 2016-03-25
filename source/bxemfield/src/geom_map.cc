// geom_map.cc

// Ourselves:
#include <emfield/geom_map.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <geomtools/manager.h>
#include <geomtools/logical_volume.h>

// This project:
#include <emfield/electromagnetic_field_manager.h>

namespace emfield {

  geom_map::association_entry::association_entry ()
  {
    logvol = 0;
    field = 0;
    return;
  }

  geom_map::geom_map ()
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _geom_manager_   = 0;
    _fields_manager_ = 0;
    _initialized_ = false;
    return;
  }

  geom_map::~geom_map ()
  {
    if (is_initialized ())
      {
        reset();
      }
    return;
  }

  const geom_map::association_dict_type & geom_map::get_associations () const
  {
    return _associations_map_;
  }

  geom_map::association_dict_type & geom_map::grab_associations ()
  {
    return _associations_map_;
  }

  void geom_map::set_logging_priority (datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  datatools::logger::priority geom_map::get_logging_priority () const
  {
    return _logging_priority_;
  }

  bool geom_map::is_initialized () const
  {

    return _initialized_;
  }

  void geom_map::initialize (const datatools::properties& config_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");

    // Logging priority:
    datatools::logger::priority lp = datatools::logger::extract_logging_configuration (config_);
    set_logging_priority(lp);

    DT_THROW_IF (! has_geometry_manager (), std::logic_error, "No geometry manager is available !");
    DT_THROW_IF (! has_fields_manager (), std::logic_error, "No EM fields manager is available !");

    _construct (config_);

    _initialized_ = true;
    return;
  }

  void geom_map::_construct (const datatools::properties& config_)
  {
    DT_LOG_DEBUG (get_logging_priority (), "Geometry volume/EM Field map: ");
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      config_.tree_dump (std::clog, "", "[debug]: ");
    }
    std::vector<std::string> association_labels;
    if (config_.has_key ("associations.labels")) {
      config_.fetch ("associations.labels", association_labels);
    }
    DT_LOG_DEBUG (get_logging_priority (), "Number of geometry volume/field associations : " << association_labels.size ());

    for (unsigned int i = 0; i < association_labels.size (); i++) {
        const std::string & association_label = association_labels[i];
        DT_LOG_DEBUG (get_logging_priority (), "Processing geometry volume/field association labelled '" << association_label << "'...");

        const std::string volume_name_key = "associations." + association_label + ".volume";
        const std::string field_name_key = "associations." + association_label + ".field_name";

        DT_THROW_IF (! config_.has_key (volume_name_key), std::logic_error, "Missing '" << volume_name_key << "' property !");
        DT_THROW_IF (! config_.has_key (field_name_key), std::logic_error, "Missing '" << field_name_key << "' property !");

        const std::string volume_name = config_.fetch_string (volume_name_key);
        geomtools::logical_volume::dict_type::const_iterator volume_found
          = _geom_manager_->get_factory ().get_logicals ().find(volume_name);
        DT_THROW_IF (volume_found == _geom_manager_->get_factory ().get_logicals ().end (),
                     std::logic_error,
                     "Cannot find logical volume named '" << volume_name << "' in the geometry manager !");
        const geomtools::logical_volume & logvol = *volume_found->second;

        const std::string field_name = config_.fetch_string (field_name_key);
        DT_THROW_IF (! _fields_manager_->has_field (field_name),
                     std::logic_error,
                     "Cannot find EM field named '" << field_name << "' from the EM fields manager !");
        const base_electromagnetic_field & field = _fields_manager_->get_field(field_name);
        {
          association_entry dummy;
          _associations_map_[association_label] = dummy;
        }
        association_entry & fe = _associations_map_[association_label];
        fe.label               = association_label;
        fe.logical_volume_name = volume_name;
        fe.logvol              = &logvol;
        fe.field_name          = field_name;
        fe.field               = &field;
        DT_LOG_NOTICE (get_logging_priority (),
                       "Add the EM association entry '" <<  association_label << "' with EM field '" << field_name << "' associated "
                       << "to the logical volume '" << volume_name << "'.");
      }
    if (_associations_map_.size() == 0) {
      DT_LOG_WARNING (datatools::logger::PRIO_ALWAYS, "No geometry volume/EM field association is defined! This may not be what you expect!");
    }
    return;
  }

  void geom_map::reset()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

    _associations_map_.clear();
    _geom_manager_   = 0;
    _fields_manager_ = 0;

    _initialized_ = false;
    return;
  }

  bool geom_map::has_fields_manager() const
  {
    return _fields_manager_ != 0;
  }

  void geom_map::set_fields_manager(const electromagnetic_field_manager & emfmgr_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");
    _fields_manager_ = &emfmgr_;
    return;
  }

  const electromagnetic_field_manager & geom_map::get_fields_manager() const
  {
    DT_THROW_IF (! has_fields_manager (), std::logic_error, "No EM fields manager is available !");
    return *_fields_manager_;
  }

  bool geom_map::has_geometry_manager() const
  {
    return _geom_manager_ != 0;
  }

  void geom_map::set_geometry_manager(const geomtools::manager & gmgr_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");
    _geom_manager_ = &gmgr_;
    return;
  }

  const geomtools::manager & geom_map::get_geometry_manager() const
  {
    DT_THROW_IF (! has_geometry_manager (), std::logic_error, "No geometry manager is available !");
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
         << "Initialized  : " <<  (is_initialized () ? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority : " << datatools::logger::get_priority_label (get_logging_priority ()) << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry manager  : " <<  (has_geometry_manager () ? "Yes": "No");
    if (has_geometry_manager ())
      {
        out_ << " [setup='" << _geom_manager_->get_setup_label() << "-" << _geom_manager_->get_setup_version() << "'";
        out_ << ']';
      }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "EM fields manager  : " <<  (has_fields_manager () ? "Yes": "No");
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
