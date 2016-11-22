/// \file genvtx/genvtx_driver.h
/// Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>

#ifndef GENVTX_GENVTX_DRIVER_H
#define GENVTX_GENVTX_DRIVER_H 1

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Third party
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

namespace geomtools {
  class manager;
}

namespace mygsl {
  class rng;
}

/// Top-level namespace of the Bayeux/genvtx module library
namespace genvtx {

  class manager;

  /// \brief Genvtx driver configuration parameters
  struct genvtx_driver_params
  {
    /// Default constructor
    genvtx_driver_params();

    /// Reset
    void reset();

    /// Rough print
    void dump(std::ostream & = std::clog) const;

    /// Check
    void check() const;

  protected:

    /// Set default values
    void _set_defaults();

  public:
    std::string logging_label;        ///< Logging priority level
    std::vector<std::string> LL_dlls; ///< List of DLL to be loaded (library loader)
    std::string LL_config;            ///< Configuration file for the library loader
    std::string GeoMgrConfigFile;     ///< Geometry manager main configuration file
    std::string VGMgrConfigFile;      ///< Vertex generator manager main configuration file

    bool        action_list = false;  ///< Flag to list available vertex generators
    std::string generator_name;       ///< Selected vertex generator name
    bool        action_show = false;  ///< Flag to print informations about the selected vertex generator

    bool        action_shoot = false; ///< Flag to shoot vertexes from the selected vertex generator
    std::string prng_type;       ///< Type of the PRNG
    int         prng_seed;       ///< Seed used to initialize the PRNG
    int         nshoots;         ///< Number of shoots
    int         shoot_modulo;    ///< Modulo on vertex number (progress bar)
    std::string VtxOutputFile;   ///< Output file where to store generated vertexes
    std::string VtxInputFile;    ///< Input file from which to load generated vertexes

    bool        action_visu = false;  ///< Flag to activate visualization
    std::string visu_object;     ///< Name of the 3D object to focus on
    int         visu_max_counts; ///< Max number of displayed vertexes
    double      visu_spot_zoom;  ///< Display zoom factor
    double      visu_spot_size;  ///< Display splot size
    std::string visu_spot_color; ///< Display splot color

    bool        action_visu_store_dd = false; ///< Flag to store generated vertexes as a display data objects
    std::string visu_store_dd_out;            ///< Output file where to store generated vertexes as display data object

  };

  /// \brief The genvtx driver
  class genvtx_driver {
  public:

    /// \brief Action activation bits
    enum action_flag_type {
      ACTION_UNDEFINED = -1,
      ACTION_NONE      =  0,
      ACTION_LIST      =  0x1,
      ACTION_SHOW      =  0x2,
      ACTION_SHOOT     =  0x4,
      ACTION_VISU      =  0x8,
      ACTION_VISU_STORE_DD = 0x10
    };

    /// Default constructor
    genvtx_driver();

    /// Constructor
    genvtx_driver(const genvtx_driver_params &);

    /// Destructor
    ~genvtx_driver();

    /// Check initialization status
    bool is_initialized() const;

    /// Load the configuration parameters:
    void setup(const genvtx_driver_params &);

    /// Initialize
    void initialize();

    /// Initialize
    void initialize(const genvtx_driver_params &);

    /// Reset
    void reset();

    /// Run the driver
    void run();

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Return the configuration parameter
    const genvtx_driver_params & get_params() const;

  private:

    bool _initialized_ = false;                    ///< Initialization flags
    genvtx_driver_params _params_;                 ///< Configuration parameters
    datatools::logger::priority _logging_;         ///< Logger priority threshold
    uint32_t _action_ = 0;                         ///< Action bitset
    std::unique_ptr<geomtools::manager> _geo_mgr_; ///< Geometry manager handle
    std::unique_ptr<mygsl::rng> _prng_;            ///< PRNG handle
    std::unique_ptr<genvtx::manager> _vtx_mgr_;    ///< Vertex generator manager handle

  };

} // end of namespace genvtx

#endif // GENVTX_GENVTX_DRIVER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
