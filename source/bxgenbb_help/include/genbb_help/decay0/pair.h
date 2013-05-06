#ifndef GENBB_DECAY0_PAIR_H_
#define GENBB_DECAY0_PAIR_H_ 1

#include <cmath>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

namespace genbb {
  namespace decay0 {

    // Generation of e+e- pair in zero-approximation to real subroutine for
    // INTERNAL pair creation:
    //    1) energy of e+ is equal to the energy of e-;
    //    2) e+ and e- are emitted in the same direction.
    // Call  : 
    //         call pair(Epair,tclev,thlev,tdlev)
    // Input:  Epair - kinetic energy of e+e- pair (MeV);
    //         tclev - time of creation of level from which pair will be
    //                 emitted (sec);
    //         thlev - level halflife (sec).
    // Output: tdlev - time of decay of level (sec);
   void decay0_pair(mygsl::rng & prng, primary_event & event, 
                    double Epair, double tclev, double thlev, double &tdlev);
    
  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_PAIR_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


