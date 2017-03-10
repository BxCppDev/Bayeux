// datatools.cc

// Ourselves:
#include <datatools/datatools.h>

// Stdlib
#include <cstdlib> // for getenv
#include <iostream>
#include <memory>

// Third party:
// - Boost:
#include <boost/smart_ptr/scoped_ptr.hpp>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/kernel.h>
#include <datatools/logger.h> // for logger, etc

namespace datatools {

  void initialize(int argc_, char * argv_[], uint32_t flags_)
  {
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    static bool _init = false;
    if (!_init) {
      bool do_kernel = true;
      if (flags_ & datatools::init_kernel_inhibit) {
        // Inhibit the kernel:
        do_kernel = false;
        DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                     "Do not instantiate the Bayeux/datatools kernel.");
     }
      if (do_kernel) {
        if (! datatools::kernel::is_instantiated()) {
          kernel::instantiate();
          DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                       "Instantiating the Bayeux/datatools kernel...");
        }
        if (datatools::kernel::is_instantiated()) {
          datatools::kernel & krnl = datatools::kernel::instance();
          if (! krnl.is_initialized()) {
            // Cut flags above high limit:
            uint32_t kernel_init_flags = flags_ & datatools::kernel::init_mask;
            DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                         "Initializing the Bayeux/datatools kernel...");
            if (argc_ && argv_) {
              krnl.initialize(argc_, argv_, kernel_init_flags);
            } else {
              krnl.initialize(0, 0, kernel_init_flags);
            }
            DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                         "Bayeux/datatools kernel has been initialized.");
          }
        }
      }
      _init = true;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  void terminate()
  {
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    static bool _terminate = false;
    if (!_terminate) {
      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (krnl.is_initialized()) {
          DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                       "Terminating the Bayeux/datatools kernel...");
          krnl.shutdown();
          DT_LOG_TRACE(detail::sys::const_instance().get_logging(),
                       "Bayeux/datatools kernel is now terminated.");
        }
      }
      _terminate = true;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  namespace detail {

    sys::sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      {
        // DATATOOLS_SYS_DEVEL Should be obsoleted
        char * e = getenv("DATATOOLS_SYS_DEVEL");
        if (e) {
          _logging_ = logger::PRIO_TRACE;
        } else {
          char * l = getenv("DATATOOLS_SYS_LOGGING");
          if (l) {
            std::string log_level(l);
            logger::priority prio = logger::get_priority(log_level);
            if (!logger::is_undefined(prio)) {
              _logging_ = prio;
            } else {
              DT_LOG_WARNING(logger::PRIO_WARNING, "Invalid environment DATATOOLS_SYS_LOGGING='" << log_level << "'!");
            }
          }
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
