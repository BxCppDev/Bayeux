// ocd_utils.cc

// Ourselves:
#include <datatools/detail/ocd_utils.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/exception.h>

namespace datatools {
namespace detail {
namespace ocd {

  ocd_registration::entry_type::entry_type()
  {
    link = 0;
  }

  ocd_registration::entry_type::~entry_type()
  {
    if (handle) {
      handle.reset();
    }
  }

  bool ocd_registration::entry_type::has_ocd() const
  {
    if (link) {
      return true;
    } else if (handle) {
      return true;
    }
    return false;
  }

  const object_configuration_description & ocd_registration::entry_type::get() const
  {
    if (link) {
      return *link;
    } else if (handle) {
      return *handle;
    } else {
      DT_THROW_IF(true, std::logic_error, "No handle to any OCD data!");
    }
  }

  bool ocd_registration::is_system() const
  {
    return this == &get_system_registration();
  }

  ocd_registration::ocd_registration(bool preload_system_registration_)
  {
    _logging_ = ::datatools::logger::PRIO_FATAL;
    char * docd_logging = getenv("DATATOOLS_OCD_DEVEL_LOGGING");
    if (docd_logging) {
      std::string ocd_logging_label = docd_logging;
      _logging_ = ::datatools::logger::get_priority(ocd_logging_label);
      if (_logging_ == ::datatools::logger::PRIO_UNDEFINED) {
        _logging_ = ::datatools::logger::PRIO_FATAL;
      }
    }
    if (preload_system_registration_) {
      _preload_system_registration();
    }
    return;
  }

  ocd_registration::~ocd_registration()
  {
    _dict_.clear();
    return;
  }

  void ocd_registration::_preload_system_registration()
  {
    // 2014-07-03, FM: do not process for it is rather dangerous without care:
    // The line below is invalid now: we should here populate the _dict_
    // with some link and not a hard copy of the system's _dict_.
    // _dict_ = get_system_registration()._dict_;
    return;
  }

  bool ocd_registration::has_id(const std::string & class_id_) const
  {
    return _dict_.find(class_id_) != _dict_.end();
  }

  const object_configuration_description &
  ocd_registration::get(const std::string & class_id_) const
  {
    ocd_dict_type::const_iterator found = _dict_.find(class_id_);
    DT_THROW_IF (found == _dict_.end(),
                 std::logic_error,
                 "No OCD for class with ID '" << class_id_ << "' !");
    const entry_type & eocd = found->second;
    if (eocd.link) {
      return *eocd.link;
    }
    return *eocd.handle;
  }

  void ocd_registration::compute_ids(std::vector<std::string> & list_) const
  {
    for (ocd_dict_type::const_iterator i = _dict_.begin();
         i != _dict_.end();
         i++) {
      list_.push_back(i->first);
    }
    return;
  }

  void ocd_registration::smart_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << "`-- "
         << "Classes with OCD support : " << '[' << _dict_.size() << ']' << std::endl;
    for (ocd_dict_type::const_iterator i = _dict_.begin();
         i != _dict_.end();
         i++) {
      ocd_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << "    ";
      if (j == _dict_.end()) out_ << "`-- ";
      else out_ << "|-- ";
      out_ << "Class '" << i->first << "'";
      out_ << std::endl;
    }

    return;
  }


  // static
  ocd_registration & ocd_registration::grab_system_registration()
  {
    static boost::scoped_ptr<ocd_registration> _ocd_registration;
    if (!_ocd_registration) {
      _ocd_registration.reset(new ocd_registration(false));
    }
    return *_ocd_registration.get();
  }


  // static
  const ocd_registration & ocd_registration::get_system_registration()
  {
    return const_cast<ocd_registration &>(grab_system_registration());
  }

} // namespace ocd
} // namespace detail
} // namespace datatools
