// -*- mode: c++ ; -*-
/* materials_driver.h
 * Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-08-23
 * Last modified: 2013-08-23
 *
 * Description:  Isotopes/elements/materials application driver.
 */

#ifndef MATERIALS_MATERIALS_DRIVER_H_
#define MATERIALS_MATERIALS_DRIVER_H_ 1

// Standard Library
#include <iostream>
#include <string>
#include <vector>

// Datatools
#include <datatools/logger.h>

// Third Party
#include <boost/scoped_ptr.hpp>

/// The namespace of the Bayeux/materials module library
namespace materials {

  class manager;

  struct materials_driver_params {
  public:

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

    materials_driver_params();

    virtual ~materials_driver_params();

    void reset();

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

    bool _initialized_; /// Initialization flag
    materials_driver_params _params_; /// Configuration parameters
    boost::scoped_ptr<manager> _mgr_; /// Embeded manager
  };

} // end of namespace materials

#endif // MATERIALS_MATERIALS_DRIVER_H_

// end of materials_driver.h
