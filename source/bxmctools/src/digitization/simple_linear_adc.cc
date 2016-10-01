/// \file mctools/digitization/simple_linear_adc.cc
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

    void simple_linear_adc::_set_defaults()
    {
      datatools::invalidate(_v_ref_low_);
      datatools::invalidate(_v_ref_high_);
      _nbits_ = 0;
      _signed_ = false;
      _underflow_channel_ = 0xFFFFFFFF;
      _overflow_channel_  = 0xFFFFFFFE;
      _invalid_channel_   = 0xFFFFFFFD;
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

    bool simple_linear_adc::is_valid() const
    {
      if (!datatools::is_valid(_v_ref_low_)) return false;
      if (!datatools::is_valid(_v_ref_high_)) return false;
      if (_nbits_ < 1) return false;
      if (!datatools::is_valid(_q_)) return false;
      return true;
    }

    void simple_linear_adc::set_v_ref_low(const double v_)
    {
      _v_ref_low_ = v_;
      return;
    }

    double simple_linear_adc::get_v_ref_low() const
    {
      return _v_ref_low_;
    }

    void simple_linear_adc::set_v_ref_high(const double v_)
    {
      _v_ref_high_ = v_;
      return;
    }

    double simple_linear_adc::get_v_ref_high() const
    {
      return _v_ref_high_;
    }

    void simple_linear_adc::set_nbits(const uint16_t nbits_)
    {
      _nbits_ = nbits_;
      return;
    }

    uint16_t simple_linear_adc::get_nbits() const
    {
      return _nbits_;
    }

    void simple_linear_adc::set_signed(bool s_)
    {
      _signed_ = s_;
      return;
    }

    bool simple_linear_adc::is_signed() const
    {
      return _signed_;
    }

    void simple_linear_adc::initialize_simple()
    {
      datatools::properties dummy_cfg;
      initialize(dummy_cfg);
      return;
    }

    void simple_linear_adc::initialize(const datatools::properties & config_)
    {

      if (_nbits_ < 1) {
        if (config_.has_key("nbits")) {
          int nbits = config_.fetch_integer("nbits");
          DT_THROW_IF(nbits < 1 || nbits > 32, std::range_error,
                      "Invalid number of coding bits!");
          set_nbits((uint32_t) nbits);
        }
      }

      if (!datatools::is_valid(_v_ref_low_)) {
        if (config_.has_key("v_ref_low")) {
          double v_ref_low = config_.fetch_real("v_ref_low");
          DT_THROW_IF(!std::isfinite(v_ref_low), std::range_error,
                      "Invalid low voltage reference value!");
          set_v_ref_low(v_ref_low);
        }
      }

      if (!datatools::is_valid(_v_ref_high_)) {
        if (config_.has_key("v_ref_high")) {
          double v_ref_high = config_.fetch_real("v_ref_high");
          DT_THROW_IF(!std::isfinite(v_ref_high), std::range_error,
                      "Invalid high voltage reference value!");
          set_v_ref_high(v_ref_high);
        }
      }

      if (config_.has_key("signed")) {
        bool s = config_.fetch_boolean("signed");
        set_signed(s);
      }

      _compute_working_data();

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
      _underflow_channel_ = _min_channel_ - 1;
      _overflow_channel_ = _max_channel_ + 1;
      _invalid_channel_ = 0xFFFFFFF;
      return;
    }

    void simple_linear_adc::reset()
    {
      _set_defaults();
      return;
    }

    // virtual
    int32_t simple_linear_adc::quantize(const double vinput_) const
    {
      DT_THROW_IF(!std::isfinite(_q_), std::range_error,
                  "ADC is not configured!");
      // std::cerr << "DEVEL: simple_linear_adc::quantize: vinput = " << vinput_ / CLHEP::volt << " V\n";
      // std::cerr << "DEVEL: simple_linear_adc::quantize:   v_ref_low = " << _v_ref_low_ / CLHEP::volt << " V\n";
      // std::cerr << "DEVEL: simple_linear_adc::quantize:   v_ref_high = " << _v_ref_high_ / CLHEP::volt << " V\n";
      double eps = std::numeric_limits<double>::epsilon();
      if (vinput_ < _v_ref_low_ - eps) {
        // std::cerr << "DEVEL: simple_linear_adc::quantize:   !!!underflow!!!\n";
        return _underflow_channel_;
      } else if (vinput_ > _v_ref_high_) {
        // std::cerr << "DEVEL: simple_linear_adc::quantize:   !!!overflow!!!\n";
        return _overflow_channel_;
      }
      int32_t channel = (int32_t) ((vinput_ - _v0_) / _q_);
      // std::cerr << "DEVEL: simple_linear_adc::quantize:   channel = " << channel << "\n";
      return _min_channel_ + channel;
    }

    double simple_linear_adc::compute_sampled_voltage(int32_t channel_, bool ignore_out_) const
    {
      if (channel_ == _underflow_channel_) {
        if (ignore_out_) return -std::numeric_limits<double>::infinity();
        return _v_ref_low_ - _q_;
      }
      if (channel_ == _overflow_channel_) {
        if (ignore_out_) return std::numeric_limits<double>::infinity();
        return _v_ref_high_ + _q_;
      }
      return channel_ * _q_ + _v0_;
    }

    int32_t simple_linear_adc::get_min_channel() const
    {
      return _min_channel_;
    }

    int32_t simple_linear_adc::get_max_channel() const
    {
      return _max_channel_;
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
           << "Validity : " << ( is_valid() ? "<yes>" : "<no>" )
           << std::endl;

      return;
    }

  } // end of namespace digitization

} // end of namespace mctools
