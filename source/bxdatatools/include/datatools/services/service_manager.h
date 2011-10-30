/* service_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-10-29
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
#include <map>
#include <typeinfo>
#include <stdexcept>

#include <boost/cstdint.hpp>

#include <datatools/services/service_tools.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {
    class properties;
    class multi_properties;
  }

  namespace service {

    using namespace std;

    class service_manager : public datatools::utils::i_tree_dumpable
    {
    public:

      enum flag_type
        {
          BLANK              = 0x0,
          NO_PRELOAD         = 0x1,
          FORCE_INITIALIZATION_AT_LOAD = 0x2,
          DEBUG              = 0x4,
        };

      // ctor :
      service_manager (const string & a_name = "", 
                       const string & a_description = "", 
                       uint32_t a_flag = BLANK);
        
      // dtor :
      ~service_manager ();

      void set_name (const string & a_name);

      void set_description (const string & a_description);

      const string & get_name () const;

      const string & get_description () const;

      bool is_debug () const;

      void set_debug (bool = true);

    private:

      void _set_preload (bool);

    public:

      bool is_initialized () const;

      void initialize ();

      void initialize (const datatools::utils::properties & a_config);

      void reset ();
  
      virtual void tree_dump (ostream & a_out         = clog, 
                              const string & a_title  = "",
                              const string & a_indent = "",
                              bool a_inherit          = false) const;
          
    protected:

      void _create_service (service_entry & a_service_entry);

      void _initialize_service (service_entry & a_service_entry);

      void _reset_service (service_entry & a_service_entry);

    public:

      /**  @param a_service_name The name of the service to be checked
       *   @return true if the manager hosts the service requested by name
       */
      bool has (const string & a_service_name) const;

      /**  @param a_service_name The name of the service to be checked
       *   @return true if the service is of requested type
       */
      template <class T>
      bool is_a (const string & a_service_name) const
      {
        service_dict_type::const_iterator found = _services_.find (a_service_name);
        if (found == _services_.end ())
          {
            ostringstream message;
            message << "datatools::services::service_manager::is_a: No service named '" << a_service_name << "' !";
            throw logic_error (message.str ());
          }
        const type_info & ti = typeid(T);
        const type_info & tf = typeid(found->second.service_handle.get ());
        return (ti == tf); 
      }

      /** Deprecated, please use the 'grab' method.  
       * @param a_service_name The name of the service to be checked
       *   @return a mutable reference to the service instance requested by name and type
       */
      template<class T>
      T & get (const string & a_service_name)
      {
        service_dict_type::iterator found 
          = _services_.find (a_service_name);
        if (found == _services_.end ())
          {
            ostringstream message;
            message << "datatools::services::service_manager::get: " 
                    << "No service named '" << a_service_name << "' !";
            throw logic_error (message.str ());
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
      T & grab (const string & a_service_name)
      {
        return get<T> (a_service_name);
      }

      /**  @param a_service_name The name of the service to be checked
       *   @return a const reference to the service instance requested by name and type
       */
      template<class T>
      const T & get (const string & a_service_name) const
      {
        service_manager * sm = const_cast<service_manager *> (this);
        return const_cast<T &>(sm->get<T> (a_service_name));
      }

      bool can_drop (const string & a_service_name);

      void drop (const string & a_service_name);

      void load (const string & a_service_name,
                 const string & a_service_id,
                 const datatools::utils::properties & a_config);

      void load (const datatools::utils::multi_properties & a_config);
        
      const service_dict_type & get_services () const;

      service_dict_type & get_services ();
        
      void dump_services (ostream & a_out = clog,
                          const string & a_title = "",
                          const string & a_indent = "") const;
        
    protected:

      void _load_service (const string & a_service_name,
                          const string & a_service_id,
                          const datatools::utils::properties & a_service_config);

      void _preload_global_dict ();

    public:

      bool has_creator (const string & a_service_id) const;
 
      void register_creator (const service_creator_type & a_service_creator, 
                             const string & a_service_id);

      void unregister_creator (const string & a_service_id);
        
      const service_creator_dict_type & get_creators () const;   
        
      service_creator_dict_type & get_creators ();   

    private:

      string _name_;
      string _description_;
      bool   _debug_;
      bool   _preload_;
      bool   _force_initialization_at_load_;
      service_creator_dict_type _creators_;
      service_dict_type         _services_;
      bool                      _initialized_;

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
