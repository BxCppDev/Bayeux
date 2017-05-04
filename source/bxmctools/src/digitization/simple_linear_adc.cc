/// \file mctools/digitization/simple_linear_adc.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-01
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
#include <mctools/digitization/simple_linear_adc.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace mctools {

  namespace digitization {

    // static
    const int32_t simple_linear_adc::INVALID_CHANNEL;

    void simple_linear_adc::_set_defaults()
    {
      datatools::invalidate(_v_ref_low_);
      datatools::invalidate(_v_ref_high_);
      _nbits_ = 0;
      _signed_ = false;
      _no_underflow_ = false;
      _no_overflow_ = false;
      _underflow_channel_ = INVALID_CHANNEL;
      _overflow_channel_  = INVALID_CHANNEL;
      _number_of_voltage_intervals_ = 0;
      datatools::invalidate(_q_);
      datatools::invalidate(_v0_);
      return;
    }

    simple_linear_adc::simple_linear_adc()
    {
      _set_defaults();
      return;
    }

    simple_linear_adc::~simple_linear_adc()
    {
      return;
    }

    void simple_linear_adc::set_v_ref_low(const double v_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _v_ref_low_ = v_;
      return;
    }

    double simple_linear_adc::get_v_ref_low() const
    {
      return _v_ref_low_;
    }

    void simple_linear_adc::set_v_ref_high(const double v_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _v_ref_high_ = v_;
      return;
    }

    double simple_linear_adc::get_v_ref_high() const
    {
      return _v_ref_high_;
    }

    void simple_linear_adc::set_nbits(const uint16_t nbits_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      DT_THROW_IF(nbits_ > 24, std::range_error, "Number of bits is too large!");
      _nbits_ = nbits_;
      return;
    }

    uint16_t simple_linear_adc::get_nbits() const
    {
      return _nbits_;
    }

    void simple_linear_adc::set_signed(bool s_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _signed_ = s_;
      return;
    }

    bool simple_linear_adc::is_signed() const
    {
      return _signed_;
    }

    void simple_linear_adc::set_no_underflow(bool n_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _no_underflow_ = n_;
      return;
    }

    bool simple_linear_adc::is_no_underflow() const
    {
      return _no_underflow_;
    }

    void simple_linear_adc::set_no_overflow(bool n_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _no_overflow_ = n_;
      return;
    }

    bool simple_linear_adc::is_no_overflow() const
    {
      return _no_overflow_;
    }

    void simple_linear_adc::set_underflow_channel(int32_t ch_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _underflow_channel_ = ch_;
      return;
    }

    int32_t simple_linear_adc::get_underflow_channel() const
    {
      return _underflow_channel_;
    }

    void simple_linear_adc::set_overflow_channel(int32_t ch_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized and locked!");
      _overflow_channel_ = ch_;
      return;
    }

    int32_t simple_linear_adc::get_overflow_channel() const
    {
      return _overflow_channel_;
    }

    void simple_linear_adc::initialize_simple()
    {
      datatools::properties dummy_cfg;
      initialize(dummy_cfg);
      return;
    }

    bool simple_linear_adc::is_initialized() const
    {
      return _initialized_;
    }

    void simple_linear_adc::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "ADC is already initialized!");

      if (_nbits_ < 1) {
        if (config_.has_key("nbits")) {
          int nbits = config_.fetch_integer("nbits");
          DT_THROW_IF(nbits < 1 || nbits > 32, std::range_error,
                      "Invalid number of coding bits!");
          set_nbits((uint32_t) nbits);
        }
      }
      DT_THROW_IF(_nbits_ == 0, std::range_error, "Number of coding bits is not set!");

      if (!datatools::is_valid(_v_ref_low_)) {
        if (config_.has_key("v_ref_low")) {
          double v_ref_low = config_.fetch_real("v_ref_low");
          DT_THROW_IF(!std::isfinite(v_ref_low), std::range_error,
                      "Invalid low voltage reference value!");
          set_v_ref_low(v_ref_low);
        }
      }
      DT_THROW_IF(!datatools::is_valid(_v_ref_low_), std::logic_error,
                  "Low voltage reference value is not set!");

      if (!datatools::is_valid(_v_ref_high_)) {
        if (config_.has_key("v_ref_high")) {
          double v_ref_high = config_.fetch_real("v_ref_high");
          DT_THROW_IF(!std::isfinite(v_ref_high), std::range_error,
                      "Invalid high voltage reference value!");
          set_v_ref_high(v_ref_high);
        }
      }
      DT_THROW_IF(!datatools::is_valid(_v_ref_high_), std::logic_error,
                  "High voltage reference value is not set!");
      DT_THROW_IF(_v_ref_high_ <= _v_ref_low_, std::range_error,
                  "Invalid low/high voltage reference value range!");

      if (config_.has_key("signed")) {
        bool s = config_.fetch_boolean("signed");
        set_signed(s);
      }

      if (config_.has_key("no_underflow")) {
        bool n = config_.fetch_boolean("no_underflow");
        set_no_underflow(n);
      }

      if (config_.has_key("no_overflow")) {
        bool n = config_.fetch_boolean("no_overflow");
        set_no_overflow(n);
      }

      if (!is_no_underflow()) {
        if (_underflow_channel_ == INVALID_CHANNEL) {
          if (config_.has_key("underflow_channel")) {
            int ch = config_.fetch_integer("underflow_channel");
            set_underflow_channel(ch);
          }
        }
      }

      if (!is_no_overflow()) {
        if (_overflow_channel_ == INVALID_CHANNEL) {
          if (config_.has_key("overflow_channel")) {
            int ch = config_.fetch_integer("overflow_channel");
            set_overflow_channel(ch);
          }
        }
      }

      _compute_working_data();

      _initialized_ = true;
      return;
    }

    void simple_linear_adc::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "ADC is not initialized!");
      _initialized_ = false;
      _set_defaults();
      return;
    }

    void simple_linear_adc::_compute_working_data()
    {
      _efsr_ = _v_ref_high_ - _v_ref_low_;
      _number_of_voltage_intervals_ = (1 << _nbits_) - 1;
      _q_ = _efsr_ / _number_of_voltage_intervals_;
      _v0_ = _v_ref_low_ - 0.5 * _q_;
      _min_channel_ = 0;
      _max_channel_ = (1 << _nbits_) - 1;
      if (_signed_) {
        _min_channel_ = - (_max_channel_ + 1) / 2;
        _max_channel_ = - _min_channel_ -1;
      }
      if (_no_underflow_) {
        _underflow_channel_ = _min_channel_;
      } else {
        if (_underflow_channel_ == INVALID_CHANNEL) {
          _underflow_channel_ = _min_channel_ - 1;
        } else {
          DT_THROW_IF((_underflow_channel_ >= _min_channel_) && (_underflow_channel_ <= _max_channel_),
                      std::range_error,
                      "Underflow channel [" << _underflow_channel_ << "] lies in the coding range ["
                      << _min_channel_ << ":" << _max_channel_ << "]!");
        }
      }
      if (_no_overflow_) {
        _overflow_channel_ = _max_channel_;
      } else {
        if (_overflow_channel_ == INVALID_CHANNEL) {
          _overflow_channel_ = _max_channel_ + 1;
        } else {
          DT_THROW_IF((_overflow_channel_ >= _min_channel_) && (_overflow_channel_ <= _max_channel_),
                      std::range_error,
                      "Overflow channel [" << _overflow_channel_ << "] lies in the coding range ["
                      << _min_channel_ << ":" << _max_channel_ << "]!");
        }
      }
      return;
    }

    // virtual
    int32_t simple_linear_adc::quantize(const double vinput_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "ADC is not initialized!");
      double eps = std::numeric_limits<double>::epsilon();
      if (vinput_ < _v_ref_low_ - eps) {
        return _underflow_channel_;
      } else if (vinput_ > _v_ref_high_) {
        return _overflow_channel_;
      }
      int32_t channel = (int32_t) ((vinput_ - _v0_) / _q_);
      return _min_channel_ + channel;
    }

    double simple_linear_adc::compute_sampled_voltage(int32_t channel_,
                                                      bool ignore_out_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "ADC is not initialized!");
      if (!is_no_underflow()) {
        if (channel_ == _underflow_channel_) {
          if (ignore_out_) return -std::numeric_limits<double>::infinity();
          return _v_ref_low_ - _q_;
        }
      }
      if (!is_no_overflow()) {
        if (channel_ == _overflow_channel_) {
          if (ignore_out_) return std::numeric_limits<double>::infinity();
          return _v_ref_high_ + _q_;
        }
      }
      return channel_ * _q_ + _v_ref_low_;
    }

    int32_t simple_linear_adc::get_min_channel() const
    {
      return _min_channel_;
    }

    int32_t simple_linear_adc::get_max_channel() const
    {
      return _max_channel_;
    }

    uint32_t simple_linear_adc::get_number_of_voltage_intervals() const
    {
      return _number_of_voltage_intervals_;
    }

    double simple_linear_adc::get_voltage_resolution() const
    {
      return _q_;
    }

    void simple_linear_adc::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Vref (low) : " << _v_ref_low_ / CLHEP::volt << " V"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Vref (high) : " << _v_ref_high_ / CLHEP::volt << " V"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Resolution : " << _nbits_ << " bits"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Signed : " << _signed_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "No underflow : " << _no_underflow_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "No overflow : " << _no_overflow_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Underflow : " << _underflow_channel_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Overflow : " << _overflow_channel_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Min code : " << _min_channel_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Max code : " << _max_channel_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "# voltage intervals : " << _number_of_voltage_intervals_
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "# voltage resolution : " << _q_ / CLHEP::volt << " V"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Reference quant. voltage : " << _v0_ / CLHEP::volt << " V"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << ( is_initialized() ? "<yes>" : "<no>" )
           << std::endl;

      return;
    }

  } // end of namespace digitization

} // end of namespace mctools
