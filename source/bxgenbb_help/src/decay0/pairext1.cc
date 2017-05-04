// pairext1.cc

#include <genbb_help/decay0/pairext1.h>

#include <iostream>
#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/gfang.h>
#include <genbb_help/decay0/gdrot.h>

namespace genbb {
  namespace decay0 {

    void decay0_pairext1(mygsl::rng & prng,
                         double Z,
                         const double *pg0, double *pe1, double *pe2)
    {
      static double emass = decay0_emass();
      double Eel1;
      double Eel2;
      double pel1;
      double pel2;
      double x;
      double d;
      double alfa, beta, w1;
      double umax, u;
      double theta, sinth, costh;
      double phi, cosphi, sinphi;
      double r1, r2;
      double dmax;
      double dsig1;
      double dsig2;
      double bpar;
      double z3;
      double f;
      double aZ2;
      double fcoulomb;
      double x0;
      double dx;
      double dmin;
      double f10, f20;
      double f1, f2;
      double screj;
      double cosal;
      double sinal;
      double cosbt;
      double sinbt;
      int irej;
      bool   rotate = false;
      double Egam = std::sqrt(gsl_pow_2(pg0[0])+gsl_pow_2(pg0[1])+gsl_pow_2(pg0[2]));
      // If not enough energy, no pair production
      if (Egam < 2.*emass) goto tag_4;
      // For photons <2.1 MeV approximate the electron energy by sampling
      // from a uniform distribution in the interval [emass,Egam/2]
      if (Egam <= 2.1)
        {
          Eel1 = emass+prng()*(0.5*Egam-emass);
          x = Eel1/Egam;
          goto tag_2;
        }
      // Calculate some constants
      z3 = std::pow(Z,1./3.);
      f = 8./3.*std::log(Z);
      if (Egam > 50.) {
        aZ2 = gsl_pow_2(Z/137.036);
        fcoulomb = aZ2*( 1./(1.+aZ2)
                                + 0.20206-0.0369*aZ2
                                + 0.0083*gsl_pow_2(aZ2)
                                - 0.002*gsl_pow_3(aZ2) );
        f = f+8.*fcoulomb;
      }
      x0 = emass/Egam;
      dx = 0.5-x0;
      dmin = 544.*x0/z3;
      if (dmin <= 1.) {
        f10 = 42.392-7.796*dmin+1.961*gsl_pow_2(dmin)-f;
        f20 = 41.405-5.828*dmin+0.8945*gsl_pow_2(dmin)-f;
      } else {
        f10 = 42.24-8.368*std::log(dmin+0.952)-f;
        f20 = f10;
      }
      // Calculate limit for screening variable, DELTA, to ensure
      // that screening rejection functions always remain positive
      dmax = exp((42.24-f)/8.368)-0.952;
      dsig1 = dx*dx*f10/3.;
      dsig2 = 0.5*f20;
      bpar = dsig1/(dsig1+dsig2);
      // Decide which screening rejection function to use and
      // sample the electron/photon fractional energy
    tag_1:
      r1 = prng();
      r2 = prng();
      if (r1 < bpar) {
        x = 0.5-dx*std::pow(r2,(1./3.));
        irej = 1;
      } else {
        x = x0+dx*r2;
        irej = 2;
      }
      // Calculate DELTA ensuring positivity
      d = 0.25*dmin/(x*(1.-x));
      if (d >= dmax) goto tag_1;
      // Calculate F1 and F2 functions. F10 and F20 are the F1
      // and F2 functions calculated for the DELTA=DELTA minimum.
      if (d <= 1.) {
        f1 = 42.392-7.796*d+1.961*gsl_pow_2(d)-f;
        f2 = 41.405-5.828*d+0.8945*gsl_pow_2(d)-f;
      } else {
        f1 = 42.24-8.368*std::log(d+0.952)-f;
        f2 = f1;
      }
      if (irej == 1) screj = f1/f10;
      if (irej == 2) screj = f2/f20;
      // Accept or reject on basis of random variate
      if (prng() > screj) goto tag_1;
      Eel1 = x * Egam;
      // Generate electron decay angles with respect to a Z-axis defined
      // along the parent photon. PHI is generated isotropically.
    tag_2:
      alfa = 0.625;
      d = 0.13*(0.8+1.3/Z)*(100.+1./Eel1)*(1.+x);
      w1 = 9./(9.+d);
      umax = Eel1*M_PI/emass;
    tag_3:
      beta = alfa;
      if (prng() > w1) beta = 3.*alfa;
      r1 = prng();
      u = -(std::log(prng()*r1))/beta;
      if (u >= umax) goto tag_3;
      theta = u*emass/Eel1;
      sinth = std::sin(theta);
      costh = std::cos(theta);
      phi = 2.*M_PI*prng();
      cosphi = std::cos(phi);
      sinphi = std::sin(phi);

      // Momentum components of first electron
      pel1 = std::sqrt((Eel1+emass)*(Eel1-emass));
      pe1[0] = pel1*sinth*cosphi;
      pe1[1] = pel1*sinth*sinphi;
      pe1[2] = pel1*costh;

      // Momentum vector of second electron. Recoil momentum of
      // target nucleus/electron is ignored.
      Eel2 = Egam-Eel1;
      pel2 = std::sqrt((Eel2+emass)*(Eel2-emass));
      // correction for conservation of x and y components of momentum
      sinth = sinth*pel1/pel2;
      costh = std::sqrt(std::max(0.,1.-sinth*sinth));
      pe2[0] = -pel2*sinth*cosphi;
      pe2[1] = -pel2*sinth*sinphi;
      pe2[2] = pel2*costh;
      // Rotate tracks to MRS
      decay0_gfang(pg0,cosal,sinal,cosbt,sinbt,rotate);
      if (rotate) {
        decay0_gdrot(pe1,cosal,sinal,cosbt,sinbt);
        decay0_gdrot(pe2,cosal,sinal,cosbt,sinbt);
      }
      return;
    tag_4:
      std::cerr << "e+e- pair is not created: Egamma < 1.022 MeV" << std::endl;
      return;
    }

  } // end of namespace decay0
} // end of namespace genbb
//
// end of pairext1.cc
//
// Local Variables: --
// mode: c++ --
// End: --
