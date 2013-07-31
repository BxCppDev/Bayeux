
#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/decay0/moller1.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/gfang.h>
#include <genbb_help/decay0/gdrot.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_moller1(mygsl::rng & prng, 
                        double dcute,
                        const double * pe0,
                        double * pe1, double * pe2)
    {
      static double emass = decay0_emass();
      bool rotate;
      double phi;
      double cosphi;
      double sinphi;
      double costh;
      double sinth;
      double eel;
      double tel;
      double pel;
      double cosph;
      double sinph;

      double p = std::sqrt(  gsl_pow_2(pe0[0])
                           + gsl_pow_2(pe0[1])
                           + gsl_pow_3(pe0[2]));
      double xe = std::sqrt(gsl_pow_2(p)+gsl_pow_2(emass));
      double te = xe-emass;
      double gam = xe/emass;
      double gam2 = gam*gam;
      double t = gam-1.;
      double x = dcute/(t*emass);
      if (x >= 0.5) {
        for(int i = 0; i < 3; i++)
          {
            pe2[i] = pe0[i];
            pe1[i] = 0.;
          }
        return;
      }
      double cc = 1.-2.*x;
    tag_1:
      double e = x/(1.-cc*prng());
      double b1 = 4./(9.*gam2-10.*gam+5.);
      double b2 = t*t*b1;
      double b3 = (2.*gam2+2.*gam-1.)*b1;
      double e1 = 1.-e;
      double screj = b2*e*e-b3*e/e1+b1*gam2/(e1*e1);
      if (prng() > screj) goto tag_1;
      eel = (t*e+1.)*emass;
      tel = eel-emass;
      pel = std::sqrt(std::abs((eel+emass)*tel));
      costh = (xe*eel+emass*(tel-xe))/(p*pel);
      if (costh >= 1.) {
        costh = 1.;
        sinth = 0.;
      }
      else if (costh <= -1.) {
        costh = -1.;
        sinth = 0.;
      }
      else {
        sinth = std::sqrt((1.+costh)*(1.-costh));
      }
      phi = 2.*M_PI*prng();
      cosphi = std::cos(phi);
      sinphi = std::sin(phi);
      // Polar co-ordinates to momentum components:
      pe2[0] = pel*sinth*cosphi;
      pe2[1] = pel*sinth*sinphi;
      pe2[2] = pel*costh;
      pe1[0] = -pe2[0];
      pe1[1] = -pe2[1];
      pe1[2] = p-pe2[2];
      // Rotate to MRS
      decay0_gfang(pe0,costh,sinth,cosph,sinph,rotate);
      if (rotate) {
        decay0_gdrot(pe1,costh,sinth,cosph,sinph);
        decay0_gdrot(pe2,costh,sinth,cosph,sinph);
      }
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
