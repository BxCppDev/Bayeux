#ifndef GENBB_DECAY0_COMMON_H_
#define GENBB_DECAY0_COMMON_H_ 1

// Standard library:
#include <cmath>
#include <string>

namespace genbb {
  namespace decay0 {

    typedef double (*func_type)(double, void*);

    typedef void (*fsub1_type)(int,
			       const double*,
			       double*,
			       double*,
			       void*);

    typedef void (*fsub2_type)(int,
			       const double*,
			       double*,
			       double*,
			       void*);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_COMMON_H_
