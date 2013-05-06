#ifndef GENBB_DECAY0_GDROT_H_
#define GENBB_DECAY0_GDROT_H_ 1

#include <cmath>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

namespace mygsl {
  class rng;
}

namespace genbb {
  namespace decay0 {

    /// \brief Rotates vector from one reference system into another.
    // Theta and phi are anti-clockwise Eulerian angles between the
    // two systems.
    // M.Hansroul, G.Patrick (GEANT).
    void decay0_gdrot(double * p,
                      double costh,double sinth,double cosph,double sinph);
 
  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_GDROT_H_
// 
// Local Variables: --
// mode: c++ --
// End: --


