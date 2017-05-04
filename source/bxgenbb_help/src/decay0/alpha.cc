
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/primary_event.h>

namespace genbb {
  namespace decay0 {
    
    void decay0_alpha(mygsl::rng & prng_, primary_event & event_, 
                      double E_, double tclev_, double thlev_, double & tdlev_)
    {
      decay0_particle(prng_, event_,
                      ALPHA, E_, E_,
                      0., M_PI, 0., 2. * M_PI,
                      tclev_, thlev_, tdlev_);
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
