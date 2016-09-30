// triangle_gate_signal_shape.cc
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

// Ourselves:
#include <mctools/signal/triangle_gate_signal_shape.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/triangle_function.h>

namespace mctools {

  namespace signal {

    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(triangle_gate_signal_shape,
                                               "mctools::signal::triangle_gate_signal_shape")

    DATATOOLS_CLONEABLE_IMPLEMENTATION(triangle_gate_signal_shape)


    void triangle_gate_signal_shape::_set_defaults()
    {
      _polarity_ = POLARITY_INVALID;
      datatools::invalidate(_amplitude_);
      datatools::invalidate(_t0_);
      datatools::invalidate(_t1_);
      datatools::invalidate(_t2_);
      datatools::invalidate(_t3_);
      datatools::invalidate(_q_);
      return;
    }

    triangle_gate_signal_shape::triangle_gate_signal_shape()
    {
      _set_defaults();
      return;
    }

    triangle_gate_signal_shape::~triangle_gate_signal_shape()
    {
      return;
    }

    void triangle_gate_signal_shape::reset_polarity()
    {
      _polarity_ = POLARITY_INVALID;
      return;
    }

    void triangle_gate_signal_shape::set_polarity(polarity_type p_)
    {
      DT_THROW_IF(p_ == POLARITY_UNKNOWN, std::logic_error, "Invalid polarity value!");
      _polarity_ = p_;
      return;
    }

    polarity_type triangle_gate_signal_shape::get_polarity() const
    {
      return _polarity_;
    }

    double triangle_gate_signal_shape::get_rise_time() const
    {
      return (_t1_ - _t0_) * 0.8;
    }

    double triangle_gate_signal_shape::get_fall_time() const
    {
      return (_t3_ - _t2_) * 0.8;
    }

    double triangle_gate_signal_shape::get_q() const
    {
      return _q_;
    }

    void triangle_gate_signal_shape::set_t0(double t0_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      _t0_ = t0_;
      return;
    }

    double triangle_gate_signal_shape::get_t0() const
    {
      return _t0_;
    }

    void triangle_gate_signal_shape::set_t1(double t1_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      _t1_ = t1_;
      return;
    }

    double triangle_gate_signal_shape::get_t1() const
    {
      return _t1_;
    }

    void triangle_gate_signal_shape::set_t2(double t2_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      _t2_ = t2_;
      return;
    }

    double triangle_gate_signal_shape::get_t2() const
    {
      return _t2_;
    }

    void triangle_gate_signal_shape::set_t3(double t3_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      _t3_ = t3_;
      return;
    }

    double triangle_gate_signal_shape::get_t3() const
    {
      return _t3_;
    }

    void triangle_gate_signal_shape::set_amplitude(double amplitude_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      DT_THROW_IF(datatools::is_valid(amplitude_)
                  && amplitude_ < 0.0,
                  std::logic_error,
                  "Invalid negative amplitude!");
      _amplitude_ = amplitude_;
      return;
    }

    double triangle_gate_signal_shape::get_amplitude() const
    {
      return _amplitude_;
    }

    double triangle_gate_signal_shape::get_non_zero_domain_min() const
    {
      return _t0_;
    }

    double triangle_gate_signal_shape::get_non_zero_domain_max() const
    {
      return _t3_;
    }

    double triangle_gate_signal_shape::get_tpeak() const
    {
      return _t1_;
    }

    double triangle_gate_signal_shape::get_duration() const
    {
      return _t3_ - _t0_;
    }

    bool triangle_gate_signal_shape::is_valid() const
    {
      if (!polarity_is_set(_polarity_)) return false;
      if (!datatools::is_valid(_t0_)) return false;
      if (!datatools::is_valid(_t1_)) return false;
      if (!datatools::is_valid(_t2_)) return false;
      if (!datatools::is_valid(_t3_)) return false;
      if (!datatools::is_valid(_amplitude_)) return false;
      return true;
    }

    void triangle_gate_signal_shape::reset()
    {
      reset_polarity();
      _set_defaults();
      this->i_unary_function::_base_reset();
      return;
    }

    bool triangle_gate_signal_shape::is_initialized() const
    {
      return this->i_unary_function::is_initialized()
        && is_valid();
    }

    void triangle_gate_signal_shape::initialize(const datatools::properties & config_,
                                                mygsl::unary_function_dict_type & functors_)
    {
      this->mygsl::i_unary_function::_base_initialize(config_, functors_);

      static const double the_default_time_unit = mctools::signal::default_time_unit().get_value();
      // static const double the_default_charge_unit = mctools::signal::default_charge_unit().get_value();
      static const double the_default_voltage_unit = mctools::signal::default_voltage_unit().get_value();

      if (!polarity_is_set(_polarity_)) {
        if (config_.has_key("polarity")) {
          const std::string & pol_label =  config_.fetch_string("polarity");
          if (pol_label == "+") {
            set_polarity(POLARITY_POSITIVE);
          } else if (pol_label == "-") {
            set_polarity(POLARITY_NEGATIVE);
          } else {
            DT_THROW(std::logic_error, "Invalid polarity label '" << pol_label << "'!");
          }
        }
      }

      if (!datatools::is_valid(_t0_)) {
        if (config_.has_key("t0")) {
          double t0 = config_.fetch_real("t0");
          if (!config_.has_explicit_unit("t0")) {
            t0 *= the_default_time_unit;
          }
          set_t0(t0);
        }
      }

      if (!datatools::is_valid(_t1_)) {
        if (config_.has_key("t1")) {
          double t1 = config_.fetch_real("t1");
          if (!config_.has_explicit_unit("t1")) {
            t1 *= the_default_time_unit;
          }
          set_t1(t1);
        }
      }

      if (!datatools::is_valid(_t2_)) {
        if (config_.has_key("t2")) {
          double t2 = config_.fetch_real("t2");
          if (!config_.has_explicit_unit("t2")) {
            t2 *= the_default_time_unit;
          }
          set_t2(t2);
        }
      }

      if (!datatools::is_valid(_t3_)) {
        if (config_.has_key("t3")) {
          double t3 = config_.fetch_real("t3");
          if (!config_.has_explicit_unit("t3")) {
            t3 *= the_default_time_unit;
          }
          set_t3(t3);
        }
      }

      if (!datatools::is_valid(_amplitude_)) {
        if (config_.has_key("amplitude")) {
          double amplitude = config_.fetch_real("amplitude");
          if (!config_.has_explicit_unit("amplitude")) {
            amplitude *= the_default_voltage_unit;
          }
          set_amplitude(amplitude);
        }
      }

      DT_THROW_IF(!polarity_is_set(_polarity_), std::logic_error, "polarity is not set!");
      DT_THROW_IF(!datatools::is_valid(_amplitude_), std::logic_error, "Amplitude is not set!");
      DT_THROW_IF(!datatools::is_valid(_t0_), std::logic_error, "Start time is not set!");
      DT_THROW_IF(!datatools::is_valid(_t1_), std::logic_error, "Plateau start time is not set!");
      DT_THROW_IF(_t1_ <= _t0_ + get_epsilon(), std::logic_error, "Invalid plateau start time!");
      DT_THROW_IF(!datatools::is_valid(_t2_), std::logic_error, "Plateau stop time is not set!");
      DT_THROW_IF(_t2_ <= _t1_ + get_epsilon(), std::logic_error, "Invalid plateau stop time!");
      DT_THROW_IF(!datatools::is_valid(_t3_), std::logic_error, "Stop time is not set!");
      DT_THROW_IF(_t3_ <= _t2_ + get_epsilon(), std::logic_error, "Invalid stop time!");

      _compute_parameters();

      return;
    }

    void triangle_gate_signal_shape::_compute_parameters()
    {
      _q_ = _amplitude_ * (0.5 * ((_t1_ - _t0_) + (_t3_ - _t2_)) + (_t2_ - _t1_));
      _a_rise_ = _amplitude_ / (_t1_ - _t0_);
      _b_rise_ = - _a_rise_ * _t0_;
      _a_fall_ = _amplitude_ / (_t2_ - _t3_);
      _b_fall_ = - _a_fall_ * _t3_;
      return;
    }

    double triangle_gate_signal_shape::_eval(double t_) const
    {
      double res = 0.0;
      if (t_ > _t0_ && t_ < _t3_) {
        if (t_ < _t1_) res = _a_rise_ * t_ + _b_rise_;
        else if (t_ < _t2_) res = _amplitude_;
        else res = _a_fall_ * t_ + _b_fall_;
      }
      if (_polarity_ == POLARITY_NEGATIVE) {
        res *= -1.0;
      }
      return res;
    }

    void triangle_gate_signal_shape::tree_dump(std::ostream & out_,
                                               const std::string & title_,
                                               const std::string & indent_,
                                               bool inherit_) const
    {
      this->i_unary_function::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Polarity: " << _polarity_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t0 : " << _t0_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t1 : " << _t1_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t2 : " << _t2_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t3 : " << _t3_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current amplitude : " << _amplitude_ / (CLHEP::volt) << " V" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value q : " << _q_ / (CLHEP::volt * CLHEP::ns) << " nV.s" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Rise time (10-90%): " << get_rise_time() / CLHEP::ns << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Fall time (10-90%): " << get_fall_time() / CLHEP::ns << " ns" << std::endl;

      return;
    }

  } // end of namespace signal

} // end of namespace mctools
