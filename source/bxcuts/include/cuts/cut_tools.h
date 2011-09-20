/* cut_tools.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-07
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
 * 
 * Description: 
 *
 *   Typedefs for the cut factory.
 * 
 * History: 
 * 
 */

#ifndef __cuts__cut_tools_h
#define __cuts__cut_tools_h 1

#include <iostream>
#include <string>
#include <map>
#include <exception>

#include <boost/scoped_ptr.hpp>

#include <datatools/utils/handle.h>

namespace datatools {
  namespace utils {
    class properties;
  }
  namespace service {
    class service_manager;
  }
}

namespace cuts {

  using namespace std;

  class i_cut;
 
  typedef map<string, i_cut *> cut_dict_type;

  typedef datatools::utils::handle<i_cut> cut_handle_type;

  typedef map<string, cut_handle_type> cut_handle_dict_type;
      
  typedef i_cut * (*cut_creator_type) (const datatools::utils::properties & a_configuration,
                                       datatools::service::service_manager & a_service_manager, 
                                       cut_handle_dict_type & a_cut_dict);
      

  typedef map<string, cut_creator_type> cut_creator_dict_type;

  /****************************************************/

  class cut_creator_db
  {
    cut_creator_dict_type dict_;
        
  public:
        
    // ctor :
    cut_creator_db (bool = false);
        
    // dtor :
    virtual ~cut_creator_db ();
        
    const cut_creator_dict_type & get_dict () const;
        
    cut_creator_dict_type & get_dict ();
        
    bool has_cut_creator (const string & a_cut_id) const;
        
    cut_creator_type & get_cut_creator (const string & a_cut_id);
        
    void register_cut_creator (cut_creator_type a_cut_creator, 
                               const string & a_cut_id);
        
    void dump_cut_creators (ostream & a_out = clog);
        
  }; // cut_creator_db

  /****************************************************/

  class cut_tools {

    typedef boost::scoped_ptr<cut_creator_db> scoped_cut_creator_db_type;
        
  private:

    static scoped_cut_creator_db_type g_cut_creator_db_;

  public:
        
    static bool g_devel;

    static cut_creator_db & get_cut_creator_db ();

  }; // cut_tools

}  // end of cuts

#endif // __cuts__cut_tools_h

// end of cut_tools.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
