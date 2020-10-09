/// \file mctools/digitization/sampled_signal.h
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
 * Description:
 *
 *   Time sampled signal (obtained from a ADC like sampler).
 *
 */

#ifndef MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_H
#define MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_H

// Standard library:
#include <limits>
#include <vector>
#include <bitset>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

// This project:
// #include <mctools/digitization/i_adc.h>

namespace mctools {

  namespace digitization {

    //! \brief A simple linear ADC (Analog/Digital Converter)
    class sampled_signal
      : public geomtools::base_hit
    {
    public:

      /// Masks to automatically tag the attributes to be stored
      enum more_store_mask_type {
        STORE_SAMPLING_FREQUENCY = datatools::bit_mask::bit03, //!< Serialization mask for the sampling frequency
        STORE_SAMPLES            = datatools::bit_mask::bit04  //!< Serialization mask for the samples
      };

      // Special sample values:
      static const int32_t INVALID_SAMPLE   = std::numeric_limits<int32_t>::min();
      static const int32_t UNDERFLOW_SAMPLE = std::numeric_limits<int32_t>::min() + 1;
      static const int32_t OVERFLOW_SAMPLE  = std::numeric_limits<int32_t>::max();

      /// Check if a sample value is normal (nor invalid, underflow or overflow)
      static bool sample_is_normal(const int32_t);

      /// Check if a sample value is invalid
      static bool sample_is_invalid(const int32_t);

      /// Check if a sample value is underflow
      static bool sample_is_underflow(const int32_t);

      /// Check if a sample value is overflow
      static bool sample_is_overflow(const int32_t);

      /// \brief Indexes of sampling status bits
      enum sampling_status_index {
        SS_INVALID   = 0, ///< Index of the invalid/uninitialized samples bit
        SS_UNDERFLOW = 1, ///< Index of the underflow samples bit
        SS_OVERFLOW  = 2  ///< Index of the overflow samples bit
      };

      //! Default constructor
      sampled_signal();

      //! Constructor
      sampled_signal(const double sampling_frequency_, const std::size_t nsamples_, const int32_t value_ = INVALID_SAMPLE);

      //! Destructor
      ~sampled_signal() override;

      //! Check validity
      bool is_valid() const override;

      //! Check if sampling frequency is set
      bool has_sampling_frequency() const;

      //! Set the sampling frequency
      void set_sampling_frequency(const double);

      //! Return the sampling frequency
      double get_sampling_frequency() const;

      //! Reset sampling frequency and associated period
      void reset_sampling_frequency();

      //! Set the sampling period
      void set_sampling_period(const double);

      //! Return the sampling period
      double get_sampling_period() const;

      //! Return the number of sampling time intervals
      std::size_t get_number_of_sampling_time_intervals() const;

      //! Return the sampling duration
      double get_sampling_duration() const;

      //! Return the minimum sampling time
      double get_min_sampling_time() const;

      //! Return the maximum sampling time
      double get_max_sampling_time() const;

      //! Set the number of samples
      void set_number_of_samples(const std::size_t nsamples_, const int32_t value_ = INVALID_SAMPLE);

      //! Reset the number of samples
      void reset_number_of_samples();

      //! Return the number of samples
      std::size_t get_number_of_samples() const;

      //! Check if the array of samples is initialized
      bool is_sampling_initialized() const;

      //! Check if the array of samples has at least one invalid sample
      bool has_invalid_samples() const;

      //! Check if the array of samples has at least one underflow sample
      bool has_underflow_samples() const;

      //! Check if the array of samples has at least one overflow sample
      bool has_overflow_samples() const;

      //! Check if the array of samples has only valid samples
      bool has_only_valid_samples() const;

      //! Check if the minimum sample value is set
      bool has_min_sample() const;

      //! Return the minimum sample value
      int32_t get_min_sample() const;

      //! Check if the maximum sample value is set
      bool has_max_sample() const;

      //! Return the maximum sample value
      int32_t get_max_sample() const;

      //! Reset
      void reset();

      //! Set the array of samples
      void set_samples(const std::vector<int32_t> &, const bool update_ = true);

      //! Return a const reference to the array of samples
      const std::vector<int32_t> & get_samples() const;

      //! Set the sample value at given time index
      void set_sample(const uint32_t index_, const int32_t sample_, const bool update_ = false);

      //! Return the sample value at given time index
      uint32_t get_sample(const uint32_t index_) const;

      //! Check if a sample at given time index has a normal value
      bool is_normal(const uint32_t index_) const;

      //! Check if a sample at given time index has an invalid value
      bool is_invalid(const uint32_t index_) const;

      //! Check if a sample at given time index has an underflow value
      bool is_underflow(const uint32_t index_) const;

      //! Check if a sample at given time index has an overflow value
      bool is_overflow(const uint32_t index_) const;

      //! Return the time associated to a given sample
      double get_time(const uint32_t index_) const;

      //! Return the sample index associated to a given time
      uint32_t get_index(const double time_) const;

      //! Force the sample value at given time index to be invalid
      void unset_sample(const uint32_t index_, const bool update_ = false);

      //! Force the sample value at given time index to be underflow
      void set_underflow_sample(const uint32_t index_, const bool update_ = false);

      //! Force the sample value at given time index to be overflow
      void set_overflow_sample(const uint32_t index_, const bool update_ = false);

      //! Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

      //! Update internal data
      void update();

      //! \brief Special flags for signal slicing
      enum slicing_bits {
        SLICING_HIT_ID      = datatools::bit_mask::bit00,
        SLICING_GEOM_ID     = datatools::bit_mask::bit01,
        SLICING_AUXILIARIES = datatools::bit_mask::bit02,
        SLICING_UPDATE      = datatools::bit_mask::bit03,
        SLICING_ALL         = SLICING_HIT_ID
        | SLICING_GEOM_ID
        | SLICING_AUXILIARIES
        | SLICING_UPDATE
      };

      //! Extract slice of the signal
      void slice_from_to(sampled_signal & target_,
                         const std::size_t first_,
                         const std::size_t last_,
                         const uint32_t flags_ = 0) const;

      //! \brief Special flags for ASCII dump
      enum print_ascii_bits {
        PRINT_ASCII_NOHEADER = datatools::bit_mask::bit00,
        PRINT_ASCII_TIME_IN_NS = datatools::bit_mask::bit01,
        PRINT_ASCII_NODATABLOCKSEP = datatools::bit_mask::bit02
      };

      //! Simple ASCII terminal dump
      void print_ascii(std::ostream & out_, const uint32_t flags_ = 0) const;

      // //! Simple ASCII terminal plot
      // void plot_ascii(std::ostream & out_, const uint32_t flags_ = 0) const;

    protected:

      //! Set default attributes values
      void _set_defaults();

      //! Set default attributes values
      void _set_default_working_data();

      //! Recompute working data
      void _recompute_working_data();

    private:

      double _sampling_frequency_;    ///< Sampling frequency/rate
      std::vector<int32_t> _samples_; ///< Samples

      // Working data:
      std::bitset<8> _sampling_status_;      ///< Special sampling status flags
      int32_t _min_sample_ = INVALID_SAMPLE; ///< Minimum value of stored samples
      int32_t _max_sample_ = INVALID_SAMPLE; ///< Maximum value of stored samples

      DATATOOLS_SERIALIZATION_DECLARATION()

      //! Reflection interface
      DR_CLASS_RTTI()

    };

  } // end of namespace digitization

} // end of namespace mctools

// Activate reflection layer :
DR_CLASS_INIT(::mctools::digitization::sampled_signal)

#endif // MCTOOLS_DIGITIZATION_SAMPLED_SIGNAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
