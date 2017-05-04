// kinematics.cc
/*
 * Copyright 2014 F. Mauger
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
#include <genbb_help/kinematics.h>

// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - CLHEP:
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/utils.h>

namespace genbb {

  kinematics::kinematics()
  {
    reset();
    return;
  }

  void kinematics::reset()
  {
    datatools::invalidate(_m_);
    datatools::invalidate(_w_);
    datatools::invalidate(_k_);
    datatools::invalidate(_p_);
    return;
  }

  double kinematics::m() const
  {
    return _m_;
  }

  double kinematics::w() const
  {
    return _w_;
  }

  double kinematics::k() const
  {
    return _k_;
  }

  double kinematics::p() const
  {
    return _p_;
  }

  double kinematics::beta() const
  {
    return _p_ / _w_;
  }

  double kinematics::gamma() const
  {
    double b = beta();
    return 1. / std::sqrt( 1 - b * b);
  }

  double kinematics::v() const
  {
    return beta() * CLHEP::c_light;
  }

  void kinematics::initialize_from_m_w(double m_, double w_)
  {
    DT_THROW_IF(m_ < 0.0, std::logic_error,
                "Invalid particle mass (" << m_ << ")!");
    DT_THROW_IF(w_ < 0.0, std::logic_error,
                "Invalid particle total energy (" << w_ << ")!");
    DT_THROW_IF(w_ < _m_, std::logic_error,
                "Invalid particle total energy (" << w_ / CLHEP::MeV << "<" << m_ / CLHEP::MeV << " MeV)!");
    _m_ = m_;
    _w_ = w_;
    _k_ = _w_ - _m_;
    _p_ = std::sqrt(_w_ * _w_ - _m_ * _m_);
    return;
  }

  void kinematics::initialize_from_m_k(double m_, double k_)
  {
    DT_THROW_IF(m_ < 0.0, std::logic_error,
                "Invalid particle mass (" << m_ << ")!");
    DT_THROW_IF(k_ < 0.0, std::logic_error,
                "Invalid particle kinetic energy (" << k_ << ")!");
    _m_ = m_;
    _k_ = k_;
    _w_ = _k_ + _m_;
    _p_ = std::sqrt(_w_ * _w_ - _m_ * _m_);
    return;
  }

  void kinematics::initialize_from_m_p(double m_, double p_)
  {
    DT_THROW_IF(m_ < 0.0, std::logic_error,
                "Invalid particle mass (" << m_ << ")!");
    DT_THROW_IF(p_ < 0.0, std::logic_error,
                "Invalid particle momentum (" << p_ << ")!");
    _m_ = m_;
    _p_ = p_;
    _w_ = std::sqrt(_p_ * _p_ + _m_ * _m_);
    _k_ = _w_ - _m_;
    return;
  }

} // end of namespace genbb
