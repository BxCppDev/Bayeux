#ifndef GENBB_DECAY0_P32_H_
#define GENBB_DECAY0_P32_H_ 1

#include <cmath>

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    void P32(mygsl::rng & prng_, primary_event & event_, double tcnuc, double & tdnuc);


  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_P32_H_
// Local Variables: --
// mode: c++ --
// End: --
