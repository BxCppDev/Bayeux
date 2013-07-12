
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/particle.h>

namespace genbb {
  namespace decay0 {

    void decay0_gamma(mygsl::rng & prng, primary_event & event,
                      double E, double tclev,double thlev,
                      double &tdlev)
    {
      bool debug = false;
      double Eeff = E;
      /* Workaround roundoff error */
      //double Eeff = E+0.000001;
      decay0_particle(prng,event,GAMMA,Eeff,Eeff,0.,M_PI,0.,2.*M_PI,tclev,thlev,tdlev);
      if (debug) {
        std::cerr << "genbb::decay0::decay0_gamma: with E=" << E << " MeV & thlev=" << thlev << " sec" << std::endl;
        std::cerr << "  -> tdlev=" << tdlev << " sec" << std::endl;
      }
      return;
    }

  } // end of namespace decay0
} // end of namespace genbb
