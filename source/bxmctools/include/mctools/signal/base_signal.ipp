/// \file mctools/signal/base_signal.ipp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-27
 * Last modified : 2016-09-27
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
 */

#ifndef MCTOOLS_SIGNAL_BASE_SIGNAL_IPP
#define MCTOOLS_SIGNAL_BASE_SIGNAL_IPP

// Ourselves:
#include <mctools/signal/base_signal.h>

namespace mctools {

  namespace signal {

    template<class Archive>
    void base_signal::serialize(Archive & ar_,
                                const unsigned int /* version */)
    {
      // inherit from the 'geomtools::base_hit' mother class:
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));
      ar_ & boost::serialization::make_nvp("initialized", _initialized_);
      if (_initialized_) {
        ar_ & boost::serialization::make_nvp("category", _category_);
        ar_ & boost::serialization::make_nvp("time_ref", _time_ref_);
        ar_ & boost::serialization::make_nvp("shape_type_id", _shape_type_id_);
        ar_ & boost::serialization::make_nvp("private_shapes_config", _private_shapes_config_);
      }

      return;
    }

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_BASE_SIGNAL_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
