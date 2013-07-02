// -*- mode: c++ ; -*-
/* event_action.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user event action class
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_EVENT_ACTION_H_
#define MCTOOLS_G4_EVENT_ACTION_H_ 1

#include <mctools/simulated_data.h>
#include <mctools/base_step_hit_processor.h>

#include <mctools/g4/loggable_support.h>

// G4 stuff:
#include <G4UserEventAction.hh>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    class run_action;
    class detector_construction;

    class event_action : public G4UserEventAction,
                         public loggable_support
    {
    public:

      typedef ::mctools::simulated_data sim_data_type;

      bool is_initialized () const;

      bool is_aborted_event () const;

      void set_aborted_event (bool = true);

      void set_external_event_data (sim_data_type & a_external_event_data);

      const sim_data_type & get_event_data () const;

      sim_data_type & grab_event_data ();

      const run_action & get_run_action () const;

      run_action & grab_run_action ();

      /// Constructor
      event_action (run_action & a_run_action, const detector_construction & a_dctor);

      /// Destructor
      virtual ~event_action ();

      void initialize (const ::datatools::properties & a_config);

      void reset ();

      // Geant4 interface :

      void BeginOfEventAction (const G4Event*);

      void EndOfEventAction (const G4Event*);

    private:

      void _at_init_ ();

      void _at_reset_ ();

    private:

      bool                          _initialized_;
      const detector_construction * _detector_construction_;
      run_action *                  _run_action_;
      sim_data_type                 _event_data_;
      sim_data_type               * _external_event_data_;
      bool                          _aborted_event_;
      ::mctools::base_step_hit_processor::step_hit_ptr_collection_type _phits_;

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::event_action)

#endif // MCTOOLS_G4d_EVENT_ACTION_H_

// end of event_action.h
