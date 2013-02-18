/* brio_common.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
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
 *
 * Description:
 *
 *    Common resources for BRIO I/O
 *
 * History:
 *
 */

/**
 *
 * Standard BRIO file format for physics event data is :
 *
 *   Store: "GI" (general informations as 'DPP_DU::properties' [optional])
 *    - header (DPP_DU::properties) [optional]
 *    - footer (DPP_DU::properties) [optional]
 *   Store: "ER" (event record as 'DPP_DU::things' (things))
 *    - event record #0
 *    - event record #1
 *    - event record #2
 *    - ...
 *    - event record #N
 *
 */

#ifndef DPP_BRIO_COMMON_H_
#define DPP_BRIO_COMMON_H_ 1

#include <string>

namespace dpp {

  /// \brief brio I/O constants
  struct brio_common
  {
    static const std::string GENERAL_INFO_STORE_LABEL;
    static const std::string EVENT_RECORD_STORE_LABEL;
  };
  
} // end of namespace dpp

#endif // DPP_BRIO_COMMON_H_

// end of brio_common.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
