/* i_data_processor.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-30
 * Last modified : 2013-02-15
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
 *   A simple interface class for data processors
 * 
 * History: 
 * 
 */

#ifndef DPP_I_DATA_PROCESSOR_H_
#define DPP_I_DATA_PROCESSOR_H_ 1

#include <cstdlib>

#include <dpp/dpp_config.h>

// Data record :
#include <datatools/things.h>

namespace dpp {

  /// \brief Data record processor basic interface
  class i_data_processor
  {
  public: 
        
    enum status_type
      {
        STATUS_OK = EXIT_SUCCESS,
        STATUS_ERROR = EXIT_FAILURE,
        STATUS_FAILURE = STATUS_ERROR
      };

    static bool is_ok (int a_status)
    {
      return a_status == STATUS_OK;
    }

    static bool is_error (int a_status)
    {
      return ! is_ok (a_status);
    }

    /// Main data record processing method
    virtual int process (datatools::things & a_data_record) = 0;

  };

}  // end of namespace dpp

#endif // DPP_I_DATA_PROCESSOR_H_ 

// end of i_data_processor.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
