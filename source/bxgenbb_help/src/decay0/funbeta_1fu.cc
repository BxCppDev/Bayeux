// -*- mode: c++; -*- 
// funbeta_1fu.cc

#include <iostream>
#include <cmath>
#include <stdexcept>

#include <gsl/gsl_math.h>

#include <genbb_help/decay0/funbeta_1fu.h>
#include <genbb_help/decay0/fermi.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/plog69.h>
#include <genbb_help/decay0/divdif.h>

namespace genbb {
  namespace decay0 {
     
    double decay0_funbeta_1fu(double E_, void * params_)
    {
      const bj69sl2 * pars = static_cast<const bj69sl2 *>(params_);
      const double & Zdtr = pars->Zdtr;
      const double & Qbeta = pars->Qbeta;
      const double & c1 = pars->c1;
      const double & c2 = pars->c2;
      const double & c3 = pars->c3;
      const double & c4 = pars->c4;
      const double * sl2 = pars->sl2;

      static const double emass = decay0_emass();
      double f = 0.0;
      if (E_ > 0.) {
        // allowed spectrum
        double all = 
          std::sqrt(E_ * (E_ + 2. * emass)) * (E_ + emass)
          * gsl_pow_2(Qbeta - E_) * decay0_fermi(Zdtr, E_);
        // correction factor 1 (theoretical)
        double w = E_ / emass + 1.;
        double pel = std::sqrt(w * w - 1.);
        double pnu = (Qbeta - E_) / emass;
        // calculation of the screened lambda2 value by interpolation of the table 
        // with the help of the divdif CERN function for logarithms of p
        double pellog = std::log(pel) ;
        double scrl2 = decay0_divdif(sl2, BJ69::plog69, 48, pellog, 2);
        double cf1 = gsl_pow_2(pnu) + scrl2 * gsl_pow_2(pel);
        // correction factor 2 (empirical)
        double cf2 = 1. 
          + c1 / w 
          + c2 * w 
          + c3 * gsl_pow_2(w)
          + c4 * gsl_pow_3(w);
        // spectrum with correction
        f = all * cf1 * cf2;
      }
      return f;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
