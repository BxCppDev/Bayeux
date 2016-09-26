// triangle_signal_shape.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2016-09-27
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
#include <mctools/signal/triangle_signal_shape.h>

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

    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(triangle_signal_shape,
                                               "mctools::signal::triangle_signal_shape")

    void triangle_signal_shape::_set_defaults()
    {
      _polarity_ = POLARITY_INVALID;
      datatools::invalidate(_rise_time_);
      datatools::invalidate(_fall_time_);
      datatools::invalidate(_q_);
      datatools::invalidate(_t0_);

      datatools::invalidate(_amplitude_);
      datatools::invalidate(_tpeak_);
      datatools::invalidate(_t1_);
      return;
    }

    void triangle_signal_shape::reset_polarity()
    {
      _polarity_ = POLARITY_INVALID;
      return;
    }

    void triangle_signal_shape::set_polarity(polarity_type p_)
    {
      DT_THROW_IF(p_ == POLARITY_INVALID, std::logic_error, "Invalid polarity!");
      _polarity_ = p_;
      return;
    }

    polarity_type triangle_signal_shape::get_polarity() const
    {
      return _polarity_;
    }

    void triangle_signal_shape::set_rise_time(double tr_)
    {
      DT_THROW_IF(tr_ <= 0.0, std::logic_error, "Invalid rise time value!");
      _rise_time_ = tr_;
      return;
    }

    double triangle_signal_shape::get_rise_time() const
    {
      return _rise_time_;
    }

    void triangle_signal_shape::set_fall_time(double tf_)
    {
      DT_THROW_IF(tf_ <= 0.0, std::logic_error, "Invalid fall time value!");
      _fall_time_ = tf_;
      return;
    }

    double triangle_signal_shape::get_fall_time() const
    {
      return _fall_time_;
    }

    void triangle_signal_shape::set_q(double q_)
    {
      DT_THROW_IF(q_ <= 0.0, std::logic_error, "Invalid charge!");
      _q_ = q_;
      return;
    }

    double triangle_signal_shape::get_q() const
    {
      return _q_;
    }

    void triangle_signal_shape::set_t0(double t0_)
    {
      _t0_ = t0_;
      return;
    }

    double triangle_signal_shape::get_t0() const
    {
      return _t0_;
    }

    double triangle_signal_shape::get_t1() const
    {
      return _t1_;
    }

    double triangle_signal_shape::get_tpeak() const
    {
      return _tpeak_;
    }

    double triangle_signal_shape::get_duration() const
    {
      return _t1_ - _t0_;
    }

    double triangle_signal_shape::get_amplitude() const
    {
      return _amplitude_;
    }

    triangle_signal_shape::triangle_signal_shape()
    {
      _set_defaults();
      return;
    }

    triangle_signal_shape::~triangle_signal_shape()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    double triangle_signal_shape::get_non_zero_domain_min() const
    {
      return _t0_;
    }

    double triangle_signal_shape::get_non_zero_domain_max() const
    {
      return _t1_;
    }

    bool triangle_signal_shape::is_valid() const
    {
      return datatools::is_valid(_t0_)
        && datatools::is_valid(_q_)
        && datatools::is_valid(_rise_time_)
        && datatools::is_valid(_fall_time_)
        && polarity_is_set(_polarity_);
    }

    void triangle_signal_shape::reset()
    {
      _triangle_shape_.reset();
      reset_polarity();
      _set_defaults();
      this->i_unary_function::reset();
      return;
    }

    bool triangle_signal_shape::is_initialized() const
    {
      return this->i_unary_function::is_initialized()
        && polarity_is_set(_polarity_)
        && datatools::is_valid(_rise_time_)
        && datatools::is_valid(_fall_time_)
        && datatools::is_valid(_q_)
        && datatools::is_valid(_t0_);
    }

    void triangle_signal_shape::initialize(const datatools::properties & config_,
                                           mygsl::unary_function_dict_type & functors_)
    {
      this->mygsl::i_unary_function::_base_initialize(config_, functors_);

      static const double default_time_unit = CLHEP::nanosecond;
      static const double default_charge_unit = CLHEP::nanosecond * CLHEP::volt;
      static const double default_voltage_unit = CLHEP::volt;

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

      if (!datatools::is_valid(_rise_time_)) {
        if (config_.has_key("rise_time")) {
          double tr = config_.fetch_real("rise_time");
          if (!config_.has_explicit_unit("rise_time")) {
            tr *= default_time_unit;
          }
          set_rise_time(tr);
        }
      }

      if (!datatools::is_valid(_fall_time_)) {
        if (config_.has_key("fall_time")) {
          double tf = config_.fetch_real("fall_time");
          if (!config_.has_explicit_unit("fall_time")) {
            tf *= default_time_unit;
          }
          set_fall_time(tf);
        }
      }

      if (!datatools::is_valid(_q_)) {
        if (config_.has_key("q")) {
          double q = config_.fetch_real("q");
          if (!config_.has_explicit_unit("q")) {
            q *= default_charge_unit;
          }
          set_q(q);
        }
      }

      if (!datatools::is_valid(_t0_)) {
        if (config_.has_key("t0")) {
          double t0 = config_.fetch_real("t0");
          if (!config_.has_explicit_unit("t0")) {
            t0 *= default_time_unit;
          }
          set_t0(t0);
        }
      }

      DT_THROW_IF(!polarity_is_set(_polarity_), std::logic_error,
                  "polarity is not set!");

      DT_THROW_IF(!datatools::is_valid(_rise_time_), std::logic_error,
                  "Rise time is not set!");

      DT_THROW_IF(!datatools::is_valid(_fall_time_), std::logic_error,
                  "Fall time is not set!");

      DT_THROW_IF(!datatools::is_valid(_q_), std::logic_error,
                  "Charge is not set!");

      DT_THROW_IF(!datatools::is_valid(_t0_), std::logic_error,
                  "Start time is not set!");

      _compute_parameters();

      return;
    }

    void triangle_signal_shape::_compute_parameters()
    {
      double tr = _rise_time_ / 0.8;
      double tf = _fall_time_ / 0.8;
      _tpeak_ = _t0_ + tr;
      _t1_ = _tpeak_ + tf;
      _amplitude_ = _q_ * 2 / (tr + tf);
      _triangle_shape_.set_amplitude(_amplitude_);
      _triangle_shape_.set_head_width(tr);
      _triangle_shape_.set_tail_width(tf);
      _triangle_shape_.set_center(_tpeak_);
      return;
    }

    double triangle_signal_shape::_eval(double x_) const
    {
      double res = _triangle_shape_.eval(x_);
      if (_polarity_ == POLARITY_NEGATIVE) {
        res *= -1.0;
      }
      return res;
    }

    void triangle_signal_shape::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      this->i_unary_function::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Polarity: " << _polarity_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Rise time (10-90%): " << _rise_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Fall time (10-90%): " << _fall_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value q : " << _q_ / (CLHEP::volt * CLHEP::ns) << " nV.s" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t0 : " << _t0_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current amplitude : " << _amplitude_ / (CLHEP::volt) << " V" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value tpeak : " << _tpeak_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t1 : " << _t1_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Triangle shape embedded function: ";
      if (_triangle_shape_.is_initialized()) {
        out_ << "<yes>";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      return;
    }

  } // end of namespace signal

} // end of namespace mctools
