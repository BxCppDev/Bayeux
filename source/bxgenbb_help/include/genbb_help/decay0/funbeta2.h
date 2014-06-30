#ifndef GENBB_DECAY0_FUNBETA2_H_
#define GENBB_DECAY0_FUNBETA2_H_ 1

// This project:
#include <genbb_help/decay0/funbeta1.h>

namespace genbb {

  namespace decay0 {

    struct parbeta2 : public parbeta1
    {
      int kf;
    };

    double decay0_funbeta2(double E_, void * params_);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_FUNBETA2_H_
//
// Local Variables: --
// mode: c++ --
// End: --
