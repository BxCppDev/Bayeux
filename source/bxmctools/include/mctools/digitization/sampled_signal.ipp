/// \file mctools/signal/sampled_signal.ipp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-02
 * Last modified : 2016-10-02
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

#ifndef MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_IPP
#define MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_IPP

// Ourselves:
#include <mctools/digitization/sampled_signal.h>

// Third party:
// - Boost :
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace mctools {

  namespace digitization {

    template<class Archive>
    void sampled_signal::serialize(Archive & ar_,
                                   const unsigned int /* version */)
    {
      // inherit from the 'geomtools::base_hit' mother class:
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));

      if (_store & STORE_SAMPLING_FREQUENCY) {
        ar_ & boost::serialization::make_nvp("sampling_frequency", _sampling_frequency_);
      }
      if (_store & STORE_SAMPLES) {
        ar_ & boost::serialization::make_nvp("samples", _samples_);
      }

      if (Archive::is_loading::value) {
        // Working data are never stored in archives but are recomputed ar reload:
        update();
      }

      return;
    }

  } // end of namespace digitization

} // end of namespace mctools

#endif // MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
