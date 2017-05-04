
#include <cmath>

#include <gsl/gsl_math.h>

#include <genbb_help/decay0/fe12_mods.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/fermi.h>

namespace genbb {
  namespace decay0 {
    
    double decay0_fe12_mod4(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_5(e0-e1-e2);
    }
    
    double decay0_fe12_mod5(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * (e0-e1-e2);
    }

    double decay0_fe12_mod6(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_3(e0-e1-e2);
    }
  
    double decay0_fe12_mod8(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_7(e0-e1-e2) * gsl_pow_2(e1-e2);
    }

    double decay0_fe12_mod13(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_7(e0-e1-e2);
    }

    double decay0_fe12_mod14(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_2(e0-e1-e2);
    }

    double decay0_fe12_mod15(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass) * p1 * decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass) * p2 * decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_5(e0-e1-e2)
        * (9. * gsl_pow_2(e0-e1-e2) + 21. * gsl_pow_2(e2-e1));
    }

    double decay0_fe12_mod16(double e2, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      const double & e1   = pars->e1;
      if (e2 > e0 - e1) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return 
        (e1+emass) * p1 * decay0::decay0_fermi(Zdbb,e1)
        * (e2+emass) * p2 * decay0::decay0_fermi(Zdbb,e2)
        * gsl_pow_5(e0-e1-e2)
        * gsl_pow_2(e2-e1);
    }



} // end of namespace decay0 
} // end of namespace genbb 
