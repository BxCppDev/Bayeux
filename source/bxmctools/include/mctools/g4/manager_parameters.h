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

#include <string>
#include <iostream>
#include <boost/cstdint.hpp>

namespace mctools {

  namespace g4 {

    class manager;

    struct manager_parameters
    {
      std::string logging;
      std::string manager_config_filename;
      std::string g4_macro;
      bool        g4_visu;
      bool        interactive;
      uint32_t    number_of_events;
      uint32_t    number_of_events_modulo;
      int         mgr_seed;
      std::string input_prng_states_file;
      std::string output_prng_states_file;
      int         prng_states_save_modulo;
      std::string input_prng_seeds_file;
      std::string output_prng_seeds_file;
      std::string output_data_file;
      std::string vg_name;
      int         vg_seed;
      std::string eg_name;
      int         eg_seed;
      int         shpf_seed;
      bool        using_time_stat;
      bool        forbid_private_hits;
      bool        dont_save_no_sensitive_hit_events;
      bool        use_run_header_footer;

    public:

      manager_parameters ();

      void set_defaults ();

      void reset ();

      static void print_examples (std::ostream      & a_out   = std::clog,
                                  const std::string & a_name  = "sng4_production",
                                  const std::string & a_title = "");

      static void setup (const manager_parameters & a_params,
                         manager & a_manager);

    }; // end of class manager_parameters

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_MANAGER_PARAMETERS_H_

// end of manager_parameters.h
