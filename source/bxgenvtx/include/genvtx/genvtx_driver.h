/// \file genvtx/genvtx_driver.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef GENVTX_GENVTX_DRIVER_H
#define GENVTX_GENVTX_DRIVER_H 1

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Third party
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
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

  struct genvtx_driver_params
  {
    /// Default constructor
    genvtx_driver_params();

    /// Reset
    void reset();

    /// Rough print
    void dump() const;

    /// Check
    bool check() const;

  public:
    bool        help;
    std::string logging_label;
    std::vector<std::string> LL_dlls;
    std::string LL_config;
    std::string GeoMgrConfigFile;
    std::string VGMgrConfigFile;

    bool        action_list;

    std::string generator_name;
    bool        action_show;

    bool        action_shoot;
    std::string prng_type;
    int         prng_seed;
    int         nshoots;
    std::string VtxOutputFile;
    std::string VtxInputFile;

    bool        action_visu;
    std::string visu_object;
    int         visu_max_counts;
    double      visu_spot_zoom;
    double      visu_spot_size;
    std::string visu_spot_color;

    bool        action_visu_store_dd;
    std::string visu_store_dd_out;

  };

  /// \brief The genvtx driver
  class genvtx_driver {
  public:

    /// \brief User interface access class (driver's friend)
    class ui_access {
     public:
      ui_access(genvtx_driver &);
      genvtx_driver_params & params();
      genvtx_driver & driver();
     private:
      genvtx_driver * _driver_;
    };

    friend class ui_access; // explicit friendship

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

    /// Reset
    void reset();

    /// Run the driver
    void run();

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Return the configuration parameter
    const genvtx_driver_params & get_params() const;

  private:

    genvtx_driver_params _params_;         ///< Configuration parameters
    bool _initialized_;                    ///< Initialization flags
    datatools::logger::priority _logging_; ///< Logger priority threshold
    uint32_t _action_;                     ///< Action bit set
    boost::scoped_ptr<geomtools::manager> _geo_mgr_; ///< Geometry manager handle
    boost::scoped_ptr<mygsl::rng> _prng_;            ///< PRNG handle
    boost::scoped_ptr<genvtx::manager> _vtx_mgr_;    ///< Vertex generator manager handle

  };

} // end of namespace genvtx

#endif // GENVTX_GENVTX_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
