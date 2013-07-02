// -*- mode: c++ ; -*-
/* stepping_action.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef MCTOOLS_G4_STEPPING_ACTION_H_
#define MCTOOLS_G4_STEPPING_ACTION_H_ 1

#include <boost/cstdint.hpp>

// G4 stuff:
#include <G4UserSteppingAction.hh>

#include <mctools/g4/loggable_support.h>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    class stepping_action : public G4UserSteppingAction,
                            public loggable_support
    {
    public:

      stepping_action ();

      virtual ~stepping_action ();

      void initialize (const datatools::properties & config_);

      // G4 interface:
      void UserSteppingAction (const G4Step *);

    protected:

      virtual void _stepping_action_base (const G4Step *);

    private:

      uint32_t _number_of_steps_;

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::stepping_action)

#endif // MCTOOLS_G4_STEPPING_ACTION_H_

// end of stepping_action.h
