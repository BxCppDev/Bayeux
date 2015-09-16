/// \file mctools/g4/simulation_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-12-13
 *
 * Description:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_G4_SIMULATION_MODULE_H
#define MCTOOLS_G4_SIMULATION_MODULE_H 1

// Third party:
// - Bayeux/dpp :
#include <dpp/base_module.h>

// This project:
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

    /// \brief The Geant4 simulation module based on Bayeux/dpp API
    class simulation_module : public dpp::base_module
    {
    public:

      /// Set the geometry service label (only used if no geometry manager is provided)
      void set_geo_label(const std::string &);

      /// Get the geometry service label (only used if no geometry manager is provided)
      const std::string & get_geo_label() const;

      /// Set the SD bank label
      void set_sd_label(const std::string &);

      /// Get the SD bank label
      const std::string & get_sd_label() const;

      /// Set the flag to erased existing SD bank in the current event record
      void set_erase_former_SD_bank(bool e_);

      /// Check if existing SD bank should be erased in the current event record
      bool is_erase_former_SD_bank() const;

      /// Set the reference to some external geoemtry manager
      void set_geometry_manager(const geomtools::manager & geometry_manager_);

      /// Set the simulation manager parameters
      void set_simulation_manager_params(const manager_parameters &);

      /// Return a non mutable reference to the simulation manager parameters
      const manager_parameters & get_simulation_manager_params() const;

      /// Constructor
      simulation_module(datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~simulation_module();

      /// Initialization
      virtual void initialize(const datatools::properties & /* config_ */,
                              datatools::service_manager & /* service_mgr_ */,
                              dpp::module_handle_dict_type & /* modules_map_ */);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual dpp::base_module::process_status process(datatools::things & /* event_record_ */);

    protected :

      void _initialize_manager(datatools::service_manager & /* smgr_ */);

      void _terminate_manager();

      int _simulate_event(datatools::things & /* event_record_ */);

    private:

      std::string _Geo_label_;     //!< The label of the Geometry service to be accessed
      std::string _SD_label_;      //!< The label of the 'simulated_data' bank in the event record
      bool _erase_former_SD_bank_; //!< A flag to erase former 'simulated data' bank from the event record

      manager_parameters _simulation_manager_params_; //!< The configuration parameters for the GEANT4 simulation manager
      const geomtools::manager * _geometry_manager_;   //!< Non-mutable reference to the geometry manager
      manager                  * _simulation_manager_; //!< The embedded GEANT4 simulation manager
      simulation_ctrl          * _simulation_ctrl_;    //!< The embedded control object for thread synchronization

      // Registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(simulation_module);

    };

  }  // end of namespace g4

}  // end of namespace mctools

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::simulation_module)

#endif // MCTOOLS_G4_SIMULATION_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
