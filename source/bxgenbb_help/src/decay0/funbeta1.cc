// -*- mode: c++; -*- 
// funbeta1.cc

#include <iostream>
#include <cmath>
#include <stdexcept>

#include <gsl/gsl_math.h>

#include <genbb_help/decay0/funbeta1.h>
#include <genbb_help/decay0/fermi.h>
#include <genbb_help/decay0/particle.h>

namespace genbb {
  namespace decay0 {
     
    /// \brief Function to search the maximum of beta spectrum curve by decay0_tgold subroutine
    double decay0_funbeta1(double E_, void * params_)
    {
      const parbeta1 * pars = static_cast<const parbeta1 *>(params_);
      const double & Zdtr   = pars->Zdtr;
      const double & Qbeta  = pars->Qbeta;
      const double & c1     = pars->c1;
      const double & c2     = pars->c2;
      const double & c3     = pars->c3;
      const double & c4     = pars->c4;

      static const double emass = decay0_emass();
      double f = 0.0;
      if (E_ > 0.) {
        // allowed spectrum:
        double all = std::sqrt(E_ * (E_ + 2. * emass)) * (E_ + emass)
          *gsl_pow_2(Qbeta - E_) * decay0_fermi(Zdtr, E_);
        // correction factor:
        double w = E_ / emass + 1.;
        double cf = 1. 
          + c1 / w
          + c2 * w 
          + c3 * gsl_pow_2(w)
          + c4 * gsl_pow_3(w);
        // spectrum with correction:
        f = all * cf;
      }
      return f;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
