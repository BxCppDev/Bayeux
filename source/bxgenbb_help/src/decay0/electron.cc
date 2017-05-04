// electron.cc

#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/primary_event.h>

namespace genbb {
  namespace decay0 {

    void decay0_electron(mygsl::rng & prng, primary_event & event,
                         double E, double tclev, double thlev,
                         double &tdlev)
    {
      decay0_particle(prng,event,ELECTRON,E,E,0.,M_PI,0.,2.*M_PI,tclev,thlev,tdlev);
      //std::cerr << "genbb::decay0::decay0_electron: E=" << E << " MeV" << std::endl;
      return;
    }

  } // end of namespace decay0
} // end of namespace genbb
//
// end of electron.cc
//
// Local Variables: --
// mode: c++ --
// End: --
