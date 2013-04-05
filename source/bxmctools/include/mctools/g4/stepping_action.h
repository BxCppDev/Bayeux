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

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {
  
    class stepping_action : public G4UserSteppingAction
    {
    public:

      bool is_debug () const;

      void set_debug (bool);
 
      // ctor:
      stepping_action ();

      // dtor:
      virtual ~stepping_action ();
      
      void initialize (const datatools::properties & config_);

      // G4 interface:
      void UserSteppingAction (const G4Step *);

    protected:

      virtual void _stepping_action_base (const G4Step *);

    private: 

      bool     _debug_;
      uint32_t _number_of_steps_;
      
    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_STEPPING_ACTION_H_

// end of stepping_action.h
