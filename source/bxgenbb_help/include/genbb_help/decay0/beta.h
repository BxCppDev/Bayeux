#ifndef GENBB_DECAY0_BETA_H_
#define GENBB_DECAY0_BETA_H_ 1

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {
    
    /// \brief Subroutine decay0_beta simulates the angles and energy of beta particles emitted in beta decay of nucleus. 
    /// The decay is considered as allowed.
    /// Only Coulomb correction to the shape of energy spectrum is taken
    /// into consideration.
    // Input : Qbeta    - beta energy endpoint (MeV; Qbeta>50 eV);
    //         Zdtr     - atomic number of daughter nucleus (Zdtr>0 for e- and
    //                    Zdtr<0 for e+ particles);
    //         tcnuc - time of creation of nucleus (sec);
    //         thnuc - nucleus halflife (sec).
    // Output: tdnuc - time of decay of nucleus (sec);
    // VIT, 30.07.1992; 15.10.1995.
    void decay0_beta(mygsl::rng & prng_, primary_event & event_,
                     double Qbeta_, double Zdtr_,
                     double tcnuc_, double thnuc_, double & tdnuc_);

    void decay0_beta(mygsl::rng & prng_, primary_event & event_, 
                     double tcnuc_, double thnuc_, double & tdnuc_, 
                     void * params_);


  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_BETA_H_
// 
// Local Variables: --
// mode: c++ --
// End: --
