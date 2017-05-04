// -*- mode: c++; -*- 
// fermi.cc

#include <iostream>
#include <cmath>
#include <stdexcept>

#include <genbb_help/decay0/funbeta.h>
#include <genbb_help/decay0/fermi.h>
#include <genbb_help/decay0/particle.h>

#include <gsl/gsl_math.h>

namespace genbb {
  namespace decay0 {
   
    /// \brief Function to search the maximum of beta spectrum curve by decay0_tgold subroutine.
    double decay0_funbeta(double E_, void * params_)
    {
      const parbeta * pars = static_cast<const parbeta *>(params_);
      const double & Qbeta = pars->Qbeta;
      const double & Zdtr = pars->Zdtr;
      double f = 0.0;
      static const double emass = decay0_emass();
      if (E_ > 0.) {
        f = std::sqrt(E_ * (E_ + 2. * emass)) 
          * (E_ + emass) 
          * gsl_pow_2(Qbeta - E_)
          * decay0_fermi(Zdtr, E_);
      }
      return f;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
