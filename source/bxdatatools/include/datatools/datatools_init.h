//! \file   datatools/datatools_init.h
//! \author François Mauger
//! \brief  Describe the datatools API configuration

#ifndef DATATOOLS_DATATOOLS_INIT_H
#define DATATOOLS_DATATOOLS_INIT_H 1

// This project:
#include <datatools/bit_mask.h>

namespace datatools {

  /// \brief datatools initialization flags
  enum datatools_init_flags {
    // Kernel specific flags:
    init_no_help             = datatools::bit_mask::bit00, //!< Inhibit help screen about kernel switches from the CLI terminal
    init_no_splash           = datatools::bit_mask::bit01, //!< Inhibit splash screen from the CLI terminal
    init_no_locale_category  = datatools::bit_mask::bit02, //!< Inhibit local category setting from CLI
    init_no_logging          = datatools::bit_mask::bit03, //!< Inhibit logging level setting from CLI
    init_no_inhibit_libquery = datatools::bit_mask::bit04, //!< Inhibit system library query service
    init_no_libquery_logging = datatools::bit_mask::bit05,
    init_no_inhibit_libinfo  = datatools::bit_mask::bit04, //!< deprecated
    init_no_libinfo_logging  = datatools::bit_mask::bit05, //!< deprecated
    init_no_resource_path    = datatools::bit_mask::bit06,
    init_no_inhibit_variant  = datatools::bit_mask::bit07,
    init_no_variant          = datatools::bit_mask::bit08,
    init_no_inhibit_qt_gui   = datatools::bit_mask::bit09,
    init_no_inhibit_urnquery = datatools::bit_mask::bit10, //!< Inhibit system URN query service
    init_reserved_11         = datatools::bit_mask::bit11, //!< Reserved for future kernel initialization
    init_reserved_12         = datatools::bit_mask::bit12, //!< Reserved for future kernel initialization
    init_reserved_13         = datatools::bit_mask::bit13, //!< Reserved for future kernel initialization
    init_reserved_14         = datatools::bit_mask::bit14, //!< Reserved for future kernel initialization
    // Kernel flags stop here.
    init_kernel_inhibit      = datatools::bit_mask::bit15  //!< Kernel inhibition flag (expert only)
  };

}

#endif // DATATOOLS_DATATOOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
