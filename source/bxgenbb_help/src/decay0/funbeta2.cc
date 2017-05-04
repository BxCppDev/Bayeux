// -*- mode: c++; -*- 
// funbeta2.cc

#include <iostream>
#include <cmath>
#include <sstream>
#include <stdexcept>

#include <gsl/gsl_math.h>

#include <genbb_help/decay0/funbeta2.h>
#include <genbb_help/decay0/fermi.h>
#include <genbb_help/decay0/particle.h>

namespace genbb {
  namespace decay0 {
 
    /// \brief Function to search the maximum of beta spectrum curve by decay0_tgold subroutine
    double decay0_funbeta2(double E_, void * params_)
    {
      const parbeta2 * pars = static_cast<const parbeta2 *>(params_);
      const double & Zdtr  = pars->Zdtr;
      const double & Qbeta = pars->Qbeta;
      const double & c1    = pars->c1;
      const double & c2    = pars->c2;
      const double & c3    = pars->c3;
      const double & c4    = pars->c4;
      const int    & kf    = pars->kf;

      static const double emass = decay0_emass();
      double f = 0.0;
      if (E_ > 0.) {
        // allowed spectrum:
        double all = 
          std::sqrt(E_ * (E_ + 2. * emass)) 
          * (E_ + emass)
          * gsl_pow_2(Qbeta - E_) 
          * decay0_fermi(Zdtr, E_);
        // correction factor
        double w = E_ / emass + 1.;
        double pel = std::sqrt(gsl_pow_2(w) - 1.);
        double pnu = (Qbeta - E_) / emass;
        double cf = 1.;
        if (kf == 1) {
            cf =     gsl_pow_2(pel)
              + c1 * gsl_pow_2(pnu);
          }
        else if (kf == 2) {
            cf =     gsl_pow_4(pel)
              + c1 * gsl_pow_2(pel) * gsl_pow_2(pnu)
              + c2 *                  gsl_pow_4(pnu);
          }
        else if (kf == 3) {
            cf =     gsl_pow_6(pel)
              + c1 * gsl_pow_4(pel) * gsl_pow_2(pnu)
              + c2 * gsl_pow_2(pel) * gsl_pow_4(pnu)
              + c3 *                  gsl_pow_6(pnu);
          }
        else if (kf == 4) { 
            cf =     gsl_pow_8(pel)
              + c1 * gsl_pow_6(pel) * gsl_pow_2(pnu)
              + c2 * gsl_pow_4(pel) * gsl_pow_4(pnu)
              + c3 * gsl_pow_2(pel) * gsl_pow_6(pnu)
              + c4 *                  gsl_pow_8(pnu);
          }
        else if (kf > 4) {  
          std::ostringstream message;
          message << "genbb::decay0::decay0_funbeta2: " 
                  << "Invalid kf value ( "
                  << kf << ") !";
          throw std::logic_error (message.str());
        }
        // spectrum with correction
        f = all * cf;
      }
      return f;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
