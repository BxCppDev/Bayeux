// -*- mode: c++ ; -*-
/* loggable_support.cc
 */

#include <mctools/g4/loggable_support.h>

#include <string>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace mctools {

  namespace g4 {

    loggable_support::loggable_support()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
    }

    loggable_support::loggable_support(datatools::logger::priority p)
    {
      _logging_priority = p;
    }

    datatools::logger::priority loggable_support::_logprio() const
    {
      return _logging_priority;
    }

    datatools::logger::priority loggable_support::get_logging_priority() const
    {
      return _logging_priority;
    }

    void loggable_support::set_debug (bool debug_)
    {
      if (debug_) {
        if (_logging_priority < datatools::logger::PRIO_DEBUG) {
          set_logging_priority(datatools::logger::PRIO_DEBUG);
        }
      } else {
        set_logging_priority(datatools::logger::PRIO_WARNING);
      }
    }

    bool loggable_support::is_debug () const
    {
      return _logging_priority >= datatools::logger::PRIO_DEBUG;
    }

    void loggable_support::set_verbose (bool verbose_)
    {
      if (verbose_) {
        if (_logging_priority < datatools::logger::PRIO_NOTICE) {
          set_logging_priority(datatools::logger::PRIO_NOTICE);
        }
      } else {
        set_logging_priority(datatools::logger::PRIO_WARNING);
      }
    }

    bool loggable_support::is_verbose () const
    {
      return _logging_priority >= datatools::logger::PRIO_NOTICE;
    }

    void loggable_support::set_logging_priority(datatools::logger::priority p)
    {
      _logging_priority = p;
    }

    void loggable_support::_initialize_logging_support(const datatools::properties & config_)
    {
      set_logging_priority(datatools::logger::extract_logging_configuration(config_,
                                                                            datatools::logger::PRIO_WARNING));
      /*
      if (config_.has_key("logging.priority")) {
        std::string ps = config_.fetch_string("logging.priority");
        datatools::logger::priority p = datatools::logger::get_priority(ps);
        DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                    std::logic_error,
                    "Invalid logging priority label '" << ps << "' !");
        set_logging_priority(p);
      } else if (config_.has_flag("debug")) {
        set_debug(true);
      } else if (config_.has_flag("verbose")) {
        set_verbose(true);
      }
      */
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of loggable_support.cc
