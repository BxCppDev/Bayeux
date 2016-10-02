/// \file mctools/digitization/sampled_signal.cc
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

// Ourselves:
#include <mctools/digitization/sampled_signal.h>

// Standard library:
#include <limits>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace mctools {

  namespace digitization {

    // Serialization implementation :
    DATATOOLS_SERIALIZATION_IMPLEMENTATION(sampled_signal, "mctools::digitization::sampled_signal")

    // static
    const int32_t sampled_signal::INVALID_SAMPLE;
    const int32_t sampled_signal::UNDERFLOW_SAMPLE;
    const int32_t sampled_signal::OVERFLOW_SAMPLE;

    // static
    bool sampled_signal::sample_is_normal(int32_t value_)
    {
      if (value_ == INVALID_SAMPLE) return false;
      if (value_ == UNDERFLOW_SAMPLE) return false;
      if (value_ == OVERFLOW_SAMPLE) return false;
      return true;
    }

    // static
    bool sampled_signal::sample_is_invalid(int32_t value_)
    {
      return value_ == INVALID_SAMPLE;
    }

    // static
    bool sampled_signal::sample_is_underflow(int32_t value_)
    {
      return value_ == UNDERFLOW_SAMPLE;
    }

    // static
    bool sampled_signal::sample_is_overflow(int32_t value_)
    {
      return value_ == OVERFLOW_SAMPLE;
    }

    void sampled_signal::_set_default_working_data()
    {
      _sampling_status_ = 0;
      _min_sample_ = INVALID_SAMPLE;
      _max_sample_ = INVALID_SAMPLE;
      return;
    }

    void sampled_signal::_set_defaults()
    {
      datatools::invalidate(_sampling_frequency_);
      _set_default_working_data();
      return;
    }

    sampled_signal::sampled_signal()
    {
      _set_defaults();
      return;
    }

    sampled_signal::~sampled_signal()
    {
      return;
    }

    bool sampled_signal::is_valid() const
    {
      if (!this->geomtools::base_hit::is_valid()) {
        return false;
      }
      if (!datatools::is_valid(_sampling_frequency_)) return false;
      if (!is_sampling_initialized()) return false;
      return true;
    }

    bool sampled_signal::has_sampling_frequency() const
    {
      return datatools::is_valid(_sampling_frequency_);
    }

    void sampled_signal::set_sampling_frequency(const double fs_)
    {
      DT_THROW_IF(!std::isfinite(fs_) || fs_ <= std::numeric_limits<double>::epsilon(),
                  std::domain_error, "Invalid sampling frequency!");
      _sampling_frequency_ = fs_;
      return;
    }

    void sampled_signal::reset_sampling_frequency()
    {
      datatools::invalidate(_sampling_frequency_);
      return;
    }

    double sampled_signal::get_sampling_frequency() const
    {
      return _sampling_frequency_;
    }

    void sampled_signal::set_sampling_period(const double ts_)
    {
      set_sampling_frequency(1.0 / ts_);
      return;
    }

    double sampled_signal::get_sampling_period() const
    {
      return 1.0 / _sampling_frequency_;
    }

    void sampled_signal::set_number_of_samples(const std::size_t ns_)
    {
      reset_number_of_samples();
      _samples_.assign(ns_, INVALID_SAMPLE);
      return;
    }

    void sampled_signal::reset_number_of_samples()
    {
      _set_default_working_data();
      _samples_.clear();
      return;
    }

    std::size_t sampled_signal::get_number_of_samples() const
    {
      return _samples_.size();
    }

    bool sampled_signal::is_sampling_initialized() const
    {
      return _samples_.size() > 0;
    }

    bool sampled_signal::has_invalid_samples() const
    {
      return _sampling_status_.test(SS_INVALID);
    }

    bool sampled_signal::has_underflow_samples() const
    {
      return _sampling_status_.test(SS_UNDERFLOW);
    }

    bool sampled_signal::has_overflow_samples() const
    {
      return _sampling_status_.test(SS_OVERFLOW);
    }

    bool sampled_signal::has_only_valid_samples() const
    {
      if (!is_sampling_initialized()) return false;
      if (has_invalid_samples()) return false;
      if (has_underflow_samples()) return false;
      if (has_overflow_samples()) return false;
      return true;
    }

    bool sampled_signal::has_min_sample() const
    {
      return _min_sample_ != INVALID_SAMPLE;
    }

    int32_t sampled_signal::get_min_sample() const
    {
      return _min_sample_;
    }

    bool sampled_signal::has_max_sample() const
    {
      return _max_sample_ != INVALID_SAMPLE;
    }

    int32_t sampled_signal::get_max_sample() const
    {
      return _max_sample_;
    }

    void sampled_signal::reset()
    {
      reset_number_of_samples();
      reset_sampling_frequency();
      _set_defaults();
      return;
    }

    void sampled_signal::set_samples(const std::vector<int32_t> & s_, bool update_)
    {
      _samples_ = s_;
      if (update_) {
        update();
      }
      return;
    }

    const std::vector<int32_t> & sampled_signal::get_samples() const
    {
      return _samples_;
    }

    void sampled_signal::update()
    {
      _recompute_working_data();
      return;
    }

    void sampled_signal::unset_sample(uint32_t index_, bool update_)
    {
      set_sample(index_, INVALID_SAMPLE, update_);
      return;
    }

    void sampled_signal::set_underflow_sample(uint32_t index_, bool update_)
    {
      set_sample(index_, UNDERFLOW_SAMPLE, update_);
      return;
    }

    void sampled_signal::set_overflow_sample(uint32_t index_, bool update_)
    {
      set_sample(index_, OVERFLOW_SAMPLE, update_);
      return;
    }

    uint32_t sampled_signal::get_sample(uint32_t index_) const
    {
      DT_THROW_IF(!is_sampling_initialized(), std::logic_error,
                  "Array of samples is not initialized!");
      DT_THROW_IF(index_ < 0 || index_ >= _samples_.size(),
                  std::range_error,
                  "Sample index [" << index_ << "] is out of range!");
      return _samples_[index_];
    }

    void sampled_signal::set_sample(uint32_t index_, int32_t sample_, bool update_)
    {
      DT_THROW_IF(!is_sampling_initialized(), std::logic_error,
                  "Array of samples is not initialized!");
      DT_THROW_IF(index_ < 0 || index_ >= _samples_.size(),
                  std::range_error,
                  "Sample index [" << index_ << "] is out of range!");
      _samples_[index_] = sample_;
      if (update_) {
        _recompute_working_data();
      }
      return;
    }

    void sampled_signal::_recompute_working_data()
    {
      _set_default_working_data();
      for (std::size_t isample = 0;
           isample < get_number_of_samples();
           isample++) {
        int32_t sample = _samples_[isample];
        if (sample == INVALID_SAMPLE) {
          // At least one invalid sample exists:
          _sampling_status_.set(SS_INVALID, true);
        } else if (sample == UNDERFLOW_SAMPLE) {
          // At least one underflow sample exists:
          _sampling_status_.set(SS_UNDERFLOW, true);
        } else if (sample == OVERFLOW_SAMPLE) {
          // At least one overflow sample exists:
          _sampling_status_.set(SS_OVERFLOW, true);
        } else {
          // Compute min sample value:
          if (has_min_sample()) {
            if (sample < get_min_sample()) {
              _min_sample_ = sample;
            }
          } else {
            _min_sample_ = sample;
          }
          // Compute max sample value:
          if (has_max_sample()) {
            if (sample > get_max_sample()) {
              _max_sample_ = sample;
            }
          } else {
            _max_sample_ = sample;
          }
        }
      }
      return;
    }

    void sampled_signal::slice_from_to(sampled_signal & target_,
                                       const std::size_t first_,
                                       const std::size_t last_,
                                       const uint32_t flags_) const
    {
      if (flags_ & SLICING_HIT_ID) {
        if (has_hit_id()) {
          target_.set_hit_id(get_hit_id());
        }
      }
      if (flags_ & SLICING_GEOM_ID) {
        if (has_geom_id()) {
          target_.set_geom_id(get_geom_id());
        }
      }
      if (flags_ & SLICING_AUXILIARIES) {
        target_.set_auxiliaries(get_auxiliaries());
      }
      target_.reset_sampling_frequency();
      target_.reset_number_of_samples();
      if (has_sampling_frequency()) {
        target_.set_sampling_frequency(get_sampling_frequency());
        target_.set_number_of_samples(last_ - first_ + 1);
        std::copy(_samples_.begin() + first_,
                  _samples_.begin() + (last_ + 1),
                  target_._samples_.begin());
      }
      if (flags_ & SLICING_UPDATE) {
        target_.update();
      }
      return;
    }

    void sampled_signal::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
    {
      this->geomtools::base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Sampling frequency : " << _sampling_frequency_ / CLHEP::megahertz << " MHz"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Sampling period : " << get_sampling_period() / CLHEP::nanosecond << " ns"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Number of samples : [" << get_number_of_samples() << "]"
           << std::endl;

      if (has_min_sample()) {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Min. sample : [" << _min_sample_ << "]"
             << std::endl;
      }

      if (has_max_sample()) {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Max. sample : [" << _max_sample_ << "]"
             << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Sampling status : " << _sampling_status_.to_string()
           << std::endl;

      return;
    }

  } // end of namespace digitization

} // end of namespace mctools
