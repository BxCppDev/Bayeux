#ifndef GENBB_DECAY0_NUCLTRANSK_H_
#define GENBB_DECAY0_NUCLTRANSK_H_ 1

#include <cmath>
#include <genbb_help/decay0/common.h>

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {
    
    // Subroutine nucltransK choise one of the three concurent processes
    // by which the transition from one nuclear state to another is
    // occured: gamma-ray emission, internal conversion and internal
    // pair creation. Conversion electrons are emitted only with one fixed energy
    // (usually with Egamma-E(K)_binding_energy).
    // Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
    //         call nucltransK(Egamma,Ebinde,conve,convp,tclev,thlev,tdlev)
    // Input : Egamma - gamma-ray energy (MeV) [=difference in state energies];
    //         Ebinde - binding energy of electron (MeV);
    //         conve  - internal electron conversion coefficient [=Nelectron/Ngamma];
    //         convp  - pair conversion coefficient [=Npair/Ngamma];
    //         tclev  - time of creation of level from which particle will be
    //                  emitted (sec);
    //         thlev  - level halflife (sec).
    // Output: tdlev               - time of decay of level (sec);
    //         tevst               - time of event's start (sec);
    // VIT, 27.07.1992; 15.10.1995.
    void decay0_nucltransK(mygsl::rng & prng, primary_event & event,
			   double Egamma,double Ebinde,
			   double conve,double convp,double tclev,
			   double thlev,double & tdlev);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_NUCLTRANSK_H_

