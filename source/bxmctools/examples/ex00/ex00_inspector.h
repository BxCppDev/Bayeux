// ex00_inspector.h

#ifndef MCTOOLS_EX00_INSPECTOR_H
#define MCTOOLS_EX00_INSPECTOR_H

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/utils.h>
// - Bayeux/mctools:
#include <bayeux/mctools/simulated_data.h>

namespace mctools {

  namespace ex00 {

    //! \bried Inspector for simulated data with embedded visualization
    class simulated_data_inspector {

    public:

      //! Default constructor
      simulated_data_inspector();

      //! Set the interactive flag
      void set_interactive(bool);

      //! Set the flag for printing simulated data
      void set_dump_simulated_data(bool);

      //! Set the flag for printing hits
      void set_dump_hits(bool);

      //! Set the flag for visualization
      void set_with_visualization(bool);

      //! Set the POIs file
      void set_pois(const std::string & pois_);

      //! Set the reference to an external geometry manager
      void set_geometry_manager(const geomtools::manager &);

      //! Inspection of a simulated data record
      bool inspect(const mctools::simulated_data & sd_);

      //! Visualization of a simulated data record
      void display(const mctools::simulated_data & sd_);

    private:

      // Management
      bool _interactive_;         //!< Interactive flag

      // Configuration parameters:
      bool _dump_simulated_data_; //!< Print data flag
      bool _dump_hits_;           //!< Print hits flag
      bool _with_visualization_;  //!< Visualization flag
      // External resources:
      const geomtools::manager * _geometry_manager_; //!< Handle to an external geometry manager
      std::string _pois_;         //!< File with points of interest (POIs)
      std::vector<geomtools::vector_3d> _poi_positions_; //!< Array of 3D positiosn of POIs

    };

  } // namespace ex00

} // namespace mctools

#endif // MCTOOLS_EX00_INSPECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
