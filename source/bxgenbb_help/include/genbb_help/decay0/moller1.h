#ifndef GENBB_DECAY0_MOLLER1_H_
#define GENBB_DECAY0_MOLLER1_H_ 1

namespace mygsl {
  class rng;
}

namespace genbb {

  namespace decay0 {

    /// \brief Simulates electron-electron Moller scattering.
    //         dimension pe0(3),pe1(3),pe2(3)
    // Input : pe0   - momentum of initial electron (in MRS);
    //         dcute - minimal energy of delta ray to be borned;
    //                 if energy of initial electron < dcute/2
    //                 delta rays are not produced;
    // Output: pe1   - momentum of 1 scattered electron (in MRS);
    //         pe2   - momentum of 2 scattered electron (in MRS).
    // VIT, 07.12.1995 from GEANT subroutine GDRAY by D.Ward, L.Urban.
    void decay0_moller1(mygsl::rng & prng, 
                        double dcute,
                        const double * pe0,
                        double * pe1, double * pe2);
    
  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_MOLLER1_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


