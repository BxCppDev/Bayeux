/* service_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2012-04-10
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * Description:
 *
 *   A service manager.
 *
 * History:
 *
 */

#ifndef __datatools__service__service_manager_h
#define __datatools__service__service_manager_h 1

#include <string>
#include <sstream>
//#include <map>
#include <typeinfo>
#include <stdexcept>

#include <boost/cstdint.hpp>

#include <datatools/services/service_tools.h>
#include <datatools/services/base_service.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {
    class properties;
    class multi_properties;
  }

  namespace service {

    //using   namespace std;

    class service_manager : public datatools::utils::i_tree_dumpable
    {
    public:

      enum flag_type
        {
          BLANK              = 0x0,
          NO_PRELOAD         = 0x1,
          FORCE_INITIALIZATION_AT_LOAD = 0x2,
          DEBUG              = 0x4,
          VERBOSE            = 0x8,
        };

      /// Constructor
      service_manager (const std::string & name_        = "",
                       const std::string & description_ = "",
                       uint32_t flag_ = BLANK);

      /// Destructor
      ~service_manager ();

      void set_name (const std::string & name_);

      void set_description (const std::string & description_);

      const std::string & get_name () const;

      const std::string & get_description () const;

      bool is_debug () const;

      void set_debug (bool debug_ = true);

    private:

      void _set_preload (bool preload_);

    public:

      bool is_initialized () const;

      void initialize ();

      void initialize (const datatools::utils::properties & config_);

      void reset ();

      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

    protected:

      void _create_service (service_entry & service_entry_);

      void _initialize_service (service_entry & service_entry_);

      void _reset_service (service_entry & service_entry_);

    public:

      /**  @param service_name_ The name of the service to be checked
       *   @return true if the manager hosts the service requested by name
       */
      bool has (const std::string & service_name_) const;

      /**  @param service_name_ The name of the service to be checked
       *   @return true if the service is of requested type
       */
      template <class T>
      bool is_a (const std::string & service_name_) const
      {
        service_dict_type::const_iterator found = _services_.find (service_name_);
        if (found == _services_.end ())
          {
            std::ostringstream message;
            message << "datatools::services::service_manager::is_a: No service named '" << service_name_ << "' !";
            throw std::logic_error (message.str ());
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(found->second.service_handle.get ());
        return (ti == tf);
      }

      /** Deprecated, please use the 'grab' method.
       *  @param service_name_ The name of the service to be checked
       *  @return a mutable reference to the service instance requested by name and type
       */
      template<class T>
      T & get (const std::string & service_name_)
      {
        service_dict_type::iterator found
          = _services_.find (service_name_);
        if (found == _services_.end ())
          {
            std::ostringstream message;
            message << "datatools::services::service_manager::get: "
                    << "No service named '" << service_name_ << "' !";
            throw std::logic_error (message.str ());
          }
        service_entry & the_service_entry = found->second;
        if (! (the_service_entry.service_status & service_entry::STATUS_INITIALIZED))
          {
            _initialize_service (the_service_entry);
          }
        return dynamic_cast<T &> (the_service_entry.service_handle.get ());
      }

      /**  Same as the mutable 'get' method
       *   @param service_name_ The name of the service to be checked
       *   @return a mutable reference to the service instance requested by name and type
       */
      template<class T>
      T & grab (const std::string & service_name_)
      {
        return get<T> (service_name_);
      }

      /**  @param service_name_ The name of the service to be checked
       *   @return a const reference to the service instance requested by name and type
       */
      template<class T>
      const T & get (const std::string & service_name_) const
      {
        service_manager * sm = const_cast<service_manager *> (this);
        return const_cast<T &>(sm->get<T> (service_name_));
      }

      bool can_drop (const std::string & service_name_);

      void drop (const std::string & service_name_);

      void load (const std::string & service_name_,
                 const std::string & service_id_,
                 const datatools::utils::properties & config_);

      void load (const datatools::utils::multi_properties & config_);

      const service_dict_type & get_services () const;

      service_dict_type & get_services ();

      void dump_services (std::ostream      & out_    = std::clog,
                          const std::string & title_  = "",
                          const std::string & indent_ = "") const;

    protected:

      void _load_service (const std::string & service_name_,
                          const std::string & service_id_,
                          const datatools::utils::properties & service_config_);

      void _preload_global_dict ();

    public:

      bool has_service_type (const std::string & service_id_) const;

      template <class ServiceClass>
      void register_service_type (const std::string & service_id_)
      {
        _factory_register_.registration (service_id_, boost::factory<ServiceClass*>());
        return;
      }

      void unregister_service_type (const std::string & service_id_);

    private:

      bool         _initialized_;
      std::string  _name_;
      std::string  _description_;
      bool         _debug_;
      bool         _preload_;
      bool         _force_initialization_at_load_;

      // 2012-04-09 FM : support for datatools::factory system :
      base_service::factory_register_type  _factory_register_;
      service_dict_type                    _services_;

    };

  }  // end of namespace service

}  // end of namespace datatools

#endif // __datatools__service__service_manager_h

// end of service_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
