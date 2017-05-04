#ifndef GENBB_DECAY0_FUNBETA_1FU_H_
#define GENBB_DECAY0_FUNBETA_1FU_H_ 1

// This project:
#include <genbb_help/decay0/funbeta1.h>

namespace genbb {

  namespace decay0 {

    struct bj69sl2 : public parbeta1
    {
      double sl2[48];
    };

    /// \brief Function to search the maximum of beta spectrum curve by decay0_tgold subroutine.
    // VIT, 26.10.2006.
    double decay0_funbeta_1fu(double E_, void * params_);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_FUNBETA_1FU_H_
//
// Local Variables: --
// mode: c++ --
// End: --
