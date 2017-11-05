/// \file datatools/configuration/i_occurrence.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-08-10
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Some utilities for the description of configuration variants and parameters.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_OCCURRENCE_FACTORY_H
#define DATATOOLS_CONFIGURATION_OCCURRENCE_FACTORY_H

// Standard library:
#include <string>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class i_occurrence;

    /// \brief Occurrence factory
    class occurrence_factory
    {
    public:

      /// Create a new occurrence object
      /** Supported format is:
       *
       *  "single" : create a single_occurrence object
       *
       *  "array size=10 start_id=0" : create an array_occurrence object
       *
       */
      static i_occurrence * create(const std::string & def_);

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_OCCURRENCE_FACTORY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
