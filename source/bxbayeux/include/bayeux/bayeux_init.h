//! \file  bayeux/bayeux_init.h
//! \brief Bayeux initialization flags

#ifndef BAYEUX_BAYEUX_INIT_H
#define BAYEUX_BAYEUX_INIT_H

// This project:
#include <datatools/bit_mask.h>
#include <datatools/datatools_init.h>

/// Top-level namespace of the Bayeux library
namespace bayeux {

  /// \brief Bayeux initialization flags
  ///
  /// Bits  0-14 : reserved to the datatools' Kernel initialization
  /// Bits 15    : datatools' Kernel inhibition
  /// Bits 16-31 : unused
  ///
  enum bayeux_init_flags {
    init_dtkernel_no_help             = datatools::init_no_help,
    init_dtkernel_no_splash           = datatools::init_no_splash,
    init_dtkernel_no_locale_category  = datatools::init_no_locale_category,
    init_dtkernel_no_logging          = datatools::init_no_logging,
    init_dtkernel_no_inhibit_libquery = datatools::init_no_inhibit_libquery,
    init_dtkernel_no_inhibit_libinfo  = datatools::init_no_inhibit_libquery, // deprecated
    init_dtkernel_no_libinfo_logging  = datatools::init_no_libinfo_logging,
    init_dtkernel_no_resource_path    = datatools::init_no_resource_path,
    init_dtkernel_no_inhibit_variant  = datatools::init_no_inhibit_variant,
    init_dtkernel_no_variant          = datatools::init_no_variant,
    init_dtkernel_no_inhibit_qt_gui   = datatools::init_no_inhibit_qt_gui,
    init_dtkernel_no_inhibit_urnquery = datatools::init_no_inhibit_urnquery,
    init_dtkernel_reserved_11         = datatools::init_reserved_11,
    init_dtkernel_reserved_12         = datatools::init_reserved_12,
    init_dtkernel_reserved_13         = datatools::init_reserved_13,
    init_dtkernel_reserved_14         = datatools::init_reserved_14,
    init_dtkernel_inhibit             = datatools::init_kernel_inhibit //!< Do not start the Bayeux/datatools kernel
  };

} // end of namespace bayeux

#endif // BAYEUX_BAYEUX_INIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
