// nucltransK.cc

#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/electron.h>

namespace genbb {
  namespace decay0 {

    void decay0_nucltransK(mygsl::rng & prng, primary_event & event,
                           double Egamma,double Ebinde,
                           double conve,double convp,double tclev,
                           double thlev,double & tdlev)
    {
      static double emass = decay0_emass();
      double p=(1.+conve+convp)*prng();
      if (p <= 1.) 
        {
          decay0_gamma(prng,event,Egamma,tclev,thlev,tdlev);
        }
      else if (p <= 1.+conve) 
        {
          decay0_electron(prng,event,Egamma-Ebinde,tclev,thlev,tdlev);
          //      decay0_gamma(Ebinde,tdlev,0.,tdlev); // for real time
          decay0_gamma(prng,event,Ebinde,0.,0.,tdlev);                // for time shift
        }
      else
        {
          decay0_pair(prng,event,Egamma-2.*emass,tclev,thlev,tdlev);
        }
      return;
    }
  } // end of namespace decay0 
} // end of namespace genbb 
//
// end of nucltransK.cc
//
// Local Variables: --
// mode: c++ --
// End: --
