/// \file mctools/signal/i_signal.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-01
 * Last modified : 2015-04-01
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   An abstract base class representing the signal of a hit detector.
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIGNAL_BASE_SIGNAL_H
#define MCTOOLS_SIGNAL_BASE_SIGNAL_H

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>

// This project:
#include <geomtools/base_hit.h>

namespace mygsl {
  // Forward class declaration:
  class i_unary_function;
}

namespace mctools {

  namespace signal {

    //! \brief Abstract base class representing the signal of a hit detector.
    class base_signal : public geomtools::base_hit
    {
    public:

      //! Default constructor
      base_signal();

      //! Destructor
      virtual ~base_signal();

      //! Check if a shape is available
      bool has_shape() const;

      //! Return a const reference to the embedded signal shape
      const mygsl::i_unary_function & get_shape() const;

      //! Discard the embedded signal shape object
      void reset_shape();

      //! Set the embedded signal shape object
      void set_shape(const datatools::handle<mygsl::i_unary_function> &);

      //! Reset the signal
      void reset();

    protected:

      datatools::handle<mygsl::i_unary_function> _shape_; //!< Handle to an embedded signal shape object

    };

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_BASE_SIGNAL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
