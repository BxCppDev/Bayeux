//! \file  datatools/ui/validators.h
//! \brief validators for command line
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_VALIDATORS_H
#define DATATOOLS_UI_VALIDATORS_H

// Standard library:
#include <string>
#include <vector>

// Third party:
#include <boost/any.hpp>

// This project:
#include <datatools/ui/wrappers.h>

namespace datatools {

  namespace ui {

    /** Validate the syntax for a boolean value
     *
     * Supported syntax:
     * \code
     * true 1 t T false 0 f F
     * \endcode
     */
    void validate(boost::any & v_,
                  const std::vector<std::string> & values_,
                  datatools::ui::wrapped_boolean *, int);

    /** Validate the syntax for an integer value
     *
     * Supported syntax:
     * \code
     * ... -2 -1 0 1 2 42 ...
     * \endcode
     */
    void validate(boost::any & v_,
                  const std::vector<std::string> & values_,
                  datatools::ui::wrapped_integer *, int);

    /** Validate the syntax for a dimensionless real value
     *
     * Supported syntax:
     * \code
     * -inf -infinity
     * inf +inf +infinity
     * nan NaN
     * ... -1.3 0 0.0 1.24 1.2e34 ...
     * \endcode
     */
    void validate(boost::any & v_,
                  const std::vector<std::string> & values_,
                  datatools::ui::wrapped_real_without_unit *, int);

    /** Validate the syntax for a real value with associated unit
     *
     * Supported syntax:
     * \code
     * -inf -infinity
     * inf +inf +infinity
     * nan NaN
     * "-1.2e34 mm"
     *  1.2_mm
     *  1.2~mm
     * "1.2 mm"
     * \endcode
     */
    void validate(boost::any & v_,
                  const std::vector<std::string> & values_,
                  datatools::ui::wrapped_real_with_unit *, int);

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_VALIDATORS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
