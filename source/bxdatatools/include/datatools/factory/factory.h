/* datatools/factory/factory.h */
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-19
 * Last modified : 2012-04-30
 *
 */
#ifndef DATATOOLS_FACTORY_FACTORY_H_
#define DATATOOLS_FACTORY_FACTORY_H_

// Standard Library
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

// - Third Party
// Boost
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/utils/i_tree_dump.h>

namespace datatools {
namespace factory {

class base_factory_register {
 public:
  enum flag_type {
    verbose = 0x1,
  };

 public:    
  base_factory_register();
  virtual ~base_factory_register ();
};
    

template <class BaseType>
class factory_register 
    : public base_factory_register,
      public datatools::utils::i_tree_dumpable {
 public:
  typedef BaseType                            base_type;
  typedef boost::function<base_type*() >      factory_type;
  typedef std::map<std::string, factory_type> factory_map_type;

 public:
  /// Constructor
  factory_register() {
    verbose_ = false;
  }


  /// Constructor
  factory_register(const std::string& label, unsigned int flags = 0x0) {
    verbose_ = false;
    if (flags & verbose) {
      verbose_ = true;
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label << ">: Construction..." << std::endl;
    }
    label_ = label;
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">: Construction done." << std::endl;
    }
  }


  /// Destructor
  virtual ~factory_register() {
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">: Destruction..." << std::endl;
    }

    this->clear();
    
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">: Destruction done." << std::endl;
    }
  }


  bool is_verbose() const {
    return verbose_;
  }


  void set_verbose(bool verbose) {
    verbose_ = verbose;
  }


  const std::string& get_label() const {
    return label_;
  }


  // ^ err, why do you need two methods to do the SAME thing?
  const std::string& label() const {
    return label_;
  }


  void set_label(const std::string& label) {
    label_ = label;
  }


  void list_of_factories(std::vector<std::string>& ids) const {
    for (typename factory_map_type::iterator i = registered_.begin();
         i != registered_.end();
         ++i) {
      ids.push_back(i->first);
    }
  }


  /// Check if some factory with some given ID is registered
  bool has(const std::string& id) const {
    return registered_.find(id) != registered_.end();
  }


  /// Clear all registered factories
  void clear() {
    for (typename factory_map_type::iterator i = registered_.begin();
         i != registered_.end();
         ++i) {
      if (i->second != 0) {
        if (this->is_verbose()) {
          std::clog << "INFO: "
                    << "datatools::factory::factory_register::clear: "
                    << "Destroying registered allocator/functor '"
                    << i->first 
                    << "'" << std::endl;
        }
      }
    }
    registered_.clear();
  }


  /// Reset
  void reset() {
    this->clear();
    label_.clear();
    verbose_ = false;
  }


  /// Get a mutable reference to a factory given by registration ID
  // TODO : STL practice for const vs non-const is same method name,
  //        different signature.
  factory_type& grab(const std::string& id) {
    typename factory_map_type::iterator found = registered_.find(id);
    if (found == registered_.end()) {
      std::ostringstream message;
      message << "datatools::factory::factory_register::grab: "
              << "Class ID '" << id << "' is not registered !";
      throw std::logic_error(message.str());
    }
    return found->second;
  }


  /// Get a const reference to a factory given by registration ID
  const factory_type& get(const std::string& id) const {
    typename factory_map_type::const_iterator found = registered_.find(id);
    if (found == registered_.end()) {
      std::ostringstream message;
      message << "datatools::factory::factory_register::get: "
              << "Class ID '" << id << "' is not registered !";
      throw std::logic_error(message.str());
    }
    return found->second;
  }


  /// Registration of a given factory with a given ID
  void registration(const std::string& id, const factory_type& factory) {
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">::registration: "
                << "registration of class with ID '" << id << "'" 
                << std::endl;
    }

    typename factory_map_type::const_iterator found = registered_.find(id);
    if (found != registered_.end()) {
      std::ostringstream message;
      message << "datatools::factory::factory_register<" << label_ << ">::registration: "
              << "Class ID '" << id << "' is already registered !";
      throw std::logic_error(message.str());
    }
    registered_[id] = factory;
  }


  /// Unregistration of the factory with a given ID
  /// TODO : better naming "unregister" for example
  void unregistration(const std::string& id) {
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">::unregistration:  "
                << "unregistration of class with ID '" << id << "'" << std::endl;
    }
    typename factory_map_type::const_iterator found = registered_.find(id);
    if (found == registered_.end()) {
      std::ostringstream message;
      message << "datatools::factory::factory_register<" << label_ << ">::unregistration: "
              << "Class ID '" << id << "' is not registered !";
      throw std::logic_error(message.str());
    }
    registered_.erase(id);
  }


  /// Import registered factories from another factory register
  /// So, err, copy assignment in other words...
  void import(const factory_register& factory_register) {
    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">::import: Importing registered factories from register '"
                << factory_register.label() << "'..." << std::endl;
    }

    for (typename factory_map_type::const_iterator i = factory_register.registered_.begin();
         i !=  factory_register.registered_.end();
         ++i) {
      const factory_type& the_out_factory = i->second;
      this->registration(i->first, the_out_factory);
    }

    if (this->is_verbose()) {
      std::clog << "INFO: " << "datatools::factory::factory_register<"
                << label_ << ">::import: Done." << std::endl;
    }
  }


  /// Simple print
  void print(std::ostream& out, const std::string& indent = "") const {
    std::ostringstream title_oss;
    title_oss << "List of registered allocators/functors for label \""
              << label_ << "\" : ";
    this->tree_dump(out, title_oss.str(), indent);
  }


  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, 
                         const std::string& title = "", 
                         const std::string& indent_ = "",
                         bool inherit = false) const {
    namespace du = datatools::utils;
    std::string indent;
    if (!indent_.empty()) indent = indent_;

    if (!title.empty()) { out << indent << title << std::endl; }

    out << indent << du::i_tree_dumpable::tag
        << "Label   : '" << label_ << "'" << std::endl;
    out << indent << du::i_tree_dumpable::tag 
        << "Verbose : " << verbose_ << std::endl;
    out << indent << du::i_tree_dumpable::inherit_tag(inherit) 
        << "Registered factories : " << std::endl;

    for (typename factory_map_type::const_iterator i = registered_.begin();
         i != registered_.end ();
         ++i) {
      out << indent_;
      out << du::i_tree_dumpable::inherit_skip_tag(inherit);
      typename factory_map_type::const_iterator j = i;
      j++;
      if (j == registered_.end()) {
        out << du::i_tree_dumpable::last_tag;
      } else {
        out << du::i_tree_dumpable::tag;
      }
      
      out << "ID: \"" << i->first << "\" @ " << &i->second << std::endl;
    }
  }


 private:
  std::string      label_;      /// Label of the factory
  bool             verbose_;    /// Verbosity flag
  factory_map_type registered_; /// Dictionary of registered factories
};

} // end of namespace factory
} // end of namespace datatools

#include <boost/type_traits/is_base_of.hpp>

namespace datatools {
namespace factory {

/* Utility to enable auto-(un)registration of the Type class in the
 * the system factory register:
 */
template <class BaseType, class DerivedType>
class _system_factory_registrator {
 public:
  _system_factory_registrator(const std::string& type_id) {
    type_id_ = type_id;
    this->_trigger_factory_registration_();
  }

  ~_system_factory_registrator() {
    this->_trigger_factory_unregistration_();
  }

 private:
  void _trigger_factory_registration_() {
    if (!boost::is_base_of<BaseType, DerivedType>::value) {
      std::ostringstream message;
      message << "datatools::factory::_system_factory_registrator::_trigger_factory_registration_: "
              << "Class ID '" << type_id_ << "' cannot be registered in register '"
              << BaseType::grab_system_factory_register().label() << "' !";

      throw std::logic_error(message.str());
    }

    BaseType::grab_system_factory_register().registration(
        type_id_,
        boost::factory<DerivedType*>());
  }

  void _trigger_factory_unregistration_() {
    if (BaseType::grab_system_factory_register().has(type_id_)) {
      BaseType::grab_system_factory_register().unregistration(type_id_);
    }
  }

 private:
  std::string type_id_;
};

} // end of namespace factory

} // end of namespace datatools

#endif // DATATOOLS_FACTORY_FACTORY_H_

