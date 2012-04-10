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
      service_manager (const std::string & a_name = "", 
                       const std::string & a_description = "", 
                       uint32_t a_flag = BLANK);
        
      /// Destructor 
      ~service_manager ();

      void set_name (const std::string & a_name);

      void set_description (const std::string & a_description);

      const std::string & get_name () const;

      const std::string & get_description () const;

      bool is_debug () const;

      void set_debug (bool = true);

    private:

      void _set_preload (bool);

    public:

      bool is_initialized () const;

      void initialize ();

      void initialize (const datatools::utils::properties & a_config);

      void reset ();
  
      virtual void tree_dump (std::ostream & a_out         = std::clog, 
                              const std::string & a_title  = "",
                              const std::string & a_indent = "",
                              bool a_inherit          = false) const;
          
    protected:

      void _create_service (service_entry & a_service_entry);

      void _initialize_service (service_entry & a_service_entry);

      void _reset_service (service_entry & a_service_entry);

    public:

      /**  @param a_service_name The name of the service to be checked
       *   @return true if the manager hosts the service requested by name
       */
      bool has (const std::string & a_service_name) const;

      /**  @param a_service_name The name of the service to be checked
       *   @return true if the service is of requested type
       */
      template <class T>
      bool is_a (const std::string & a_service_name) const
      {
        service_dict_type::const_iterator found = _services_.find (a_service_name);
        if (found == _services_.end ())
          {
            std::ostringstream message;
            message << "datatools::services::service_manager::is_a: No service named '" << a_service_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(found->second.service_handle.get ());
        return (ti == tf); 
      }

      /** Deprecated, please use the 'grab' method.  
       * @param a_service_name The name of the service to be checked
       *   @return a mutable reference to the service instance requested by name and type
       */
      template<class T>
      T & get (const std::string & a_service_name)
      {
        service_dict_type::iterator found 
          = _services_.find (a_service_name);
        if (found == _services_.end ())
          {
            std::ostringstream message;
            message << "datatools::services::service_manager::get: " 
                    << "No service named '" << a_service_name << "' !";
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
       *   @param a_service_name The name of the service to be checked
       *   @return a mutable reference to the service instance requested by name and type
       */
      template<class T>
      T & grab (const std::string & a_service_name)
      {
        return get<T> (a_service_name);
      }

      /**  @param a_service_name The name of the service to be checked
       *   @return a const reference to the service instance requested by name and type
       */
      template<class T>
      const T & get (const std::string & a_service_name) const
      {
        service_manager * sm = const_cast<service_manager *> (this);
        return const_cast<T &>(sm->get<T> (a_service_name));
      }

      bool can_drop (const std::string & a_service_name);

      void drop (const std::string & a_service_name);

      void load (const std::string & a_service_name,
                 const std::string & a_service_id,
                 const datatools::utils::properties & a_config);

      void load (const datatools::utils::multi_properties & a_config);
        
      const service_dict_type & get_services () const;

      service_dict_type & get_services ();
        
      void dump_services (std::ostream & a_out = std::clog,
                          const std::string & a_title = "",
                          const std::string & a_indent = "") const;
        
    protected:

      void _load_service (const std::string & a_service_name,
                          const std::string & a_service_id,
                          const datatools::utils::properties & a_service_config);

      void _preload_global_dict ();

    public:

      bool has_service_type (const std::string & a_service_id) const;

      template <class ServiceClass>
      void register_service_type (const std::string & a_service_id)
      {
        _factory_register_.registration (a_service_id, boost::factory<ServiceClass*>());   
        return;
      }

      void unregister_service_type (const std::string & a_service_id);

    private:

      bool        _initialized_;
      std::string _name_;
      std::string _description_;
      bool   _debug_;
      bool   _preload_;
      bool   _force_initialization_at_load_;
      // 2012-04-09 FM : support for datatools::factory system :
      base_service::factory_register_type _factory_register_;
      service_dict_type         _services_;

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
