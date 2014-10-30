// Ourselves:
#include <datatools/enriched_base.h>

// This project:
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/logger.h>

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(enriched_base,
                                                  "datatools::enriched_base")

  bool enriched_base::validate_name(const std::string & candidate_name_)
  {
    static const std::string allowed_chars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
    if (candidate_name_.empty()) return false;
    if (candidate_name_.find_first_not_of(allowed_chars)
        != candidate_name_.npos) return false;
    if (candidate_name_.find_first_of("0123456789.") == 0) return false;
    if (candidate_name_[candidate_name_.size()-1] == '.') return false;
    return true;
  }

  enriched_base::enriched_base()
  {
    _logging_priority_ = logger::PRIO_FATAL;
    return;
  }

  enriched_base::enriched_base(const std::string & name_,
                               const std::string & display_name_,
                               const std::string & terse_description_,
                               logger::priority lp_)
  {
    _logging_priority_ = lp_;
    set_name(name_);
    set_display_name(display_name_);
    set_terse_description(terse_description_);
    return;
  }

  enriched_base::~enriched_base()
  {
    return;
  }

  bool enriched_base::has_name() const
  {
    return ! _name_.empty();
  }

  void enriched_base::set_name(const std::string & name_)
  {
    set_name_c(name_);
  }

  enriched_base & enriched_base::set_name_c(const std::string & name_)
  {
    DT_THROW_IF(! validate_name(name_),
                std::logic_error,
                "Invalid name '" << name_ << "' !");
    if (_name_ != name_) {
      DT_LOG_TRACE(_logging_priority_, "Rename object from '" << _name_ << "' to '" << name_ << "'.");
    }
    _name_ = name_;
    return *this;
  }

  const std::string & enriched_base::get_name() const
  {
    return _name_;
  }

  bool enriched_base::has_display_name() const
  {
    return ! _display_name_.empty();
  }

  void enriched_base::set_display_name(const std::string & display_name_)
  {
    set_display_name_c(display_name_);
    return;
  }

  enriched_base & enriched_base::set_display_name_c(const std::string & display_name_)
  {
    _display_name_ = display_name_;
    return *this;
  }

  const std::string & enriched_base::get_display_name() const
  {
    return _display_name_;
  }

  bool enriched_base::has_terse_description() const
  {
    return ! _terse_description_.empty();
  }

  void enriched_base::set_terse_description(const std::string & terse_description_)
  {
    set_terse_description_c(terse_description_);
    return;
  }

  enriched_base & enriched_base::set_terse_description_c(const std::string & terse_description_)
  {
    _terse_description_ = terse_description_;
    return *this;
  }

  const std::string & enriched_base::get_terse_description() const
  {
    return _terse_description_;
  }

  bool enriched_base::has_auxiliaries() const
  {
    return _auxiliaries_.size() > 0;
  }

  const datatools::properties & enriched_base::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & enriched_base::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  void enriched_base::reset_auxiliaries()
  {
    _auxiliaries_.reset();
    return;
  }

  void enriched_base::set_logging_priority(logger::priority lp_)
  {
    set_logging_priority_c(lp_);
    return;
  }

  enriched_base & enriched_base::set_logging_priority_c(logger::priority lp_)
  {
    _logging_priority_ = lp_;
    return *this;
  }

  bool enriched_base::is_debug() const
  {
    return _logging_priority_ >= logger::PRIO_DEBUG;
  }

  logger::priority enriched_base::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void enriched_base::clear()
  {
    _name_.clear();
    _display_name_.clear();
    _terse_description_.clear();
    reset_auxiliaries();
    return;
  }

  void enriched_base::tree_dump(std::ostream& out_,
                                const std::string& title_,
                                const std::string& indent_,
                                bool inherit_) const {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Name              : ";
    if (! _name_.empty()) {
      out_ << "'" << _name_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Display name      : ";
    if (! _display_name_.empty()) {
      out_ << "'" << _display_name_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Terse description : ";
    if (! _terse_description_.empty()) {
      out_ << "'" << _terse_description_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Logging priority  : '"
         << logger::get_priority_label(_logging_priority_) << "'" << std::endl;

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Auxiliaries       : ";
    if (! has_auxiliaries()) {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (has_auxiliaries()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::inherit_skip_tag(inherit_);
      get_auxiliaries().tree_dump(out_, "", indent2.str());
    }

    return;
  }

  void enriched_base::initialize(const datatools::properties & config_, bool clear_)
  {
    if (clear_) this->clear();

    // Parse logging priority:
    logger::priority prio = logger::extract_logging_configuration(config_, logger::PRIO_FATAL, true);
    if (prio != logger::PRIO_UNDEFINED) {
      set_logging_priority(prio);
    }

    // Parse name:
    if (!has_name()) {
      if (config_.has_key("name")) {
        set_name(config_.fetch_string("name"));
      }
    }

    // Parse display name:
    if (!has_display_name()) {
      if (config_.has_key("display_name")) {
        set_display_name(config_.fetch_string("display_name"));
      }
    }

    // Parse terse description:
    if (!has_terse_description()) {
      if (config_.has_key("terse_description")) {
        set_terse_description(config_.fetch_string("terse_description"));
      }
    }

    // Import auxiliary properties:
    config_.export_and_rename_starting_with(grab_auxiliaries(), "aux.", "");

    return;
  }

  // static
  void enriched_base::init_ocd(datatools::object_configuration_description & ocd_)
  {
    logger::declare_ocd_logging_configuration(ocd_, "fatal");

    {
      configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("name")
        .set_from("datatools::enriched_base")
        .set_terse_description("Set the name of the object")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set a dummy name::             \n"
                     "                               \n"
                     "  name : string = \"foo_id\"   \n"
                     "                               \n"
                     )
        ;
    }

    {
      configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("display_name")
        .set_from("datatools::enriched_base")
        .set_terse_description("Set the display name of the object")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set a dummy display name::                \n"
                     "                                          \n"
                     "  display name : string = \"Foo's ID\"    \n"
                     "                                          \n"
                     )
        ;
    }

    {
      configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("description")
        .set_from("datatools::enriched_base")
        .set_terse_description("Set the description of the object")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set a terse description string::            \n"
                     "                                            \n"
                     "  description : string = \"A dummy object\" \n"
                     "                                            \n"
                     )
        ;
    }

    return;
  }

} // end of namespace datatools
