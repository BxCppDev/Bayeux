
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
      decay0_particle(prng,event,GAMMA,E,E,0.,M_PI,0.,2.*M_PI,tclev,thlev,tdlev);
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
