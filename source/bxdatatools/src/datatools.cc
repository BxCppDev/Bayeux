// datatools.cc

// Ourselves:
#include <datatools/datatools.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/kernel.h>

namespace datatools {

  void initialize(int argc_, char * argv_[])
  {
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
      ::datatools::_special_initialize_impl();
      _init = true;
    } else {
#if DATATOOLS_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to initialize the already initialized Bayeux/datatools library module !");
#endif
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  void terminate()
  {
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    static bool _terminate = false;
    if (!_terminate) {
      ::datatools::_special_terminate_impl();
      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (krnl.is_initialized()) {
          krnl.shutdown();
        }
      }
      _terminate = true;
    } else {
#if DATATOOLS_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to terminate the already terminated Bayeux/datatools library module !");
#endif
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  namespace detail {

    sys::sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      {
        char * e = getenv("DATATOOLS_SYS_DEVEL");
        if (e) {
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
