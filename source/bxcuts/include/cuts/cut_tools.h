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

  class i_cut;
 
  typedef std::map<std::string, i_cut *> cut_dict_type;

  typedef datatools::utils::handle<i_cut> cut_handle_type;

  typedef std::map<std::string, cut_handle_type> cut_handle_dict_type;

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
