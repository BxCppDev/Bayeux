/// \file mctools/simulation_ctrl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-06-03
 *
 * Description:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_G4_SIMULATION_CTRL_H
#define MCTOOLS_G4_SIMULATION_CTRL_H 1

// Standard library:
#include <thread>
#include <mutex>
#include <cstdint>
#include <memory>
#include <condition_variable>

// Third party:

// This project:
#include <mctools/g4/loggable_support.h>

namespace mctools {
  namespace g4 {

    // Forward declarations
    class manager;
    class simulation_run;

    /** Object that hosts control parameters and data structures (thread/mutex/condition)
     *  for a multi-threaded running of the G4 simulation manager (simulation module)
     */
    class simulation_ctrl
      : public loggable_support
    {
    public :
      enum event_availability_status_type
        {
         NOT_AVAILABLE_FOR_G4 = 0,
         AVAILABLE_FOR_G4     = 1,
         ABORT                = 2
        };

    public :
      /// Constructor
      simulation_ctrl(manager & simulation_manager_, uint32_t max_counts_ = 0);

      /// Destructor
      ~simulation_ctrl() override;

      /// Set the Geant4 simulation manager
      void set_simulation_manager(manager & simulation_manager_);

      /// Start
      void start();

      /// Start the simulation run
      /// As it's run inthe thread, shouldn't it be private?
      void start_simulation_run();

      /// Set the requested stop flag
      void set_stop_requested();

      /// Check the requested stop flag
      bool is_stop_requested() const;

    public:
      
      // Why are these public ? Because event_action class needs to access to
      // these thread internals (however we could provide inline public accessors...):
      manager        * simulation_manager = nullptr;
      std::uint32_t    max_counts = 0;

      // Thread management:
      std::unique_ptr<std::thread> simulation_thread; 
      std::unique_ptr<std::mutex> event_mutex; 
      std::unique_ptr<std::condition_variable> event_available_condition; 
      int                event_availability_status = NOT_AVAILABLE_FOR_G4;
      bool               stop_requested = false;
      std::uint32_t      counts = 0;
      
    };
  }  // end of namespace g4
}  // end of namespace mctools

#endif // MCTOOLS_G4_SIMULATION_CTRL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
