/// \file datatools/configuration/variant_dependency_utils.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-26
 * Last modified : 2016-10-26
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Utilities for variant dependency.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_UTILS_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_UTILS_H

// Standard library:
#include <memory>

namespace datatools {

  namespace configuration {

    class base_dependency_logic;

    // Type alias for smart pointer on base_dependency_logic objects;
    typedef std::shared_ptr<base_dependency_logic> dependency_logic_ptr_type;

  } // end of namespace configuration

} // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
