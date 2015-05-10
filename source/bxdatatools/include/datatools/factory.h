/// \file datatools/factory.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-19
 * Last modified : 2013-04-22
 *
 */
#ifndef DATATOOLS_FACTORY_H
#define DATATOOLS_FACTORY_H

// Standard Library:
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third Party:
// - Boost:
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/scoped_ptr.hpp>

// This project:
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
    /// Default constructor
    base_factory_register();
    /// Destructor
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

    /// Constructor
    factory_register();

    /// Constructor
    factory_register(const std::string& label, unsigned int flags = 0x0);

    /// Destructor
    virtual ~factory_register();

    //! Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    //! Set logging priority
    void set_logging_priority(datatools::logger::priority logging);

    //! Get the label associated to the factory
    const std::string& get_label() const;

    //! Set the label associated to the factory
    void set_label(const std::string& label);

    /// Copy factory IDs into supplied vector
    void list_of_factories(std::vector<std::string>& ids) const;

    /// Return true if factory with given ID is registered
    bool has(const std::string& id) const;

    /// Return true if factory with given ID is registered with given group
    bool is_group(const std::string& id) const;

    /// Clear all registered factories
    void clear();

    /// Reset
    void reset();

    /// Return a mutable reference to a factory given by registration ID
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

    /// Import all registered factories from another factory register
    void import(const factory_register & factory_register);

    /// Import only registered factories addressed by their registration names
    /// from another factory register
    void import_some(const factory_register & factory_register,
                     const std::vector<std::string> & imported_factories);

    /// Simple print
    void print(std::ostream& out, const std::string & indent = "") const;

    /// Smart print
    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string& title = "",
                           const std::string& indent_ = "",
                           bool inherit = false) const;

  private:
    std::string      label_;              //!< Label of the factory
    datatools::logger::priority logging_; //!< Verbosity flag
    factory_map_type registered_;         //!< Dictionary of registered factories
  };

} // end of namespace datatools

// Template definitions:
#include <datatools/factory-inl.h>

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
    // Constructor
    _system_factory_registrator(const std::string& type_id) {
      type_id_ = type_id;
      this->_trigger_factory_registration_();
    }
    // Return registered type id
    const std::string& get_type_id()const
    {
      return type_id_;
    }
    // Destructor
    ~_system_factory_registrator() {
      this->_trigger_factory_unregistration_();
    }

  private:
    // Factory registration
    void _trigger_factory_registration_() {
      bool fatal = !boost::is_base_of<BaseType, DerivedType>::value;
      DT_THROW_IF (fatal,
                   std::logic_error,
                   "Class ID '" << type_id_ << "' cannot be registered in register '"
                   << BaseType::grab_system_factory_register().get_label() << "' !");
      BaseType::grab_system_factory_register().registration(type_id_,
                                                            boost::factory<DerivedType*>());
    }

    // Factory unregistration
    void _trigger_factory_unregistration_() {
      if (BaseType::grab_system_factory_register().has(type_id_)) {
        BaseType::grab_system_factory_register().unregistration(type_id_);
      }
    }

  private:
    std::string type_id_; //!< The registration type unique identifier of the auto-registered class
  };

} // end of namespace datatools

#endif // DATATOOLS_FACTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
