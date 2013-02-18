/* context_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-16
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
 *   Context service.
 * 
 * History: 
 * 
 */

#ifndef DPP_CONTEXT_SERVICE_H_
#define DPP_CONTEXT_SERVICE_H_ 1

#include <iostream>
#include <string>

#include <dpp/dpp_config.h>

// Service interface :
#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/services/base_service.h>
#include <datatools/services/service_tools.h>
#include <datatools/services/service_macros.h>
#else
#include <datatools/base_service.h>
#include <datatools/service_tools.h>
#include <datatools/service_macros.h>
#endif

namespace datatools {
#if DPP_DATATOOLS_LEGACY == 1
  namespace utils {
#endif
    class multi_properties;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif
}

namespace dpp {

  /// \brief A service that handles a context object store implemented as a datatools' multi_properties container
  class context_service : 
    public DPP_DS::base_service
  {
  public:
    
    const DPP_DU::multi_properties & get_store () const;

    DPP_DU::multi_properties & get_store ();

    const DPP_DU::multi_properties & operator () () const;      

    DPP_DU::multi_properties & operator () ();      
  
    bool is_debug () const;

    void set_debug (bool);
 
    bool is_verbose () const;

    void set_verbose (bool);

    virtual bool is_initialized () const;

    virtual int initialize (const DPP_DU::properties & a_config,
                            DPP_DS::service_dict_type & a_service_dict);

    virtual int reset ();
    
  public: 

    // ctor:
    context_service ();
 
    // dtor:
    virtual ~context_service ();
 
    virtual void tree_dump (std::ostream & a_out         = std::clog, 
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const;

  private:

    bool initialized_;
    bool debug_;
    bool verbose_;
        
    DPP_DU::multi_properties * store_;

    std::string load_filename_;
    std::string store_filename_;
    std::string backup_filename_;

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE (context_service);
   
  };

}  // end of namespace dpp

#endif // DPP_CONTEXT_SERVICE_H_

// end of context_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
