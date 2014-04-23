/// \file mctools/stepping_action.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user stepping action class
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_STEPPING_ACTION_H
#define MCTOOLS_G4_STEPPING_ACTION_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Geant4:
#include <G4UserSteppingAction.hh>

// This project:
#include <mctools/g4/loggable_support.h>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    /// \brief Stepping action using the Geant4 interface
    class stepping_action : public G4UserSteppingAction,
                            public loggable_support
    {
    public:

      /// Constructor
      stepping_action ();

      /// Destructor
      virtual ~stepping_action ();

      /// Initialize
      void initialize (const datatools::properties & config_);

      /// Main stepping action for the Geant4 interface
      void UserSteppingAction (const G4Step *);

    protected:

      /// Embedded stepping action
      virtual void _stepping_action_base (const G4Step *);

    private:

      uint32_t _number_of_steps_; /// Counter for steps

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::stepping_action)

#endif // MCTOOLS_G4_STEPPING_ACTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
