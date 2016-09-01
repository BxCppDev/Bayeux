// datatools.cc

// Ourselves:
#include <datatools/datatools.h>

// Stdlib
#include <cstdlib>                        // for getenv
#include <iostream>

// Third party:
// - Boost:
#include <boost/smart_ptr/scoped_ptr.hpp>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/kernel.h>
#include "datatools/logger.h"              // for logger, etc


namespace datatools {

  void initialize(int argc_, char * argv_[], uint32_t /*flags_*/)
  {
    // std::cerr << "DEVEL: datatools::initialize: Entering..." << std::endl;

    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    static bool _init = false;
    if (!_init) {
      if (! datatools::kernel::is_instantiated()) {
        kernel::instantiate();
      }
      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (! krnl.is_initialized()) {
          if (argc_ && argv_) {
            krnl.initialize(argc_, argv_);
          } else {
            krnl.initialize(0, 0);
          }
        }
      }
      _init =  true;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    //std::cerr << "DEVEL: datatools::initialize: Exiting." << std::endl;
    return;
  }

  void terminate()
  {
    //std::cerr << "DEVEL: datatools::terminate: Entering..." << std::endl;
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    static bool _terminate = false;
    if (!_terminate) {
      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (krnl.is_initialized()) {
          krnl.shutdown();
        }
      }
      _terminate = true;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    //std::cerr << "DEVEL: datatools::terminate: Exiting." << std::endl;
    return;
  }

  namespace detail {

    sys::sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      {
        char * e = getenv("DATATOOLS_SYS_DEVEL");
        if (e) {
          // std::cerr << "DEVEL: datatools::detail::sys: TRACE" << std::endl;
          _logging_ = logger::PRIO_TRACE;
        }
      }
      return;
    }

    sys::~sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      return;
    }

    logger::priority sys::get_logging() const
    {
      return _logging_;
    }

    // static
    const sys & sys::const_instance()
    {
      static boost::scoped_ptr<sys> _sys(new sys);
#ifdef DATATOOLS_SYS_DEVEL
      _sys->_logging_ = logger::PRIO_TRACE;
#endif // DATATOOLS_SYS_DEVEL
      return *_sys;
    }

  } // end of namespace detail

} // end of namespace datatools
