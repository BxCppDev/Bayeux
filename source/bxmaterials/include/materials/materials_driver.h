/// \file materials/materials_driver.h
/* Author(s)    : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-08-23
 * Last modified: 2014-06-11
 *
 * Description:  Isotopes/elements/materials application driver.
 */

#ifndef MATERIALS_MATERIALS_DRIVER_H
#define MATERIALS_MATERIALS_DRIVER_H 1

// Standard Library
#include <iostream>
#include <string>
#include <vector>

// Third Party
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

/// The namespace of the Bayeux/materials module library
namespace materials {

  // Forward declaration
  class manager;

  /// \brief Parameters for the material driver
  struct materials_driver_params
  {
  public:

    /// \brief Supported actions
    enum action_flag_type {
      ACTION_UNDEFINED      = -1,
      ACTION_LIST_ISOTOPES  =  0x1,
      ACTION_LIST_ELEMENTS  =  0x2,
      ACTION_LIST_MATERIALS =  0x4,
      ACTION_LIST_ALIASES   =  0x8,
      ACTION_SHOW_ISOTOPE   =  0x10,
      ACTION_SHOW_ELEMENT   =  0x20,
      ACTION_SHOW_MATERIAL  =  0x40,
      ACTION_DUMP_MANAGER   =  0x1000,
    };

    /// Default constructor
    materials_driver_params();

    /// Destructor
    virtual ~materials_driver_params();

    /// Reset
    void reset();

    /// Basic print
    void dump(std::ostream & = std::clog) const;

  public:

    int         action;
    bool        with_decoration;
    std::vector<std::string> action_options;
    std::string show_target;
    std::vector<std::string> LL_dlls;
    std::string LL_config;
    std::string MaterialsMgrConfigFile;
    datatools::logger::priority logging;

  };

  /// \brief Driver for materials
  class materials_driver
  {
  public:

    /// Constructor
    materials_driver();

    /// Destructor
    virtual ~materials_driver();

    /// Test initialization status
    bool is_initialized() const;

    /// Initialization
    void initialize(const materials_driver_params & mdp_);

    /// reset
    void reset();

    /// Main run method
    int run_action(std::ostream & out_ = std::cout,
                   std::ostream & log_ = std::clog);

  private:

    bool _initialized_;               //!< Initialization flag
    materials_driver_params _params_; //!< Configuration parameters
    boost::scoped_ptr<manager> _mgr_; //!< Embeded manager

  };

} // end of namespace materials

#endif // MATERIALS_MATERIALS_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
