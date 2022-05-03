// simulation_ctrl.cc

// http://drdobbs.com/cpp/184401518

// Ourselves:
#include <mctools/g4/simulation_ctrl.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/bind.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <mctools/g4/manager.h>

namespace mctools {
  namespace g4 {
    
    simulation_ctrl::simulation_ctrl(manager & a_simulation_manager,
                                     uint32_t a_max_counts)
    {
      simulation_manager = nullptr;
      event_availability_status = simulation_ctrl::NOT_AVAILABLE_FOR_G4;
      stop_requested = false;
      counts = 0;
      max_counts = a_max_counts;
      set_simulation_manager(a_simulation_manager);
      event_mutex = std::make_unique<std::mutex>(); // = new boost::mutex;
      event_available_condition = std::make_unique<std::condition_variable>();
    }

    simulation_ctrl::~simulation_ctrl()
    {
      stop_requested = true;
      if (event_mutex != nullptr) {
        std::unique_lock<std::mutex> lock(*event_mutex);
        event_availability_status = simulation_ctrl::ABORT;
        event_available_condition->notify_one();
      }
      if (simulation_thread) { 
        simulation_thread->join();
      }
      if (event_available_condition) { 
        event_available_condition.reset();
      }
      if (event_mutex) {
        event_mutex.reset();
      }
      if (simulation_thread) {
        simulation_thread.reset();
      }
      simulation_manager = nullptr;
      return;
    }

    void simulation_ctrl::set_simulation_manager(manager & a_simulation_manager)
    {
      simulation_manager = &a_simulation_manager;
      return;
    }

    void simulation_ctrl::start()
    {
      simulation_thread = std::make_unique<std::thread>(std::bind(&simulation_ctrl::start_simulation_run, this));
      return;
    }

    void simulation_ctrl::start_simulation_run() {
      {
        std::unique_lock<std::mutex> lock(*event_mutex);
        while (event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
          event_available_condition->wait(lock); 
        }
      }
      simulation_manager->run_simulation();
      return;
    }

    void simulation_ctrl::set_stop_requested()
    {
      stop_requested = true;
      return;
    }

    bool simulation_ctrl::is_stop_requested() const
    {
      return stop_requested;
    }

  }  // end of namespace g4
}  // end of namespace mctools

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
