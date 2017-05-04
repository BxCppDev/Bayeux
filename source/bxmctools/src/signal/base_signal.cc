// base_signal.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2015-04-10
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
 */

// Ourselves:
#include <mctools/signal/base_signal.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>

namespace mctools {

  namespace signal {

    base_signal::base_signal()
    {
      return;
    }

    base_signal::~base_signal()
    {
      return;
    }

    void base_signal::reset()
    {
      reset_shape();
      return;
    }

    bool base_signal::has_shape() const
    {
      return _shape_.has_data();
    }

    const mygsl::i_unary_function & base_signal::get_shape() const
    {
      DT_THROW_IF(!has_shape(), std::logic_error, "No available embedded signal shape!");
      return _shape_.get();
    }

    void base_signal::reset_shape()
    {
      _shape_.reset();
      return;
    }

    void base_signal::set_shape(const datatools::handle<mygsl::i_unary_function> & hshape_)
    {
      _shape_ = hshape_;
      return;
    }

  } // end of namespace signal

} // end of namespace mctools
