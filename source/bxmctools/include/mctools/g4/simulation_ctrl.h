/* simulation_ctrl.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-03-09
 *
 * Description:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_G4_SIMULATION_CTRL_H_
#define MCTOOLS_G4_SIMULATION_CTRL_H_ 1

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/cstdint.hpp>

namespace mctools {

  namespace g4 {

    class manager;

    class simulation_run;

    /** Object that hosts control parameters and data structures (thread/mutex/condition)
     * for a multi-threaded running of the G4 simulation manager (simulation module)
     */
    class simulation_ctrl
    {
    public :

      static bool g_devel;

      enum event_availability_status_type
        {
          NOT_AVAILABLE_FOR_G4 = 0,
          AVAILABLE_FOR_G4     = 1,
          ABORT                = 2
        };

      manager          * simulation_manager;
      boost::thread    * simulation_thread;
      boost::mutex     * event_mutex;
      boost::condition * event_available_condition;
      int                event_availability_status;
      bool               stop_requested;
      uint32_t           counts;
      uint32_t           max_counts;

    public :

      void set_stop_requested ();

      bool is_stop_requested () const;

      void set_simulation_manager (manager & simulation_manager_);

      // ctor:
      simulation_ctrl (manager & simulation_manager_, uint32_t max_counts_ = 0);

      // dtor:
      ~simulation_ctrl ();

      void start ();

      void start_simulation_run ();

    };

  }  // end of namespace g4

}  // end of namespace mctools

#endif // MCTOOLS_G4_SIMULATION_CTRL_H_

// end of simulation_ctrl.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
