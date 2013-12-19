// -*- mode: c++ ; -*-
/* manager_parameters.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-16
 * Last modified: 2013-06-03
 *
 * License:
 *
 * Description:
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_MANAGER_PARAMETERS_H_
#define MCTOOLS_G4_MANAGER_PARAMETERS_H_ 1

// Standard library:
#include <string>
#include <iostream>
#include <vector>

// Third party:
// - Boost
#include <boost/cstdint.hpp>

namespace mctools {

  namespace g4 {

    class manager;

    /// \brief The set of parameters for the Geant4 simulation manager
    struct manager_parameters
    {
      bool        interactive; /// Flag to activate the interactive mode
      std::string g4_macro; /// Name of a Geant4 macro to be executed
      bool        g4_visu; /// Flag to activate Geant4 visulization

      std::string logging; /// Logging priority threshold label
      std::vector<std::string> dlls; /// List of DLLs to be loaded
      std::string dll_loader_config; /// Configuration file of the DLL loader
      std::string manager_config_filename; /// Main manager configuration file
      uint32_t    number_of_events; /// Number of simulated event
      uint32_t    number_of_events_modulo; /// Number of events modulo
      int         mgr_seed; /// Seed for the Geant4 engine's PRNG
      std::string input_prng_states_file;
      std::string output_prng_states_file;
      int         prng_states_save_modulo; /// Period saving the PRNG's internal states
      std::string input_prng_seeds_file;
      std::string output_prng_seeds_file;
      std::string output_data_file; /// Name of the output data file
      std::string vg_name; /// Name of the vertex generator
      int         vg_seed; /// Seed for the vertex generator's PRNG
      std::string eg_name; /// Name of the primary event generator
      int         eg_seed; /// Seed for the primary event generator's PRNG
      int         shpf_seed; /// Seed for the hit post-processing factory
      bool        using_time_stat; /// Print time statistic at the end of simulation run
      bool        forbid_private_hits; /// Do not save 'private' MC hits at the end of the simulated event
      bool        dont_save_no_sensitive_hit_events; /// Do not save 'no sensitive' MC hits
      bool        use_run_header_footer; /// Add a header/footer in the output data file

    public:

      /// Default constructor
      manager_parameters();

      /// Set default values
      void set_defaults();

      /// Reset
      void reset();

      /// Print
      void print(std::ostream &) const;

      /// Setup a simulation manager from a set of parameters
      static void setup(const manager_parameters & a_params,
                        manager & a_manager);

    }; // end of class manager_parameters

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_MANAGER_PARAMETERS_H_

// end of manager_parameters.h
