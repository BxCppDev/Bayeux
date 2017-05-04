/// \file mctools/digitization/i_adc.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-01
 * Last modified : 2016-10-01
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
 *   Interface class for ADC (Analog Digital Converter).
 *
 * History:
 *
 */

#ifndef MCTOOLS_DIGITIZATION_I_ADC_H
#define MCTOOLS_DIGITIZATION_I_ADC_H

// This project:

namespace mctools {

  namespace digitization {

    //! \brief A simple linear ADC (Analog Digital Converter).
    class i_adc
    {
    public:

      //! Default constructor
      i_adc(){}

      //! Destructor
      virtual ~i_adc(){}

      //! Quantize
      virtual int32_t quantize(const double vinput_) const = 0;

      //! Reflection interface
      DR_CLASS_RTTI()

    };

  } // end of namespace digitization

} // end of namespace mctools

// Activate reflection layer :
DR_CLASS_INIT(::mctools::digitization::i_adc)

#endif // MCTOOLS_DIGITIZATION_I_ADC_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
