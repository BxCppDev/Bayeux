#ifndef GENBB_DECAY0_FERMI_H_
#define GENBB_DECAY0_FERMI_H_ 1

namespace genbb {

  namespace decay0 {

    /// \brief Approximate A from Z for most nuclei
    double decay0_a_from_z (double z_);

    double decay0_fermi_func_nr_approx (double z_, double e_);

    double decay0_fermi_func_orig (double z_, double e_);

    double decay0_fermi_func (double z_, double e_, bool use_l0_ = false);

    double decay0_fermi_func_shape_only (double z_, double e_);

    // Kinetic energy in MeV :
    double decay0_fermi (double z_, double e_);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_FERMI_H_

// Local Variables: --
// mode: c++ --
// End: --
