#ifndef GENBB_DECAY0_PAIREXT1_H_
#define GENBB_DECAY0_PAIREXT1_H_ 1


namespace mygsl {
  class rng;
}

namespace genbb {
  namespace decay0 {

    /// \brief Simulates e+e- pair production by photons in a target with atomic number Z.
    // Call  : common/const/pi,emass
    //         dimension pg0(3),pe1(3),pe2(3)
    //         call pairext1(Z,pg0,pe1,pe2)
    // Input : Z   - atomic number of the substance;
    //         pg0 - momentum of initial gamma quantum (in MRS);
    // Output: pe1 - momentum of 1 created electron (in MRS);
    //         pe2 - momentum of 2 created electron (in MRS).
    // Which particle is e+ and which e-, should be selected randomly.
    // VIT, 11.03.1998 from GEANT subroutines GPAIRG of G.Patrick, L.Urban
    // and GBTETH of L.Urban.
    void decay0_pairext1(mygsl::rng & prng, double Z,
                         const double *pg0, double *pe1, double *pe2);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_PAIREXT1_H_
//
// Local Variables: --
// mode: c++ --
// End: --
