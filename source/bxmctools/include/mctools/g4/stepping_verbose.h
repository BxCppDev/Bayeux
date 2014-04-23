/// \file mctools/stepping_verbose.h

#ifndef MCTOOLS_G4_STEPPING_VERBOSE_H
#define MCTOOLS_G4_STEPPING_VERBOSE_H 1

// Third party:
// - Geant4:
#include <G4SteppingVerbose.hh>

namespace mctools {

  namespace g4 {

    /// \brief Stepping verbose support using the Geant4 interface
    class stepping_verbose: public G4SteppingVerbose
    {
    public:

      /// Constructor
      stepping_verbose ();

      /// Destructor
      virtual ~stepping_verbose ();

      /// Geant4 interface
      void StepInfo ();

      /// Geant4 interface
      void TrackingStarted ();

    };

  } // end of namespace g4

} // end of namespace snemo

#endif // MCTOOLS_G4_STEPPING_VERBOSE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
