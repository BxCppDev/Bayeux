// -*- mode: c++; -*- 
// ocd_utils.cc

#include <datatools/detail/ocd_utils.h>

#include <boost/scoped_ptr.hpp>

namespace datatools {
namespace detail {
namespace ocd {

  ocd_registration::ocd_registration(bool preload_system_registration_)
  {
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
    _dict_ = get_system_registration()._dict_;
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
    if (found == _dict_.end()) {
      std::ostringstream message;
      message << "datatools::detail::ocd::ocd_registration::get: "
              << "No OCD for class with ID '" << class_id_ << "' !";
      throw std::logic_error(message.str());
    }
    return *found->second.handle.get();
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
    static boost::scoped_ptr<ocd_registration> _ocd_registration(0);
    if (_ocd_registration.get() == 0) {
      _ocd_registration.reset(new ocd_registration);
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

// end of ocd_utils.cc
