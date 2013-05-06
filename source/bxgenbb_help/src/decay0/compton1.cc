
#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/decay0/compton1.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/gfang.h>
#include <genbb_help/decay0/gdrot.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_compton1(mygsl::rng & prng, 
                         const double * pg0, double *pg1, double *pe1)
    {
      static double emass = decay0_emass();
      bool rotate;
      
      double egam0 = std::sqrt(gsl_pow_2(pg0[0])
                               + gsl_pow_2(pg0[1])
                               + gsl_pow_2(pg0[2]));
      double ezero=egam0/emass;
      double emini=1.+2.*ezero;
      double emin=1./emini;
      double dsig1=std::log(emini);
      double dsig2=0.5*(1.-emin*emin);
      double dsigt=dsig1+dsig2;
      double brd;
      double br;
      double egam1;
      double t;
      double costh;
      double sinth;
      double cosph;
      double sinph;
      double rej;
      double phi;
      double cosphi;
      double sinphi;

      // Decide which part of f(e)=1/e+e to sample from
    tag_1:
      if (dsig1 < dsigt*prng()) {
        // Sample from f2(e) distribution
        brd=prng();
        if(ezero >= (ezero+1.)*prng()) {
          brd=std::max(brd,prng());
        }
        br = emin+(1.-emin)*brd;
      }
      else {
        br = emin * std::exp(dsig1*prng());
      }
      // Scattered photon energy
      egam1 = br * egam0;
      // Calculate rejection function g(e)
      t = emass*(1.-br)/egam1;
      sinth = std::max(0.,t*(2.-t));
      rej = 1.0-(br*sinth)/(1.+br*br);
      if (prng() > rej) goto tag_1;

      // Generate photon angles with respect to a Z-axis defined
      // along the parent photon; phi is generated isotropically
      sinth = std::sqrt(sinth);
      costh = 1.-t;
      phi = 2.*M_PI*prng();
      cosphi = std::cos(phi);
      sinphi = std::sin(phi);
      // Polar co-ordinates to momentum components
      pg1[0] = egam1*sinth*cosphi;
      pg1[1] = egam1*sinth*sinphi;
      pg1[2] = egam1*costh;
      // Momentum vector of recoil electron
      pe1[0] = -pg1[0];
      pe1[1] = -pg1[1];
      pe1[2] = egam0-pg1[2];
      // Rotate electron and scattered photon into MRS system
      decay0_gfang(pg0,costh,sinth,cosph,sinph,rotate);
      if (rotate) {
        decay0_gdrot(pg1,costh,sinth,cosph,sinph);
        decay0_gdrot(pe1,costh,sinth,cosph,sinph);
      }
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
