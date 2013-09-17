// Ourselves:
#include <datatools/advanced_object.h>

// This project:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(advanced_object,
                                                  "datatools::advanced_object")

  bool advanced_object::validate_name(const std::string & candidate_name_)
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

  advanced_object::advanced_object()
  {
    _logging_priority_ = logger::PRIO_WARNING;
    return;
  }

  advanced_object::advanced_object(const std::string & name_,
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

  advanced_object::~advanced_object()
  {
    return;
  }

  bool advanced_object::has_name() const
  {
    return ! _name_.empty();
  }

  void advanced_object::set_name(const std::string & name_)
  {
    set_name_c(name_);
  }

  advanced_object & advanced_object::set_name_c(const std::string & name_)
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

  const std::string & advanced_object::get_name() const
  {
    return _name_;
  }

  bool advanced_object::has_display_name() const
  {
    return ! _display_name_.empty();
  }

  void advanced_object::set_display_name(const std::string & display_name_)
  {
    set_display_name_c(display_name_);
    return;
  }

  advanced_object & advanced_object::set_display_name_c(const std::string & display_name_)
  {
    _display_name_ = display_name_;
    return *this;
  }

  const std::string & advanced_object::get_display_name() const
  {
    return _display_name_;
  }

  bool advanced_object::has_terse_description() const
  {
    return ! _terse_description_.empty();
  }

  void advanced_object::set_terse_description(const std::string & terse_description_)
  {
    set_terse_description_c(terse_description_);
    return;
  }

  advanced_object & advanced_object::set_terse_description_c(const std::string & terse_description_)
  {
    _terse_description_ = terse_description_;
    return *this;
  }

  const std::string & advanced_object::get_terse_description() const
  {
    return _terse_description_;
  }

  bool advanced_object::has_auxiliaries() const
  {
    return _auxiliaries_.size() > 0;
  }

  const datatools::properties & advanced_object::get_auxiliaries() const
  {
   return _auxiliaries_;
  }

  datatools::properties & advanced_object::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  void advanced_object::reset_auxiliaries()
  {
    _auxiliaries_.reset();
    return;
  }

  void advanced_object::set_logging_priority(logger::priority lp_)
  {
    set_logging_priority_c(lp_);
    return;
  }

  advanced_object & advanced_object::set_logging_priority_c(logger::priority lp_)
  {
    _logging_priority_ = lp_;
    return *this;
  }

  bool advanced_object::is_debug() const
  {
    return _logging_priority_ >= logger::PRIO_DEBUG;
  }

  logger::priority advanced_object::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void advanced_object::clear_advanced()
  {
    _name_.clear();
    _display_name_.clear();
    _terse_description_.clear();
    reset_auxiliaries();
    return;
  }

  void advanced_object::tree_dump(std::ostream& out_,
                                  const std::string& title_,
                                  const std::string& indent_,
                                  bool inherit_) const {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Name : ";
    if (! _name_.empty()) {
      out_ << "'" << _name_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Display name : ";
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
         << "Auxiliaries : ";
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

} // end of namespace datatools
