/* cut_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-20
 * Last modified : 2011-09-20
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
 * Description: 
 *
 *   A cut distribution service embedding a cut manager.
 * 
 * History: 
 * 
 */

#ifndef __cuts__cut_service_h
#define __cuts__cut_service_h 1

#include <string>

#include <datatools/services/base_service.h>
#include <datatools/services/service_tools.h>
#include <datatools/services/service_macros.h>

// Forward declaration :
namespace datatools {
  namespace utils {
    class properties;
  }
}

namespace cuts {

  using namespace std;

  // Forward declaration :
  class cut_manager;
  
  class cut_service : public datatools::service::base_service
    {
    public:
                   
      bool owns_cut_manager () const;
             
      const cut_manager & get_cut_manager () const;
                                
      void set_cut_manager (const cut_manager & a_cut_manager);
        
    public:

      virtual bool is_initialized () const;

      virtual int initialize (const datatools::utils::properties & a_config,
                              datatools::service::service_dict_type & a_service_dict); 

      virtual int reset ();
                  
    public: 

      // ctor:
      cut_service ();
        
      // dtor:
      virtual ~cut_service ();
        
      virtual void tree_dump (ostream & a_out         = clog, 
                              const string & a_title  = "",
                              const string & a_indent = "",
                              bool a_inherit          = false) const;

    private:

      bool          _owns_manager_; //!< Flag for embedded cut manager ownership
      cut_manager * _cut_manager_;  //!< Handle to the embedded cut manager

      // Registration :
      DATATOOLS_SERVICE_REGISTRATION_INTERFACE (cut_service);
                 
    };

}  // end of namespace cuts

#endif // __cuts__cut_service_h

// end of cut_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
