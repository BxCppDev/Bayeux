#ifndef GENBB_DECAY0_COMPTON1_H_
#define GENBB_DECAY0_COMPTON1_H_ 1

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    /// \brief Simulates photon-electron Compton scattering.
    // Call  : 
    //         dimension pg0(3),pg1(3),pe1(3)
    //         call compton1(pg0,pg1,pe1)
    // Input : pg0 - momentum of initial gamma quantum (in MRS);
    // Output: pg1 - momentum of scattered gamma quantum (in MRS);
    //         pe1 - momentum of recoil electron (in MRS).
    // 
    // VIT, 07.12.1995 from GEANT subroutine GCOMP by G.Patrick, L.Urban.
    void decay0_compton1(mygsl::rng & prng, 
                         const double * pg0, double *pg1, double *pe1);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_COMPTON1_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


