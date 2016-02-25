// -*- mode: c++; -*-
// ex01_inspector.h

#ifndef MCTOOLS_EX01_INSPECTOR_H
#define MCTOOLS_EX01_INSPECTOR_H

// Third party:
// - Bayeux/geomtools:
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

namespace mctools {

  namespace ex01 {

    /// \brief A class that inspects and optionaly displays the Monte Carlo simulated event
    class simulated_data_inspector {

    public:

      /// Default constructor
      simulated_data_inspector();

      /// Destructor
      void set_interactive(bool);

      /// Set the flag to activate the dump of each simulated data record
      void set_dump_simulated_data(bool);

      /// Set the flag to activate the dump of Monte Carlo hits
      void set_dump_hits(bool);

      /// Set the flag to activate visualization
      void set_with_visualization(bool);

      /// Set the geometry manager
      void set_geometry_manager(const geomtools::manager &);

      /// Inspect a simulation data record
      bool inspect(const mctools::simulated_data & sd_);

      /// Display a simulation data record
      void display(const mctools::simulated_data & sd_);

    private:

      bool _interactive_; ///< Interactive session flag
      bool _dump_simulated_data_; ///< Flag to activate the dump of each simulated data record
      bool _dump_hits_;  ///< Flag to activate the dump of Monte Carlo hits
      bool _with_visualization_; ///< Flag to activate visualization
      const geomtools::manager * _geometry_manager_; ///< Handle to the geometry manager

    };

  } // namespace ex01

} // namespace mctools

#endif // MCTOOLS_EX01_INSPECTOR_H
