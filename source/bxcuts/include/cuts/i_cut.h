/* i_cut.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-05-16
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef CUTS_I_CUT_H_
#define CUTS_I_CUT_H_ 1

#include <ostream>
#include <string>
#include <typeinfo>

#include <boost/cstdlib.hpp>
#include <boost/shared_ptr.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>
#include <datatools/bit_mask.h>
#include <datatools/logger.h>

#include <cuts/cut_tools.h>

namespace datatools {
  class properties;
  class service_manager;
}

/// Top-level namespace of the Bayeux/cuts module library
namespace cuts {

  /// \brief The cut abstract base class
  class i_cut : public datatools::i_tree_dumpable
  {
  public:

    /// \brief An abstract class for daughter templatized data wrapper classes
    struct i_referenced_data {
      virtual operator bool() const = 0;
      virtual const std::type_info * get_typeinfo() const = 0;
      virtual bool match(const std::type_info * tit_) const = 0;
      virtual ~i_referenced_data() {}
    };

    /// \brief A weak reference to an arbitrary object with tracked ``type_info``
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
      virtual const std::type_info * get_typeinfo() const {
        return _ti;
      }
      virtual bool match(const std::type_info * tit_) const {
        return tit_ == _ti;
      }
      void set(const T & obj) {
        _address = &obj;
        _ti = &typeid(T);
      }
      virtual operator bool() const {
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
      virtual ~referenced_data() {
        _address = 0;
        _ti = 0;
      }
    protected:
      const T              * _address;
      const std::type_info * _ti;
    };

    bool is_debug () const;

    bool has_name () const;

    void set_name (const std::string & a_name);

    const std::string & get_name () const;

    bool has_description () const;

    const std::string & get_description () const;

    void set_description (const std::string & a_description);

    bool has_version () const;

    const std::string & get_version () const;

    void set_version (const std::string & a_version);

    /// Check initialization status
    bool is_initialized () const;

    /// Check if some user data is referenced
    bool has_user_data () const;

    template<class T>
    void set_user_data(const T & obj_)
    {
      DT_LOG_TRACE(_logging, "Adding user data of type \"" << typeid(T).name() << "\" in cut named '" << (has_name()?get_name():"?") << "'...");
      boost::shared_ptr<i_referenced_data> ud(new referenced_data<T>(obj_));
      _set_user_data(ud);
    }

    template<class T>
    bool is_user_data() const
    {
      DT_THROW_IF(! _user_data_,
                  std::logic_error,
                  "Cut '" << (has_name()?get_name():"?") << "' references no user data of type '" << typeid(T).name() << "' !");
      const std::type_info & ti = typeid(T);
      return _user_data_.get()->match(&ti);
    }

    template<class T>
    const T & get_user_data() const
    {
      DT_THROW_IF(! is_user_data<T>(),
                  std::logic_error,
                  "Invalid request on user data !");
      const referenced_data<T> * rd = dynamic_cast<const referenced_data<T> *>(_user_data_.get());
      DT_THROW_IF(rd == 0,
                  std::logic_error,
                  "Invalid cast !");
      return rd->get();
    }

    /// Clear the referenced user data
    void reset_user_data ();

    /// Naked initialization method (default implementation, @see initialize)
    virtual void initialize_simple();

    /// Initialization method through a container of configuration properties (default implementation, @see initialize)
    virtual void initialize_standalone(const datatools::properties & a_config);

    /// Initialization method through a container of configuration properties
    /// and a service manager (default implementation, @see initialize)
    virtual void initialize_with_service_only(const datatools::properties & a_config,
                                              datatools::service_manager & a_service_manager);

    /// Initialization method through a container of configuration properties
    /// and a dictionnary of cuts (default implementation, @see initialize)
    virtual void initialize_without_service(const datatools::properties & a_config,
                                            cut_handle_dict_type & a_cut_dictionnary);

    /** The main initialization method (post-construction):
     * @param a_config the container of configuration parameters
     * @param a_cut_dictionnary a dictionnary of existing 'cuts' which can be used
     *        to build the current cut (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */
    virtual void initialize(const datatools::properties & a_config,
                            datatools::service_manager & a_service_manager,
                            cut_handle_dict_type & a_cut_dictionnary) = 0;

    /** The main cut processing method
     * @return the selection status
     */
    virtual int process();

    /// Function interface for the selection method @see process
    int operator()();

    /** The main termination method
     */
    virtual void reset() = 0;

    /// Constructor
    explicit i_cut(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~i_cut();

    /// Smart print
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    /// Print shortcut @see tree_dump()
    void print (std::ostream & a_out = std::clog) const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority p);

    /// Get the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

  protected:

    /// The main selection method (pure virtual, invoked by the @see process method)
    virtual int _accept () = 0;

    void _set_user_data(const boost::shared_ptr<i_referenced_data> & hd_);

    void _common_initialize (const datatools::properties & a_config);

    void _import_user_data_from(const i_cut &);

    void _export_user_data_to(i_cut &) const;

    void _set_name (const std::string & a_name);

    void _set_initialized (bool a_initialized);

    /// Hook invoked before the main selection method @see _accept ()
    virtual void _prepare_cut ();

    /// Hook invoked after the main selection method @see _accept ()
    virtual int _finish_cut (int a_selection_status);

    virtual void _at_set_user_data();

    virtual void _at_reset_user_data();

    void _reset();

  protected:

    datatools::logger::priority _logging; //!< Logging priority threshold
    std::string  _name;          //!< The name of the cut
    std::string  _description;   //!< The description of the cut
    std::string  _version;       //!< The version of the cut

  private:

    bool         _initialized_;  //!< The initialization flag
    boost::shared_ptr<i_referenced_data> _user_data_; //!< Internal weak (not managed) handle to user data;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_cut);

  };

}  // end of namespace cuts

#include <cuts/cut_macros.h>

#endif // CUTS_I_CUT_H_

// end of i_cut.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
