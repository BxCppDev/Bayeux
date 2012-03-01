// -*- mode: c++; -*- 
// fermi.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <genbb_help/fermi.h>

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <gsl/gsl_sf.h>
#include <CLHEP/Units/SystemOfUnits.h>

double a_from_z (double z_)
{
 return 0.0075 * z_ * z_ + 1.9 * z_ + 0.25;
}


float fermi_func_nr_approx (float z_, float e_)
{
  double e = e_; 
  if (e_ < 50.e-6) e = 50.e-6;
  double z = z_;
  double emass = 0.510999084; 
  double alpha = 1.0 / 137.036;
  double me    = emass;
  double w     = e / emass + 1.; 
  double p     = sqrt (w * w - 1.);
  double beta  = p / w;
  /*
  std::clog << "genbb_help::fermi_func_nr_approx: DEVEL: p/beta==" 
            << p << ' ' << beta << std::endl;
  */
  double t     = 2. * M_PI * alpha * z / beta;
  return (t / (1. - std::exp (-t)));
}

float fermi_func_decay0 (float z_, float e_, bool devel_)
{ 
  using namespace std;
  bool test = devel_;
  double e = e_; 
  double z = z_; 
  if (e < 50.e-6) e = 50.e-6;
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: e = " << e << endl;
  double alfaz = z / 137.036;
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: alfaz = " << alfaz << endl;
  double w = e / 0.511 + 1.;
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: w = " << w << endl;
  double p = sqrt (w * w - 1.);
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: p = " << p << endl;
  double y = alfaz * w / p;
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: y = " << y << endl;
  double g = sqrt (1. - alfaz * alfaz);
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: g = " << g << endl;
  //double carg = cmplx (g, y);
  gsl_sf_result res_lnr, res_arg;
  int status = gsl_sf_lngamma_complex_e (g, y, &res_lnr, &res_arg);
  if (status != GSL_SUCCESS) 
    {
      cerr << "genbb_help::fermi_func_decay0: GSL error: " 
           << gsl_strerror (status) << endl;
      throw logic_error ("genbb_help::fermi_func_decay0: GSL error at 'gsl_sf_lngamma_complex_e' invocation!");
    }
  double lnr = res_lnr.val;
  //double res = exp((2. * g - 2.)*log (p)) * exp (M_PI * y + 2. * lnr);
  double res = pow (p, 2. * g - 2.) * exp (M_PI * y + 2. * lnr);
  if (test) clog << "DEVEL: genbb_help::fermi_func_decay0: ----> res = " << res << endl;
  return (float) res;
}


float fermi_func (float z_, float e_, bool use_l0_)
{
  using namespace std;
  double e = e_; 
  if (e_ < 50.e-6) e = 50.e-6;
  double z = z_;
  double emass = 0.510999084; // MeV 
  double alpha = 1.0 / 137.036;
  double hbarc = 197.3269631; // MeV.fm
  double r0    = 1.2; // fm
  double me    = emass; // MeV
  double we    = e / emass + 1.; 
  double pe    = sqrt (we * we - 1.);
  //double beta  = pe / we;
  /*
  clog << "genbb_help::fermi_func_2: DEVEL: p/beta==" 
            << pe << ' ' << beta << endl;
  */
  double aZ     = alpha * z;
  double gamma1 = sqrt (1. - aZ * aZ);
  double y      = aZ * we / pe;
  double a      = a_from_z (z);
  double R      = r0 * exp (log (a) / 3.) * me / hbarc;
  double F0 = 4. * exp (2. * (gamma1 - 1.) * log (2. * pe * R)) 
    * exp (M_PI * y);
  double g2 = gsl_sf_gamma (2. * gamma1 + 1.);  
  gsl_sf_result res_lnr, res_arg;
  int status = gsl_sf_lngamma_complex_e (gamma1, y, &res_lnr, &res_arg);
  if (status != GSL_SUCCESS) 
    {
      cerr << "genbb_help::fermi_func_2: GSL error: " 
           << gsl_strerror (status) << endl;
      throw logic_error ("genbb_help::fermi_func_2: GSL error at 'gsl_sf_lngamma_complex_e' invocation !");
    }
  double lnr = res_lnr.val;
  //clog << "genbb_help::fermi_func_2: lnr=" << lnr << endl;
  double g1 = exp (lnr);
  //clog << "genbb_help::fermi_func_2: g1=" << g1 << endl;
  F0 *= (g1 * g1);
  F0 /= (g2 * g2);
  double L0 = 1.0;
  if(use_l0_) 
    {
      double term = 1. - aZ * (we * R - 7. * aZ / 15.) 
        - 0.5 * gamma1* aZ * R / we;
      L0 = 0.5 * (1. + gamma1) * term;
    }
  //clog << "genbb_help::fermi_func_2: L0=" << L0 << endl;
  /*
  double term = 1.-aZ*(we*R-7.*aZ/15.) - 0.5*gamma1*aZ*R/we;
  L0 = 0.5*(1.+gamma1)*term;
  */
  return (F0 * L0);
}


float fermi_func_shape_only (float z_, float e_)
{
  using namespace std;
  double e = e_; 
  if (e_ < 50.e-6) e = 50.e-6;
  double z = z_;
  double emass = 0.510999084; 
  double alpha = 1.0 / 137.036;
  double aZ     = alpha * z;
  double we = e / emass + 1.; 
  double pe = sqrt (we * we - 1.);
  double beta  = pe / we;
  /*
  clog << "genbb_help::fermi_func: DEVEL: p/beta==" 
            << pe << ' ' << beta << endl;
  */
  double y = aZ * we / pe;
  double gamma1 = sqrt (1. - aZ * aZ); 
  /*
  cout << "genbb_help::fermi_func: DEVEL: gamma1/y==" 
            << gamma1 << ' ' << y << endl;
  */
  gsl_sf_result res, arg;
  int err = gsl_sf_lngamma_complex_e (gamma1, y, &res, &arg);
  double lnr     = res.val;
  //clog << "genbb_help::fermi_func: lnr=" << lnr << endl;
  double lnr_err = res.err;
  double zarg     = arg.val;
  double zarg_err = arg.err;
  return exp (2. * (gamma1 - 1.) * log (pe)) 
    * exp (M_PI * y + 2. * lnr);
}

/* The following function is invoked from Fortran
 * using special mangling name.
 */

//#include <genbb_help/__genbb_help_FC.h>

extern "C"
{
  float fermi_wrap (float * z_, float * e_)
  {
    float res = fermi_func_decay0 (*z_, *e_);
    return res;
  }
}

// end
