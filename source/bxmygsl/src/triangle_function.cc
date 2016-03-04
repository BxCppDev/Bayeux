//! \file mygsl/triangle_function.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mygsl/triangle_function.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(triangle_function,
                                             "mygsl::triangle_function")

  void triangle_function::_set_defaults()
  {
    datatools::invalidate(_center_);
    datatools::invalidate(_head_width_);
    datatools::invalidate(_tail_width_);
    datatools::invalidate(_amplitude_);
    return;
  }

  triangle_function::triangle_function()
  {
    _set_defaults();
    return;
  }

  triangle_function::triangle_function(double width_, double center_, double amplitude_)
  {
    _set_defaults();
    set_head_width(0.5 * width_);
    set_tail_width(0.5 * width_);
    set_center(center_);
    if (datatools::is_valid(amplitude_)) {
      set_amplitude(amplitude_);
    }
    return;
  }

  triangle_function::triangle_function(double head_width_,
                                       double tail_width_,
                                       double center_,
                                       double amplitude_)
  {
    _set_defaults();
    set_head_width(head_width_);
    set_tail_width(tail_width_);
    set_center(center_);
    if (datatools::is_valid(amplitude_)) {
      set_amplitude(amplitude_);
    }
    return;
  }

  triangle_function::~triangle_function()
  {
    return;
  }

  bool triangle_function::is_initialized() const
  {
    return datatools::is_valid(_head_width_)
      && datatools::is_valid(_tail_width_)
      && datatools::is_valid(_center_)
      && datatools::is_valid(_amplitude_);
  }

  void triangle_function::initialize(const datatools::properties & config_,
                                     unary_function_dict_type & /*functors_*/)
  {
    // Parse configuration:
    if (!datatools::is_valid(_head_width_)) {
      if (config_.has_key("head_width")) {
        double hw = config_.fetch_real("head_width");
        set_head_width(hw);
      }
    }

    if (!datatools::is_valid(_tail_width_)) {
      if (config_.has_key("tail_width")) {
        double tw = config_.fetch_real("tail_width");
        set_tail_width(tw);
      }
    }

    if (!datatools::is_valid(_center_)) {
      if (config_.has_key("center")) {
        double c = config_.fetch_real("center");
        set_center(c);
      }
    }

    if (!datatools::is_valid(_amplitude_)) {
      if (config_.has_key("amplitude")) {
        double a = config_.fetch_real("amplitude");
        set_amplitude(a);
      }
    }

    // Checks:

    if (!datatools::is_valid(_center_)) {
      set_center(0.0);
    }

    if (!datatools::is_valid(_head_width_)) {
      set_head_width(0.5);
    }

    if (!datatools::is_valid(_tail_width_)) {
      set_tail_width(0.5);
    }

    if (!datatools::is_valid(_amplitude_)) {
      set_amplitude(2.0 / (_head_width_ + _tail_width_));
    }

    return;
  }

  void triangle_function::reset()
  {
    _set_defaults();
    return;
  }

  void triangle_function::set_head_width(double head_width_)
  {
    DT_THROW_IF(!datatools::is_valid(head_width_), std::logic_error,
                "Invalid head width!");
    DT_THROW_IF(head_width_ <= 0.0, std::logic_error,
                "Invalid negative or null head width!");
    _head_width_ = head_width_;
    return;
  }

  void triangle_function::set_tail_width(double tail_width_)
  {
    DT_THROW_IF(!datatools::is_valid(tail_width_), std::logic_error,
                "Invalid tail width!");
    DT_THROW_IF(tail_width_ <= 0.0, std::logic_error,
                "Invalid negative or null tail width!");
    _tail_width_ = tail_width_;
    return;
  }

  void triangle_function::set_center(double center_)
  {
    DT_THROW_IF(!datatools::is_valid(center_), std::logic_error,
                "Invalid center!");
    _center_ = center_;
    return;
  }

  void triangle_function::set_amplitude(double amplitude_)
  {
    DT_THROW_IF(!datatools::is_valid(amplitude_), std::logic_error,
                "Invalid amplitude!");
    DT_THROW_IF(amplitude_ <= 0.0, std::logic_error,
                "Invalid negative or null amplitude!");
    _amplitude_ = amplitude_;
    return;
  }

  double triangle_function::_eval(double x_) const
  {
    if (has_non_zero_domain_min() && x_ < get_non_zero_domain_min()) {
      return 0.0;
    }
    if (has_non_zero_domain_max() && x_ > get_non_zero_domain_max()) {
      return 0.0;
    }
    double x = x_ - _center_;
    if (x < 0.0) {
      return _amplitude_ + x * _amplitude_ / _head_width_;
    } else {
      return _amplitude_ - x * _amplitude_ / _tail_width_;
    }
  }

  double triangle_function::get_non_zero_domain_min() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Triangle function is not initialized!");
    return _center_ - _head_width_;
  }

  double triangle_function::get_non_zero_domain_max() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Triangle function is not initialized!");
    return _center_ + _tail_width_;
  }

  void triangle_function::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Head width : " << _head_width_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Tail width : " << _tail_width_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Center : " << _center_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Amplitude : " << _amplitude_ << std::endl;

    return;
  }

} // end of namespace mygsl
