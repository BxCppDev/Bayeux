// -*- mode: c++ ; -*- 
/* tracking_action.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 * 
 * License: 
 * 
 * Description: 
 *
 *   G4 user tracking action class
 * 
 * History: 
 * 
 */

#ifndef MCTOOLS_G4_TRACKING_ACTION_H_
#define MCTOOLS_G4_TRACKING_ACTION_H_ 1

#include <boost/cstdint.hpp>

// G4 stuff:
#include <G4UserTrackingAction.hh>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {
  
    class tracking_action : public G4UserTrackingAction
    {
    public: 

      bool is_debug () const;
 
      void set_debug (bool);
 
      /// Constructor
      tracking_action ();

      /// Destructor
      virtual ~tracking_action ();
      
      void initialize (const datatools::properties & config_);
      
      virtual void PreUserTrackingAction (const G4Track*);

      virtual void PostUserTrackingAction (const G4Track*);
  
    private: 
      
      bool _debug_;
      
    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_TRACKING_ACTION_H_

// end of tracking_action.h
