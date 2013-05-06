
#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/gfang.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_gfang(const double * p, 
                      double &costh, double &sinth, double &cosph, double &sinph, 
                      bool &rotate)
    {
      /*
        // Could also use CLHEP 3D-vector class :
        geomtools::vector_3d pp(p[0],p[1],p[2]);
        std::clog << "Cos(theta) = " << std::cos(pp.theta()) 
        << " (" << pp.cosTheta() << ')' << '\n';
        std::clog << "Sin(theta) = " << std::sin(pp.theta()) << '\n';
        std::clog << "Cos(phi)   = " << std::cos(pp.phi()) << '\n';
        std::clog << "Sin(phi)   = " << std::sin(pp.phi()) << '\n';
        std::clog << '\n';
      */
      static const double one = 1.0;
      // additional (as compared with original GFANG) normalization
      double pfull=std::sqrt(gsl_pow_2(p[0])
                             +gsl_pow_2(p[1])
                             +gsl_pow_2(p[2]));
      double dux=p[0]/pfull;
      double duy=p[1]/pfull;
      double duz=p[2]/pfull;
      rotate=true;
      if (std::abs(duz) >= 0.85) {
        double dsith2=gsl_pow_2(dux)+gsl_pow_2(duy);
        if(dsith2 > 0.) {
          double sduz = (duz > 0)? one: -one;
          costh=sduz*std::sqrt(one-dsith2);
          double dsith=std::sqrt(dsith2);
          sinth=dsith;
          cosph=dux/dsith;
          sinph=duy/dsith;
        }
        else if (duz > 0.) {
          rotate=false;
          costh=1.;
          sinth=0.;
          cosph=1.;
          sinph=0.;
        }
        else {
          costh=-1.;
          sinth=0.;
          cosph=1.;
          sinph=0.;
        }
      }
      else {
        costh=duz;
        double dsith=std::sqrt((one+duz)*(one-duz));
        sinth=dsith;
        double dnorm=one/std::sqrt(gsl_pow_2(dux)+gsl_pow_2(duy));
        cosph=dux*dnorm;
        sinph=duy*dnorm;
      }
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
