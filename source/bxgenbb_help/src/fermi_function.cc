// fermi_function.cc
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
#include <genbb_help/fermi_function.h>

// Standard library:
#include <cmath>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - GSL:
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project:
//#include <genbb_help/decay0/fermi.h>

namespace genbb {

  bool fermi_function::is_beta_plus() const
  {
    return _decay_type_ == DT_BETA_PLUS;
  }

  bool fermi_function::is_beta_minus() const
  {
    return _decay_type_ == DT_BETA_MINUS;
  }

  bool fermi_function::is_valid() const
  {
    return _decay_type_ != DT_INVALID && _Z_ >= 0 && _A_ >= 0;
  }

  void fermi_function::reset()
  {
    _Z_ = -1;
    _A_ = -1;
    _decay_type_  = DT_INVALID;
    _mode_ = 0;
    _ke_cut_ = 0.1 * CLHEP::keV;
    return;
  }

  void fermi_function::set(int z_,
                           int a_,
                           decay_type decay_type_,
                           unsigned int mode_,
                           double ke_cut_)
  {
    _Z_ = z_;
    _A_ = a_;
    _decay_type_ = decay_type_;
    _mode_ = mode_;
    _ke_cut_ = 0.1 * CLHEP::keV;
    if (ke_cut_ > 0.0) {
      _ke_cut_ = ke_cut_;
    }
    return;
  }

  fermi_function::fermi_function()
  {
    _Z_ = -1;
    _A_ = -1;
    _decay_type_ = DT_INVALID;
    _mode_ = 0;
    _ke_cut_ = 0.1 * CLHEP::keV;
    return;
  }

  fermi_function::fermi_function(int z_,
                                 int a_,
                                 decay_type decay_type_,
                                 unsigned int mode_,
                                 double ke_cut_)
  {
    set(z_, a_, decay_type_, mode_, ke_cut_);
    return;
  }


  double fermi_function::operator()(double ke_) const
  {
    double f = 1.0;
    bool shape_only = false;
    if (_mode_ & MODE_SHAPE_ONLY) {
      shape_only = true;
    }
    if (_mode_ & MODE_NON_RELATIVISTIC) {
      return eval_non_relativistic_approx(_Z_,
                                          _decay_type_,
                                          _ke_cut_,
                                          shape_only,
                                          ke_);
    } else if (_mode_ & MODE_SPHERICALLY_SYMMETRIC) {
      return eval_spherically_symmetric_approx(_Z_,
                                               _A_,
                                               _decay_type_,
                                               _ke_cut_,
                                               shape_only,
                                               ke_);
    } else {
      return 1.0;
    }
    return f;
  }

  // static
  double fermi_function::eval_spherically_symmetric_approx(int z_daughter_,
                                                           int a_daughter_,
                                                           decay_type type_,
                                                           double ke_cut_,
                                                           bool shape_only_,
                                                           double ke_)
  {
    double ke = ke_;
    if (ke_ < ke_cut_) {
      ke = ke_cut_;
    }
    double z = z_daughter_;
    double a = a_daughter_;
    // Reduced total energy:
    double we = ke / CLHEP::electron_mass_c2 + 1.;
    // Reduced momentum:
    double pe = std::sqrt(we * we - 1.);
    // Relativistic beta:
    double beta_e = pe / we;
    double alphaz = CLHEP::fine_structure_const * z;
    double g = std::sqrt(1.0 - alphaz * alphaz);
    // Default is DT_BETA_MINUS
    double eta = alphaz / beta_e;
    if (type_ == DT_BETA_PLUS) {
      eta *= -1;
    }
    gsl_sf_result res_lnr, res_arg;
    int status = gsl_sf_lngamma_complex_e(g, eta, &res_lnr, &res_arg);
    if (status != GSL_SUCCESS) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "GSL error: " << gsl_strerror(status));
      DT_THROW_IF(true, std::logic_error,
                  "GSL error at 'gsl_sf_lngamma_complex_e' invocation!");
    }
    double lnr = res_lnr.val;
    double res = std::pow(pe, 2. * g - 2.) * std::exp(M_PI * eta + 2. * lnr);
    if (! shape_only_) {
      res *= 2 * (1 + g);
      res /= gsl_pow_2(gsl_sf_gamma(1. + 2 * g));
      // Radius of the nucleus in 'hbarc/mec2' unit:
      double rho = 1.2 * CLHEP::fermi
        * std::pow(a, 0.3333333333) * CLHEP::electron_mass_c2 / CLHEP::hbarc;
      res *= std::pow(2 * rho, 2. * g - 2.);
    }
    return res;
  }

  // static
  double fermi_function::eval_non_relativistic_approx(int z_daughter_,
                                                      decay_type type_,
                                                      double ke_cut_,
                                                      bool shape_only_,
                                                      double ke_)
  {
    double ke = ke_;
    if (ke_ < ke_cut_) {
      ke = ke_cut_;
    }
    double z = z_daughter_;
    // Reduced total energy:
    double w = ke / CLHEP::electron_mass_c2 + 1.;
    // Reduced momentum:
    double p = std::sqrt(w * w - 1.);
    // Relativistic beta:
    double beta = p / w;
    double tr = 1 / beta;
    double c = 2. * M_PI * CLHEP::fine_structure_const * z;
    double t = c * tr;
    if (type_ == DT_BETA_PLUS) {
      t *= -1;
    }
    double res = tr / (1. - std::exp(-t));
    if (! shape_only_) {
      res *= 2. * M_PI * CLHEP::fine_structure_const * z;
    }
    return std::abs(res);
  }

} // end of namespace genbb
