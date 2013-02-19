/* base_cut.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-18
 * Last modified : 2013-02-18
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
 *   Base processing cut on data record.
 * 
 */

#ifndef DPP_BASE_CUT_H_
#define DPP_BASE_CUT_H_ 1

#include <ostream>
#include <string>

// SuperNEMO data models :
#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <dpp/dpp_config.h>

namespace datatools {
#if DPP_DATATOOLS_LEGACY == 1
  namespace utils {
#endif
    class properties;
    class things;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif

#if DPP_DATATOOLS_LEGACY == 1
  namespace service {
#endif
    class service_manager;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif
}

namespace dpp {
                        
  CUT_CLASS_DECLARE (base_cut)  
  {    
                                
  public: 
                                
    /// Constructor
    base_cut (const std::string & a_cut_name = "", 
              const std::string & a_cut_description = "", 
              const std::string & a_cut_version = "", 
              int a_debug_level = 0);
                                
    // Destructor
    CUT_DESTRUCTOR_DECLARE (base_cut);
                                
    CUT_RESET_DECLARE ();
                               
    const DPP_DU::things & get_event_record () const;
              
    // Does it make sense to access the operand (event record) in mutable mode ? probably no !     
    // DPP_DU::things & grab_event_record ();
  
  };
        
}  // end of namespace dpp

#endif // DPP_BASE_CUT_H_

// end of base_cut.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
