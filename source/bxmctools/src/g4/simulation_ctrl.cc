/* simulation_ctrl.cc */

// http://drdobbs.com/cpp/184401518

#include <mctools/g4/simulation_ctrl.h>
#include <mctools/g4/manager.h>
#include <boost/bind.hpp>
#include <cstdlib>

namespace mctools {

  namespace g4 {

    using namespace std;

    bool simulation_ctrl::g_devel = false;

    void simulation_ctrl::set_simulation_manager (manager & a_simulation_manager)
    {
      simulation_manager = &a_simulation_manager;
      return;
    }

    simulation_ctrl::simulation_ctrl (manager & a_simulation_manager,
                                      uint32_t a_max_counts)
    {
      if ( getenv ("MCTOOLS_G4_SIMULATION_CTRL_DEVEL") != NULL )
        {
          simulation_ctrl::g_devel = true;
          cerr << datatools::io::warning
               << "mctools::g4::simulation_ctrl::CTOR: "
               << "Found 'MCTOOLS_G4_SIMULATION_CTRL_DEVEL' environ !"
               << endl;
        }

      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::CTOR: "
             << "Entering..."
             << endl;
      simulation_manager = 0;
      simulation_thread = 0;
      event_mutex = 0;
      event_available_condition = 0;
      event_availability_status = simulation_ctrl::NOT_AVAILABLE_FOR_G4;
      stop_requested = false;
      counts = 0;
      max_counts = a_max_counts;
      set_simulation_manager(a_simulation_manager);
      event_mutex = new boost::mutex;
      event_available_condition = new boost::condition;
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::CTOR: Exiting."
             << endl;
      return;
    }

    simulation_ctrl::~simulation_ctrl ()
    {
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::DTOR: Entering..."
             << endl;
      stop_requested = true;
      if (event_mutex != 0)
        {
          if (g_devel)
            clog << datatools::io::devel
                 << "mctools::g4::simulation_ctrl::DTOR: "
                 << "Acquire the event control lock..."
                 << endl;
          boost::mutex::scoped_lock lock (*event_mutex);
          stop_requested = true;
          event_availability_status = simulation_ctrl::ABORT;
          event_available_condition->notify_one ();
        }
      if (simulation_thread != 0)
        {
          simulation_thread->join ();
        }

      if (event_available_condition != 0)
        {
          delete event_available_condition;
          event_available_condition = 0;
        }
      if (event_mutex != 0)
        {
          delete event_mutex;
          event_mutex = 0;
        }

      if (simulation_thread)
        {
          delete simulation_thread;
          simulation_thread = 0;
        }
      simulation_manager = 0;
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::DTOR: Exiting."
             << endl;
     return;
    }

    void simulation_ctrl::start ()
    {
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start: Entering..."
             << endl;
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start: "
             << "Allocating the simulation thread..."
             << endl;

      simulation_thread = new boost::thread (boost::bind (&simulation_ctrl::start_simulation_run, this));
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start: "
             << "Simulation thread is allocated..."
             << endl;
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start: Exiting."
             << endl;
      return;
    }

    void simulation_ctrl::start_simulation_run ()
    {
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start_simulation_run: Entering..."
             << endl;
      {
        boost::mutex::scoped_lock lock (*event_mutex);
        while (event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4)
          {
            event_available_condition->wait (*event_mutex);
          }
      }
      simulation_manager->run_simulation ();
      if (g_devel)
        clog << datatools::io::devel
             << "mctools::g4::simulation_ctrl::start_simulation_run: Exiting."
             << endl;
      return;
    }

    void simulation_ctrl::set_stop_requested ()
    {
      stop_requested = true;
      return;
    }

    bool simulation_ctrl::is_stop_requested () const
    {
      return stop_requested;
    }

  }  // end of namespace g4

}  // end of namespace mctools

// end of simulation_ctrl.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
