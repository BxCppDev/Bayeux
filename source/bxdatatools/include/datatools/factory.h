/* datatools/factory.h */
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-19
 * Last modified : 2013-04-22
 *
 */
#ifndef DATATOOLS_FACTORY_H_
#define DATATOOLS_FACTORY_H_

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
#include <datatools/i_tree_dump.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {
/*! \brief The base class for all specialized template factory registration classes
 */
class base_factory_register {
 public:
  enum flag_type {
    verbose = 0x1,
  };

 public:
  base_factory_register();
  virtual ~base_factory_register ();
};


/*! \brief Template factory registration class
 */
template <class BaseType>
class factory_register
    : public base_factory_register,
      public datatools::i_tree_dumpable {
 public:
  typedef BaseType                            base_type;
  typedef boost::function<base_type*() >      factory_type;
  typedef std::map<std::string, factory_type> factory_map_type;

 public:
  /// Constructor
  factory_register();

  /// Constructor
  factory_register(const std::string& label, unsigned int flags = 0x0);

  /// Destructor
  virtual ~factory_register();

  //! Check if verbosity flag is activated
  bool is_verbose() const;

  //! Set verbosity flag
  void set_verbose(bool verbose);

  //! Get the label associated to the factory
  const std::string& get_label() const;

  // ^ err, why do you need two methods to do the SAME thing?
  const std::string& label() const;

  //! Set the label associated to the factory
  void set_label(const std::string& label);

  /// Copy factory IDs into supplied vector
  void list_of_factories(std::vector<std::string>& ids) const;

  /// Return true if factory with given ID is registered
  bool has(const std::string& id) const;

  /// Clear all registered factories
  void clear();

  /// Reset
  void reset();

  /// Return a mutable reference to a factory given by registration ID
  // TODO : STL practice for const vs non-const is same method name,
  //        different signature.
  factory_type& grab(const std::string& id);

  /// Return a const reference to a factory given by registration ID
  const factory_type& get(const std::string& id) const;

  /// Register the supplied factory under the given ID
  void registration(const std::string& id, const factory_type& factory);

  /// Remove registration of the factory stored under supplied ID
  /// TODO : better naming "unregister" for example
  /// COMMENT: "register"/"unregister" would be fine names but "register"
  ///          is a reserved word
  void unregistration(const std::string& id);

  /// Import registered factories from another factory register
  /// So, err, copy assignment in other words...
  /// COMMENT: not a copy, but an append.
  void import(const factory_register& factory_register);

  /// Simple print
  void print(std::ostream& out, const std::string& indent = "") const;

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string& title = "",
                         const std::string& indent_ = "",
                         bool inherit = false) const;

 private:
  std::string      label_;      /// Label of the factory
  bool             verbose_;    /// Verbosity flag
  factory_map_type registered_; /// Dictionary of registered factories
};

} // end of namespace datatools


//----------------------------------------------------------------------
// class factory_register implementation section
// TODO : refactor out into -inl.h file
namespace datatools {

template <typename BaseType>
factory_register<BaseType>::factory_register()
    : label_(),
      verbose_(false),
      registered_() {}

template <typename BaseType>
factory_register<BaseType>::factory_register(const std::string& label,
                                             unsigned int flags)
    : label_(label),
      verbose_(false) {
  if (flags & verbose) {
    verbose_ = true;
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Construction (label='" << label_ << "'...");
  }
  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Construction is done (label='" << label_ << "'...");
  }
}


/// Destructor
template <typename BaseType>
factory_register<BaseType>::~factory_register() {
  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destruction (label='" << label_ << "'...");
  }

  this->clear();

  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destruction is done (label='" << label_ << "'...");
  }
}


template <typename BaseType>
bool factory_register<BaseType>::is_verbose() const {
  return verbose_;
}


template <typename BaseType>
void factory_register<BaseType>::set_verbose(bool verbose) {
  verbose_ = verbose;
}


template <typename BaseType>
const std::string& factory_register<BaseType>::get_label() const {
  return label_;
}


template <typename BaseType>
const std::string& factory_register<BaseType>::label() const {
  return label_;
}


template <typename BaseType>
void factory_register<BaseType>::set_label(const std::string& label) {
  label_ = label;
}


template <typename BaseType>
void factory_register<BaseType>::list_of_factories(
    std::vector<std::string>& ids) const {
  for (typename factory_map_type::iterator i = registered_.begin();
       i != registered_.end();
       ++i) {
    ids.push_back(i->first);
  }
}


template <typename BaseType>
bool factory_register<BaseType>::has(const std::string& id) const {
  return registered_.find(id) != registered_.end();
}


template <typename BaseType>
void factory_register<BaseType>::clear() {
  for (typename factory_map_type::iterator i = registered_.begin();
       i != registered_.end();
       ++i) {
    if (i->second != 0) {
      if (this->is_verbose()) {
	DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destroying registered allocator/functor '" << i->first << "'");
      }
    }
  }
  registered_.clear();
}


template <typename BaseType>
void factory_register<BaseType>::reset() {
  this->clear();
  label_.clear();
  verbose_ = false;
}


template <typename BaseType>
typename factory_register<BaseType>::factory_type&
factory_register<BaseType>::grab(const std::string& id) {
  typename factory_map_type::iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
	       std::logic_error,
	       "Class ID '" << id << "' is not registered !");
  return found->second;
}


template <typename BaseType>
const typename factory_register<BaseType>::factory_type&
factory_register<BaseType>::get(const std::string& id) const {
  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
	       std::logic_error,
	       "Class ID '" << id << "' is not registered !");
  return found->second;
}


template <typename BaseType>
void factory_register<BaseType>::registration(const std::string& id,
                                              const factory_type& factory) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
		  "Registration of class with ID '" << id << "'");
  }

  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found != registered_.end(),
	       std::logic_error,
	       "Class ID '" << id << "' is already registered !");
  registered_[id] = factory;
}


template <typename BaseType>
void factory_register<BaseType>::unregistration(const std::string& id) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
		  "Unregistration of class with ID '" << id << "'");
  }
  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
	       std::logic_error,
	       "Class ID '" << id << "' is not registered !");
  registered_.erase(id);
}


template <typename BaseType>
void factory_register<BaseType>::import(const factory_register& other) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
		  "Importing registered factories from register '" << other.label() << "'...");
  }
  for (typename factory_map_type::const_iterator i = other.registered_.begin();
       i !=  other.registered_.end();
       ++i) {
    const factory_type& the_out_factory = i->second;
    this->registration(i->first, the_out_factory);
  }
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Done.");
  }
}


template <typename BaseType>
void factory_register<BaseType>::print(std::ostream& out,
                                       const std::string& indent) const {
  std::ostringstream title_oss;
  title_oss << "List of registered allocators/functors for label \""
            << label_
            << "\" : ";
  this->tree_dump(out, title_oss.str(), indent);
}


template <typename BaseType>
void factory_register<BaseType>::tree_dump(std::ostream& out,
                                           const std::string& title,
                                           const std::string& a_indent,
                                           bool inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!title.empty()) { out << indent << title << std::endl; }

  out << indent << i_tree_dumpable::tag
      << "Label   : '"
      << label_ << "'" << std::endl;
  out << indent << i_tree_dumpable::tag
      << "Verbose : "
      << verbose_ << std::endl;
  out << indent << i_tree_dumpable::inherit_tag(inherit)
      << "Registered factories : " << std::endl;

  for (typename factory_map_type::const_iterator i = registered_.begin();
       i != registered_.end ();
       ++i) {
    out << a_indent;
    out << i_tree_dumpable::inherit_skip_tag(inherit);
    typename factory_map_type::const_iterator j = i;
    j++;
    if (j == registered_.end()) {
      out << i_tree_dumpable::last_tag;
    } else {
      out << i_tree_dumpable::tag;
    }

    out << "ID: \"" << i->first << "\" @ " << &i->second << std::endl;
  }
}

} // namespace datatools



//----------------------------------------------------------------------
// Hmm, move this to a detail header?

#include <boost/type_traits/is_base_of.hpp>

namespace datatools {

/* Utility to enable auto-(un)registration of the Type class in the
 * the system factory register:
 */
/*! \brief Utility template class to enable auto-(un)registration of a derived class in a system factory register of a base class
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
    bool fatal = !boost::is_base_of<BaseType, DerivedType>::value;
    DT_THROW_IF (fatal,
		 std::logic_error,
		 "Class ID '" << type_id_ << "' cannot be registered in register '"
		 << BaseType::grab_system_factory_register().label() << "' !");
    BaseType::grab_system_factory_register().registration(type_id_,
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

} // end of namespace datatools

#endif // DATATOOLS_FACTORY_H_

