#ifndef GENBB_DECAY0_FE12_MODS_H_
#define GENBB_DECAY0_FE12_MODS_H_ 1

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=4 (as tsimpr needs)
    // (2nu2b, 0+ -> 0+, 2n mechanism)
    double decay0_fe12_mod4(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=5 (as tsimpr needs)
    // (0nu2b with Gelmini-Roncadelli Majoron (spectral index = 1),
    // 0+ -> 0+, 2n mechanism)
    double decay0_fe12_mod5(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=6 (as tsimpr needs)
    // (0nu2b with massive vector etc. Majoron (spectral index = 3),
    // 0+ -> 0+, 2n mechanism)
    double decay0_fe12_mod6(double e2, void * params);
  
    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=8 (as tsimpr needs)
    // (2nu2b, 0+ -> 2+, 2n or N* mechanism)
    double decay0_fe12_mod8(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=13 (as tsimpr needs)
    // (0nu2b with Majoron with spectral index = 7, 0+ -> 0+, 2n mechanism)
    double decay0_fe12_mod13(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=14 (as tsimpr needs)
    // (0nu2b with Mohapatra Majoron with spectral index = 2, 0+ -> 0+, 2n mechanism)
    double decay0_fe12_mod14(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=15 (as tsimpr needs)
    // (2nu2b, 0+ -> 0+, bosonic neutrinos, HSD: A.S.Barabash et al., NPB 783(2007)90,
    // eq. (27a) integrated in Enu1)
    double decay0_fe12_mod15(double e2, void * params);

    /// \brief Two-dimensional probability distribution for energies of e-/e+ for
    // modebb=16 (as tsimpr needs)
    // (2nu2b, 0+ -> 2+, bosonic neutrinos, HSD: A.S.Barabash et al., NPB 783(2007)90,
    // eq. (27b) integrated in Enu1)
    double decay0_fe12_mod16(double e2, void * params);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_GAUSS_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


