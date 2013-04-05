// -*- mode: c++ ; -*- 
/* stepping_verbose.h
 */

#ifndef MCTOOLS_G4_STEPPING_VERBOSE_H_
#define MCTOOLS_G4_STEPPING_VERBOSE_H_ 1

#include <G4SteppingVerbose.hh>

namespace mctools {

  namespace g4 {
  
    class stepping_verbose: public G4SteppingVerbose 
    {
    public:

      /// Constructor
      stepping_verbose ();

      /// Destructor
      virtual ~stepping_verbose ();

      // Geant4 interface :

      void StepInfo ();

      void TrackingStarted ();

    };

  } // end of namespace g4

} // end of namespace snemo

#endif // MCTOOLS_G4_STEPPING_VERBOSE_H_

// end of stepping_verbose.h
