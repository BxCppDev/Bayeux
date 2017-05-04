// two_body_decay.cc
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
#include <genbb_help/two_body_decay.h>

// Third party:
// - GSL:
#include <gsl/gsl_pow_int.h>
// - Bayeux/datatools:
#include <datatools/utils.h>

namespace genbb {

  two_body_decay::two_body_decay()
  {
    datatools::invalidate(_M_);
    return;
  }

  two_body_decay::two_body_decay(double M_, double m1_, double m2_)
  {
    set(M_, m1_, m2_);
    return;
  }

  void two_body_decay::reset()
  {
    datatools::invalidate(_M_);
    _daughter1_.reset();
    _daughter2_.reset();
    return;
  }

  void two_body_decay::set(double M_, double m1_, double m2_)
  {
    DT_THROW_IF(M_ < (m1_ + m2_), std::logic_error,
                "Mass of the decaying particle is below the threshold!");
    _M_ = M_;
    double E1 = 0.5 * (gsl_pow_2(_M_) - gsl_pow_2(m2_) + gsl_pow_2(m1_)) / _M_;
    double E2 = _M_ - E1;
    _daughter1_.initialize_from_m_w(m1_, E1);
    _daughter2_.initialize_from_m_w(m2_, E2);
    return;
  }

  bool two_body_decay::is_valid() const
  {
    return datatools::is_valid(_M_);
  }

  double two_body_decay::M() const
  {
    return _M_;
  }

  double two_body_decay::m1() const
  {
    return _daughter1_.m();
  }

  double two_body_decay::m2() const
  {
    return _daughter2_.m();
  }

  const kinematics & two_body_decay::daughter1() const
  {
    return _daughter1_;
  }

  const kinematics & two_body_decay::daughter2() const
  {
    return _daughter2_;
  }

} // end of namespace genbb
