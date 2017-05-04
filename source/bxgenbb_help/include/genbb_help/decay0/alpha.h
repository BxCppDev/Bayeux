#ifndef GENBB_DECAY0_ALPHA_H_
#define GENBB_DECAY0_ALPHA_H_ 1

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    /// \brief Generation of emission of alpha particle with fixed energy isotropically in the whole space.
    // Input : E     - kinetic energy of particle (MeV);
    //         tclev - time of creation of level from which particle will be
    //                 emitted (sec);
    //         thlev - level halflife (sec).
    // Output: tdlev - time of decay of level (sec);
    // VIT, 15.10.1995.
    void decay0_alpha(mygsl::rng & prng, primary_event & event, 
                      double E, double tclev, double thlev, double &tdlev);


  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_ALPHA_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


