/// \file mctools/digitization/simple_linear_adc.h
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
 *   Simple linear ADC (Analog Digital Converter).
 *
 * History:
 *
 */

#ifndef MCTOOLS_DIGITIZATION_SIMPLE_LINEAR_ADC_H
#define MCTOOLS_DIGITIZATION_SIMPLE_LINEAR_ADC_H

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <mctools/digitization/i_adc.h>

namespace mctools {

  namespace digitization {

    //! \brief A simple linear ADC (Analog Digital Converter)
    class simple_linear_adc
      : public i_adc,
        public datatools::i_tree_dumpable
    {
    public:

      //! Default constructor
      simple_linear_adc();

      //! Destructor
      virtual ~simple_linear_adc();

      //! Check validity
      bool is_valid() const;

      //! Set the low voltage reference value
      void set_v_ref_low(const double);

      //! Return the low voltage reference value
      double get_v_ref_low() const;

      //! Set the high voltage reference value
      void set_v_ref_high(const double);

      //! Return the high voltage reference value
      double get_v_ref_high() const;

      //! Set the number of encoding bits
      void set_nbits(const uint16_t);

      //! Return the number of encoding bits
      uint16_t get_nbits()const;

      //! Set the signed codes flag
      void set_signed(bool);

      //! Check the signed codes flag
      bool is_signed() const;

      //! Initialization
      void initialize_simple();

      //! Initialization
      void initialize(const datatools::properties & config_);

      //! Reset
      void reset();

      //! Compute the sampled voltage associated to a channel
      double compute_sampled_voltage(int32_t channel_, bool ignore_out_ = false) const;

      //! Quantize
      virtual int32_t quantize(const double vinput_) const;

      //! Return the minimum channel
      int32_t get_min_channel() const;

      //! Return the maximum channel
      int32_t get_max_channel() const;

      //! Smart print
      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

      //! Reflection interface
      DR_CLASS_RTTI()

    protected:

      //! Set default attributes values
      void _set_defaults();

      //! Compute working data
      void _compute_working_data();

    private:

      // Configuration parameters:
      double   _v_ref_low_;         ///< Reference voltage (low value)
      double   _v_ref_high_;        ///< Reference voltage (high value)
      uint16_t _nbits_;             ///< Number of coding bits (resolution)
      bool     _signed_;            ///< Flag to use signed channel codes
      int32_t  _underflow_channel_; ///< Conventional value for underflow channel
      int32_t  _overflow_channel_;  ///< Conventional value for overflow channel
      int32_t  _invalid_channel_;   ///< Conventional value for invalid channel

      // Working data:
      int32_t _min_channel_; ///< Conventional minimum channel
      int32_t _max_channel_; ///< Conventional maximum channel
      uint32_t _number_of_voltage_intervals_; ///< Number of voltage interval
      double _efsr_;  ///< Full scale voltage range (span)
      double _q_;     ///< Voltage resolution
      double _v0_;    ///< Reference quantization voltage

    };

  } // end of namespace digitization

} // end of namespace mctools

// Activate reflection layer :
DR_CLASS_INIT(::mctools::digitization::simple_linear_adc)

#endif // MCTOOLS_DIGITIZATION_SIMPLE_LINEAR_ADC_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
