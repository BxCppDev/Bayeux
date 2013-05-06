#ifndef GENBB_DECAY0_PARTICLE_H_
#define GENBB_DECAY0_PARTICLE_H_ 1

#include <string>

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    enum decay0_particle_type
      {
        GAMMA          = 1,  // gamma
        POSITRON       = 2,  // positron
        ELECTRON       = 3,  // electron
        NEUTRINO       = 4,  // neutrino
        MUONPLUS       = 5,  // muon+
        MUONMINUS      = 6,  // muon-
        PION0          = 7,  // pion0 
        PIONPLUS       = 8,  // pion+ 
        PIONMINUS      = 9,  // pion-
        KAON0LONG      = 10, // kaon0 long
        KAONPLUS       = 11, // kaon+ 
        KAONMINUS      = 12, // kaon-
        NEUTRON        = 13, // neutron 
        PROTON         = 14, // proton
        ANTIPROTON     = 15, // antiproton
        KAON0SHORT     = 16, // kaon0 short
        ETA            = 17, // eta
        LAMBDA         = 18, // lambda
        SIGMAPLUS      = 19, // sigma+ 
        SIGMA0         = 20, // sigma0 
        SIGMAMINUS     = 21, // sigma-
        XI0            = 22, // xi0 
        XIMINUS        = 23, // xi-
        OMEGA          = 24, // omega
        ANTINEUTRON    = 25, // antineutron
        ANTILAMBDA     = 26, // antilambda
        ANTISIGMAMINUS = 27, // antisigma-
        ANTISIGMA0     = 28, // antisigma0
        ANTISIGMAPLUS  = 29, // antisigma+ 
        ANTIXI0        = 30, // antixi0
        ANTIXIPLUS     = 31, // antixi+ 
        ANTIOMEGAPLUS  = 32, // antiomega+
        DEUTERON       = 45, // deuteron
        TRITIUM        = 46, // tritium
        ALPHA          = 47, // alpha 
        GEANTINO       = 48, // geantino
        HE3            = 49, // He3 
        CERENKOV       = 50, // Cerenkov
      };

    /// Return the name of the particle with given ID (MeV)
    const std::string decay0_geant3_particle_name(int np);

    /// Return the mass of the particle with given ID (MeV)
    double decay0_particle_mass(int np);

    double decay0_emass();

    /// \brief Generation of isotropical emission of particle in the range of energies and angles.
    // Input : np          - GEANT number for particle identification:
    //                        1 - gamma         2 - positron     3 - electron
    //                        4 - neutrino      5 - muon+        6 - muon-
    //                        7 - pion0         8 - pion+        9 - pion-
    //                       10 - kaon0 long   11 - kaon+       12 - kaon-
    //                       13 - neutron      14 - proton      15 - antiproton
    //                       16 - kaon0 short  17 - eta         18 - lambda
    //                       19 - sigma+       20 - sigma0      21 - sigma-
    //                       22 - xi0          23 - xi-         24 - omega
    //                       25 - antineutron  26 - antilambda  27 - antisigma-
    //                       28 - antisigma0   29 - antisigma+  30 - antixi0
    //                       31 - antixi+      32 - antiomega+  45 - deuteron
    //                       46 - tritium      47 - alpha       48 - geantino
    //                       49 - He3          50 - Cerenkov;
    //         E1,E2       - range of kinetic energy of particle (MeV);
    //         teta1,teta2 - range of teta angle (radians);
    //         phi1,phi2   - range of phi  angle (radians);
    //         tclev       - time of creation of level from which particle will be
    //                       emitted (sec);
    //         thlev       - level halflife (sec).
    // Output: tdlev       - time of decay of level (sec);
    // VIT, 15.10.1995.
    void decay0_particle(mygsl::rng & prng, primary_event & event,
                         int np, double E1, double E2,
                         double teta1, double teta2,
                         double phi1, double phi2,
                         double tclev, double thlev, double & tdlev);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_PARTICLE_H_
// 
// Local Variables: --
// mode: c++ --
// End: --

