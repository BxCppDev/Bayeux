/* simulation_module.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2012-04-10
 *
 * Description:
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_G4_SIMULATION_MODULE_H_
#define MCTOOLS_G4_SIMULATION_MODULE_H_ 1

// Load the abstract module interface :
#include <dpp/base_module.h>

// Load useful macros for module :
#include <dpp/module_macros.h>

#include <mctools/g4/manager_parameters.h>

namespace geomtools {
  // Forward declaration :
  class manager;
}

namespace mctools {

  // Forward declaration :
  class simulated_data;

  namespace g4 {

    class manager;
    class simulation_ctrl;

    /// \brief The Geant4 simulation module
    DPP_MODULE_CLASS_DECLARE(simulation_module)
    {

    public:

      void set_geometry_manager (const geomtools::manager & geometry_manager_);

      /// Constructor
      simulation_module (datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~simulation_module ();

      // This macro setup the module standard interface (initialize/reset/process) :
      DPP_MODULE_INTERFACE ();

    protected :

      void _initialize_manager (datatools::service_manager & smgr_);

      void _terminate_manager ();

      int _simulate_event (datatools::things & the_event_record);

    private:

      std::string _Geo_label_;     //!< The label of the Geometry service to be accessed
      std::string _SD_label_;      //!< The label of the 'simulated_data' bank in the event record
      bool _erase_former_SD_bank_; //!< A flag to erase former 'simulated data' bank from the event record

      manager_parameters _simulation_manager_params_; //!< The configuration parameters
                                                      // for the GEANT4 simulation manager

      const geomtools::manager * _geometry_manager_;   //!< Non-mutable reference to the geometry manager

      manager                  * _simulation_manager_; //!< The embedded GEANT4 simulation manager
      simulation_ctrl          * _simulation_ctrl_;    //!< The embedded control object for thread synchronization

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(simulation_module);

    };

  }  // end of namespace g4

}  // end of namespace mctools

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::simulation_module)

#endif // MCTOOLS_G4_SIMULATION_MODULE_H_

// end of simulation_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
