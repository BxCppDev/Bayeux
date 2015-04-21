//! \file mygsl/gate_function.cc
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
#include <mygsl/gate_function.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(gate_function,
                                             "mygsl::gate_function");

  void gate_function::_set_defaults()
  {
    datatools::invalidate(_center_);
    datatools::invalidate(_width_);
    datatools::invalidate(_amplitude_);
    return;
  }

  gate_function::gate_function()
  {
    _set_defaults();
    return;
  }

  gate_function::gate_function(double width_, double center_, double amplitude_)
  {
    _set_defaults();
    set_width(width_);
    set_center(center_);
    if (datatools::is_valid(amplitude_)) {
      set_amplitude(amplitude_);
    }
    return;
  }

  gate_function::~gate_function()
  {
    return;
  }

  bool gate_function::is_initialized() const
  {
    return datatools::is_valid(_width_)
      && datatools::is_valid(_center_)
      && datatools::is_valid(_amplitude_);
  }

  void gate_function::initialize(const datatools::properties & config_,
                                 unary_function_dict_type & /*functors_*/)
  {
    // Parse configuration:
    if (!datatools::is_valid(_width_)) {
      if (config_.has_key("width")) {
        double w = config_.fetch_real("width");
        set_width(w);
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

    if (!datatools::is_valid(_width_)) {
      set_width(1.0);
    }

    if (!datatools::is_valid(_amplitude_)) {
      set_amplitude(1.0 / _width_);
    }

    return;
  }

  void gate_function::reset()
  {
    _set_defaults();
    return;
  }

  void gate_function::set_width(double width_)
  {
    DT_THROW_IF(!datatools::is_valid(width_), std::logic_error,
                "Invalid width!");
    DT_THROW_IF(width_ <= 0.0, std::logic_error,
                "Invalid negative or null width!");
    _width_ = width_;
    return;
  }

  void gate_function::set_center(double center_)
  {
    DT_THROW_IF(!datatools::is_valid(center_), std::logic_error,
                "Invalid center!");
    _center_ = center_;
    return;
  }

  void gate_function::set_amplitude(double amplitude_)
  {
    DT_THROW_IF(!datatools::is_valid(amplitude_), std::logic_error,
                "Invalid amplitude!");
    DT_THROW_IF(amplitude_ <= 0.0, std::logic_error,
                "Invalid negative or null amplitude!");
    _amplitude_ = amplitude_;
    return;
  }

  double gate_function::_eval(double /*x_*/) const
  {
    double res = _amplitude_;
    if (res < _center_ - 0.5 * _width_) {
      return 0.0;
    }
    if (res > _center_ + 0.5 * _width_) {
      return 0.0;
    }
    return _amplitude_;
  }

  double gate_function::get_non_zero_domain_min() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Gate function is not initialized!");
    return _center_ - 0.5 * _width_;
  }

  double gate_function::get_non_zero_domain_max() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Gate function is not initialized!");
    return _center_ + 0.5 * _width_;
  }

  void gate_function::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Width : " << _width_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Center : " << _center_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Amplitude : " << _amplitude_ << std::endl;

    return;
  }

} // end of namespace mygsl
