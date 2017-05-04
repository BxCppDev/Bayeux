#ifndef GENBB_DECAY0_TSIMPR_H_
#define GENBB_DECAY0_TSIMPR_H_ 1

// This project:
#include <genbb_help/decay0/common.h>

namespace genbb {
  namespace decay0 {

    // Function decay0_tsimpr calculates the value of integral of function f from a to b
    // with step h using Simpson's formula.
    // (b-a)/h must be equal 2*m where m is integer.
    // Function f must be decribed as external in main program.
    // VIT, DLP KINR, 20.11.1984.
    double decay0_tsimpr(func_type f, double a, double b, double h,
                         void * params);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_TSIMPR_H_
//
// Local Variables: --
// mode: c++ --
// End: --
