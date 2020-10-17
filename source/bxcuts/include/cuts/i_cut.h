//! \file cuts/i_cut.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2015-11-09
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Base cut.
 *
 * History:
 *
 */

#ifndef CUTS_I_CUT_H
#define CUTS_I_CUT_H 1

// Standard library:
#include <ostream>
#include <string>
#include <typeinfo>
#include <set>

// Third party:
// - Boost:
#include <boost/cstdlib.hpp>
#include <boost/shared_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>
#include <datatools/bit_mask.h>
#include <datatools/logger.h>

// This project:
#include <cuts/cut_tools.h>

namespace datatools {
  class properties;
  class service_manager;
}

//! Top-level namespace of the Bayeux/cuts module library
namespace cuts {

  //! \brief The cut abstract base class (interface)
  class i_cut : public datatools::i_tree_dumpable
  {
  public:

    //! \brief An abstract class for daughter templatized data wrapper classes
    struct i_referenced_data {
      virtual operator bool() const = 0;
      virtual const std::type_info * get_typeinfo() const = 0;
      virtual bool match(const std::type_info * tit_) const = 0;
      virtual ~i_referenced_data() {}
    };

    //! \brief A weak reference to an arbitrary object with tracked ``type_info``
    template<class T>
    struct referenced_data : public i_referenced_data {
    public:
      const T & get() const {
        DT_THROW_IF(_address == 0,
                    std::logic_error,
                    "No referenced data !"
                    );
        return *_address;
      }
      const std::type_info * get_typeinfo() const override {
        return _ti;
      }
      bool match(const std::type_info * tit_) const override {
        return tit_ == _ti;
      }
      void set(const T & obj) {
        _address = &obj;
        _ti = &typeid(T);
      }
      operator bool() const override {
        return _address != 0 && _ti != 0;
      }
      void reset() {
        _address = 0;
        _ti = 0;
      }
      referenced_data() {
        _address = 0;
        _ti = 0;
      }
      referenced_data(const T & obj) {
        set(obj);
      }
      ~referenced_data() override {
        _address = 0;
        _ti = 0;
      }
    protected:
      const T              * _address; //!< Address of the referenced object
      const std::type_info * _ti;      //! Reference of the referenced object's type
    };

    //! Check if logging priority is at least at debug level
    bool is_debug() const;

    //! Check if the name of the cut is set
    bool has_name() const;

    //! Set the name of the cut
    void set_name(const std::string & a_name);

    //! Return the name of the cut
    const std::string & get_name() const;

    //! Check if the description of the cut is set
    bool has_description() const;

    //! Return the description of the cut
    const std::string & get_description() const;

    //! Set the description of the cut
    void set_description(const std::string & a_description);

    //! Check if the version of the cut is set
    bool has_version() const;

    //! Return the version of the cut
    const std::string & get_version() const;

    //! Return the version of the cut
    void set_version(const std::string & a_version);

    //! Check initialization status
    bool is_initialized() const;

    //! Check if some user data is referenced
    bool has_user_data() const;

    //! Set user data
    template<class T>
    void set_user_data(const T & obj_)
    {
      DT_LOG_TRACE(_logging,
                   "Cut '" << (has_name() ? get_name() : "?") << "' : "
                   << "adding user data of type '" << typeid(T).name() << "'...");
      // DT_THROW_IF(!is_user_data_type_supported<T>(),
      //             std::logic_error,
      //             "User data type '" << typeid(T).name() << "' is not supported by cut '" << get_name() << "'!");
      boost::shared_ptr<i_referenced_data> ud(new referenced_data<T>(obj_));
      _set_user_data(ud);
    }

    //! Check user data type
    template<class T>
    bool is_user_data() const
    {
      DT_THROW_IF(! _user_data_,
                  std::logic_error,
                  "Cut '" << (has_name()?get_name():"?") << "' does not reference any user data of type '" << typeid(T).name() << "' !");
      const std::type_info & ti = typeid(T);
      return _user_data_.get()->match(&ti);
    }

    //! Get user data of given type
    template<class T>
    const T & get_user_data() const
    {
      DT_THROW_IF(! is_user_data<T>(),
                  std::logic_error,
                  "Invalid request on user data for cut named '" << (has_name()?get_name():"?") << "' !");
      const referenced_data<T> * rd = dynamic_cast<const referenced_data<T> *>(_user_data_.get());
      DT_THROW_IF(rd == 0,
                  std::logic_error,
                  "Invalid cast for reference data in cut named '" << (has_name()?get_name():"?") << "' !");
      return rd->get();
    }

    //! Register user data type is supported
    template<class T>
    void register_supported_user_data_type()
    {
      const std::type_info & ti = typeid(T);
      DT_LOG_DEBUG(_logging, "Registering supported type '" << ti.name() << "' !!!");
      _supported_types_.insert(&ti);
      return;
    }

    //! Unregister user data type is supported
    template<class T>
    void unregister_supported_user_data_type()
    {
      const std::type_info & ti = typeid(T);
      DT_LOG_DEBUG(_logging, "Unregistering supported type '" << ti.name() << "' !!!");
      _supported_types_.erase(&ti);
      return;
    }

    //! Check if data type is supported
    template<class T>
    bool is_user_data_type_supported() const
    {
      if (_supported_types_.size() == 0) {
        DT_LOG_TRACE(_logging, "Supported type");
        return true;
      }
      DT_LOG_TRACE(_logging, "Checking supported type...");
      const std::type_info & ti = typeid(T);
      if (_supported_types_.count(&ti) == 1) {
        DT_LOG_TRACE(_logging, "Supported registered type.");
        return true;
      }
      return false;
    }

    //! Check if a data type is supported
    bool is_user_data_type_supported(const std::type_info & tinfo_) const;

    //! Clear the referenced user data
    void reset_user_data();

    //! Naked initialization method (default implementation, @see initialize)
    virtual void initialize_simple();

    //! Initialization method through a container of configuration properties (default implementation, @see initialize)
    virtual void initialize_standalone(const datatools::properties & a_config);

    //! Initialization method through a container of configuration properties
    //! and a service manager (default implementation, @see initialize)
    virtual void initialize_with_service_only(const datatools::properties & a_config,
                                              datatools::service_manager & a_service_manager);

    //! Initialization method through a container of configuration properties
    //! and a dictionary of cuts (default implementation, @see initialize)
    virtual void initialize_without_service(const datatools::properties & a_config,
                                            cut_handle_dict_type & a_cut_dictionary);

    /** The main initialization method (post-construction)
     *
     * @param a_config the container of configuration parameters
     * @param a_cut_dictionary a dictionary of existing 'cuts' which can be used
     *        to build the current cut (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */
    virtual void initialize(const datatools::properties & a_config,
                            datatools::service_manager & a_service_manager,
                            cut_handle_dict_type & a_cut_dictionary) = 0;

    //! \brief Flags for export to a container of properties
    enum export_config_flags {
      EXPORT_CONFIG_CLEAR             = datatools::bit_mask::bit00,
      EXPORT_CONFIG_NAME              = datatools::bit_mask::bit01,
      EXPORT_CONFIG_DESCRIPTION       = datatools::bit_mask::bit02,
      EXPORT_CONFIG_VERSION           = datatools::bit_mask::bit03,
      EXPORT_CONFIG_LOGGING_PRIORITY  = datatools::bit_mask::bit04,
      EXPORT_CONFIG_DEFAULT           = (EXPORT_CONFIG_CLEAR |
                                         EXPORT_CONFIG_DESCRIPTION |
                                         EXPORT_CONFIG_VERSION),
      EXPORT_CONFIG_LAST              = EXPORT_CONFIG_LOGGING_PRIORITY
    };

    //! Export to a container of properties
    virtual void export_to_config(datatools::properties & config_,
                                  uint32_t flags_ = EXPORT_CONFIG_DEFAULT,
                                  const std::string & prefix_ = "") const;

    /** The main cut processing method
     * @return the selection status
     */
    virtual int process();

    //! Function interface for the selection method @see process
    int operator()();

    //! The main termination method
    virtual void reset() = 0;

    //! Constructor
    explicit i_cut(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

    //! Destructor
    ~i_cut() override;

    //! Smart print
    void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const override;

    //! Print shortcut @see tree_dump()
    void print(std::ostream & a_out = std::clog) const;

    //! Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority p);

    //! Get the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    //! Basic OCD support shared by all inherited modules
    static void common_ocd(datatools::object_configuration_description & ocd_);

    //! Return the number of entries accepted by the cut
    size_t get_number_of_accepted_entries() const;

    //! Return the number of entries rejected by the cut
    size_t get_number_of_rejected_entries() const;

    //! Return the total number of entries processed by the cut
    size_t get_number_of_processed_entries() const;

    //! Reset the embedded counters
    void reset_counters();

    //! Check the flag to activate counters
    bool is_activated_counters() const;

    //! Set the flag to activate statistics
    void set_activated_counters(bool);

  protected:

    //! The main selection method (pure virtual, invoked by the @see process method)
    virtual int _accept() = 0;

    //! Set user data by shared pointer
    void _set_user_data(const boost::shared_ptr<i_referenced_data> & hd_);

    //! Common initialization of all cuts
    void _common_initialize(const datatools::properties & a_config);

    //! Import user data from another cut
    void _import_user_data_from(const i_cut &);

    //! Export user data to another cut
    void _export_user_data_to(i_cut &) const;

    //! Set the cut name
    void _set_name(const std::string & a_name);

    //! Set the initialization flag
    void _set_initialized(bool a_initialized);

    //! Hook invoked before the main selection method @see _accept ()
    virtual void _prepare_cut();

    //! Hook invoked after the main selection method @see _accept ()
    virtual int _finish_cut(int a_selection_status);

    //! Increment statistics counters
    virtual void _increment_counters(int a_selection_status);

    //! Hook executed when user data is set
    virtual void _at_set_user_data();

    //! Hook executed when user data is reset
    virtual void _at_reset_user_data();

    //! Reset the cut
    void _reset();

    //! Set default attributes values
    void _set_defaults();

  protected:

    // Configuration parameters:
    datatools::logger::priority _logging; //!< Logging priority threshold
    std::string  _name;          //!< The name of the cut
    std::string  _description;   //!< The description of the cut
    std::string  _version;       //!< The version of the cut
    std::set<const std::type_info *> _supported_types_; //!< The set of supported user data types

  private:

    // Status:
    bool _initialized_; //!< The initialization flag

    // Working data:
    bool _activated_counters_; //!< Flag to activate statistics counters
    boost::shared_ptr<i_referenced_data> _user_data_; //!< Internal weak (not managed) handle to user data
    size_t _number_of_accepted_entries_; //!< Number of entries accepted by the cut
    size_t _number_of_rejected_entries_; //!< Number of entries rejected by the cut

  public:

    //! Return the unique type identifier associated to the class of the cut object
    virtual std::string get_type_id() const = 0;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_cut)

  };

}  // end of namespace cuts

/** Interface macro for automated registration of a cut class in the global register */
#define CUT_REGISTRATION_INTERFACE(T)           \
  public:                                       \
  std::string get_type_id() const override; \
  private:                                                                \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::cuts::i_cut, T) \
  /**/

/** Implementation macro for automated registration of a cut class in the global register */
#define CUT_REGISTRATION_IMPLEMENT(T,CutID)               \
  std::string T::get_type_id() const { return CutID; } \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::cuts::i_cut, T, CutID) \
  /**/

#endif // CUTS_I_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
