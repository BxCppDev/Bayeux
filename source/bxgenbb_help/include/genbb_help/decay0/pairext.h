#ifndef GENBB_DECAY0_PAIREXT_H_
#define GENBB_DECAY0_PAIREXT_H_ 1

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    // To sample the momentum of initial gamma quantum for PAIREXT1 subroutine
    // and store the momenta of created positron and electron in common/genevent/.
    // Call  :
    //         call pairext(E1,E2,teta1,teta2,phi1,phi2,Z)
    // Input : E1,E2       - range of kinetic energy of gamma (MeV);
    //         teta1,teta2 - range of teta angle (radians);
    //         phi1,phi2   - range of phi  angle (radians);
    //         Z           - atomic number of target nuclei;
    // Output: see description of common/genevent/. Times emission of scattered
    //         e+ and e- are supposed to be 0.
    // VIT, 12.03.1998.
    void decay0_pairext(mygsl::rng & prng, primary_event & event,
                        double E1,double E2,
                        double teta1,double teta2,
                        double phi1,double phi2,double Z);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_PAIREXT_H_
//
// Local Variables: --
// mode: c++ --
// End: --
