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
  class base_factory_register
  {
  public:
    enum flag_type {
      verbose = 0x1
    };

    /// Default constructor
    base_factory_register();

    /// Destructor
    virtual ~base_factory_register ();
  };


  /*! \brief Template factory registration class
   */
  template <class BaseType>
  class factory_register
    : public base_factory_register
    ,  public datatools::i_tree_dumpable
  {
  public:

    typedef BaseType                       base_type;
    typedef boost::function<base_type*() > factory_type;

    struct factory_record_type {
      std::string  type_id;
      factory_type fact;
      const std::type_info * tinfo = nullptr;
      std::string  description;
      std::string  category;
    };
    
    /// \brief Dictionary of object factories
    typedef std::map<std::string, factory_record_type> factory_map_type;

    /// Constructor
    factory_register();

    /// Constructor
    factory_register(const std::string & label_, unsigned int flags_ = 0x0);

    /// Destructor
    ~factory_register() override;

    //! Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    //! Set logging priority
    void set_logging_priority(datatools::logger::priority logging_);

    //! Get the label associated to the factory
    const std::string & get_label() const;

    //! Set the label associated to the factory
    void set_label(const std::string & label_);

    /// Copy factory IDs into supplied vector
    void list_of_factories(std::vector<std::string> & ids_) const;

    /// Return true if factory with given ID is registered
    bool has(const std::string & id_) const;

    /// Return true if factory with given ID is registered with given group
    bool is_group(const std::string & id_) const;

    /// Clear all registered factories
    void clear();

    /// Reset
    void reset();

    /// Return a mutable reference to a factory given by registration ID
    factory_type & grab(const std::string & id_);

    /// Return a const reference to a factory given by registration ID
    const factory_type & get(const std::string & id_) const;

    /// Return a const reference to a factory record given by registration ID
    const factory_record_type & get_record(const std::string & id_) const;

    /// Register the supplied factory under the given ID
    void registration(const std::string & id_,
                      const factory_type & factory_,
                      const std::type_info & tinfo_,
                      const std::string & description_ = "",
                      const std::string & category_ = "");

    /// Register the supplied factory under the given ID
    template<class DerivedType>
    void registration(const std::string & id_,
                      const std::string & description_ = "",
                      const std::string & category_ = "");

    /// Fetch the registration type ID associated to a given class
    template<class DerivedType>
    bool fetch_type_id(std::string & id_) const;

    /// Fetch the registration type ID associated to a given type_info
    bool fetch_type_id(const std::type_info & tinfo_, std::string & id_) const;
                      
    /// Remove registration of the factory stored under supplied ID
    /// TODO : better naming "unregister" for example
    /// COMMENT: "register"/"unregister" would be fine names but "register"
    ///          is a reserved word
    void unregistration(const std::string & id_);

    /// Import all registered factories from another factory register
    void import(const factory_register & factory_register_);

    /// Import only registered factories addressed by their registration names
    /// from another factory register
    void import_some(const factory_register & factory_register_,
                     const std::vector<std::string> & imported_factories_);

    /// Simple print
    void print(std::ostream & out_, const std::string & indent_ = "") const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string& title_ = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const override;

  private:

    std::string                 _label_;      //!< Label of the factory
    datatools::logger::priority _logging_;    //!< Verbosity flag
    factory_map_type            _registered_; //!< Dictionary of registered factories

  };

} // end of namespace datatools

// Template definitions:
#include <datatools/factory-inl.h>

// Boost:
#include <boost/type_traits/is_base_of.hpp>

namespace datatools {

  /*! \brief Utility template class to enable auto-(un)registration of a derived class in a system factory register of a base class
   */
  template <class BaseType, class DerivedType>
  class _system_factory_registrator
  {
  public:

    // Constructor
    _system_factory_registrator(const std::string & type_id_)
    {
      _type_info_ = &typeid(DerivedType);
      _type_id_ = type_id_;
      this->_trigger_factory_registration_();
      return;
    }

    // Destructor
    ~_system_factory_registrator()
    {
      this->_trigger_factory_unregistration_();
      return;
    }

    /// Return registered type id
    const std::string & get_type_id()const
    {
      return _type_id_;
    }

  private:

    // Factory registration
    void _trigger_factory_registration_()
    {
      bool fatal = !boost::is_base_of<BaseType, DerivedType>::value;
      DT_THROW_IF(fatal,
                  std::logic_error,
                  "Class ID '" << _type_id_ << "' cannot be registered in register '"
                  << BaseType::grab_system_factory_register().get_label() << "' !");
      BaseType::grab_system_factory_register().registration(_type_id_,
                                                            boost::factory<DerivedType*>(),
                                                            typeid(DerivedType));
      return;
    }

    // Factory unregistration
    void _trigger_factory_unregistration_()
    {
      if (BaseType::grab_system_factory_register().has(_type_id_)) {
        BaseType::grab_system_factory_register().unregistration(_type_id_);
      }
      return;
    }

  private:

    const std::type_info * _type_info_ = nullptr;
    std::string _type_id_; //!< The registration type unique identifier of the auto-registered class
    
    
  };

} // end of namespace datatools

#endif // DATATOOLS_FACTORY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
