#ifndef GENBB_DECAY0_FE1_MODS_H_
#define GENBB_DECAY0_FE1_MODS_H_ 1

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    /// \brief Probability distribution for energy of first e-/e+ for modebb=1
    double decay0_fe1_mod1(double e1, void * params);

    /// \brief Probability distribution for energy of first e-/e+ for modebb=2
    double decay0_fe1_mod2(double e1, void * params);

    /// \brief Probability distribution for energy of first e-/e+ for modebb=3
    // (0nu2b with rhc, 0+ -> 0+ or 0+ -> 2+, N* mechanism)
    double decay0_fe1_mod3(double e1, void * params);
  
    /// \brief Probability distribution for energy of first e-/e+ for modebb=7
    double decay0_fe1_mod7(double e1, void * params);

    /// \brief Probability distribution for energy of first e-/e+ for modebb=10
    double decay0_fe1_mod10(double e1, void * params);

    /// \brief Probability distribution for energy of first e-/e+ for modebb=17
    double decay0_fe1_mod17(double e1, void * params);

    /// \brief Probability distribution for energy of first e-/e+ for modebb=18
    double decay0_fe1_mod18(double e1, void * params);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_GAUSS_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


