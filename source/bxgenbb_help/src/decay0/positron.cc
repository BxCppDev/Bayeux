// positron.cc

#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/primary_event.h>

namespace genbb {
  namespace decay0 {
    
    void decay0_positron(mygsl::rng & prng, primary_event & event, 
                         double E, double tclev, double thlev, 
                         double &tdlev)
    {
      decay0_particle(prng,event,POSITRON,E,E,0.,M_PI,0.,2.*M_PI,tclev,thlev,tdlev);
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
// 
// end of positron.cc
// 
// Local Variables: --
// mode: c++ --
// End: --
