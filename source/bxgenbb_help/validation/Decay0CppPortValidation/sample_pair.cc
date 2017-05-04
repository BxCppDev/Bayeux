#include <cmath>
#include <mygsl/rng.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/electron.h>
namespace genbb {
  namespace decay0 {
    void decay0_pair(mygsl::rng & prng,
                     primary_event & event,
                     double Epair, double tclev, double thlev,
                     double &tdlev)
    {
      double phi=2.*M_PI*prng();
      double ctet=-1.+2.*prng();
      double teta=std::acos(ctet);
      double E=0.5*Epair;
      decay0_particle(prng,event,2,E,E,teta,teta,phi,phi,tclev,thlev,tdlev);
      decay0_particle(prng,event,3,E,E,teta,teta,phi,phi,0.,0.,tdlev);
      return;
    }
 } // end of namespace decay0
} // end of namespace genbb
