/* geometry_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-03-09
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
 *   Generic geometry service.
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_GEOMETRY_SERVICE_H_
#define GEOMTOOLS_GEOMETRY_SERVICE_H_ 1

#include <iostream>
#include <string>

// Service interface :
#include <datatools/base_service.h>
#include <datatools/service_tools.h>
#include <datatools/service_macros.h>

namespace datatools {
  class properties;
}

namespace geomtools {
  
  class manager;

  class geometry_service : public datatools::base_service
  {
  public:
    
    const geomtools::manager & get_geom_manager () const;
 
  public:

    virtual bool is_initialized () const;

    virtual int initialize (const datatools::properties & a_config,
                            datatools::service_dict_type & a_service_dict);

    virtual int reset ();
    
  public: 

    // ctor:
    geometry_service ();
 
    // dtor:
    virtual ~geometry_service ();
 
    virtual void tree_dump (std::ostream & a_out         = std::clog, 
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const;

  private:

    bool                 _initialized_;
    geomtools::manager * _geom_manager_;

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE (geometry_service);
   
  };

}  // end of namespace geomtools

#endif // GEOMTOOLS_GEOMETRY_SERVICE_H_

// end of geometry_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
