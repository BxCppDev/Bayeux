//! \file datatools/logger-reflect.h
#ifndef DATATOOLS_LOGGER_REFLECT_H
#define DATATOOLS_LOGGER_REFLECT_H

// Standard Library:
#include <iostream>

// This project:
#include <datatools/logger.h>
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'datatools::logger::priority' enum:
DR_CLASS_EXPORT(::datatools::logger, "datatools::logger")
DR_TYPE_EXPORT(::datatools::logger::priority, "datatools::logger::priority")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::logger, /*tag_*/)
      {
        DR_CLASS_DECLARE(::datatools::logger)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::logger::priority, /*tag_*/)
      {
        DR_ENUM_DECLARE(::datatools::logger::priority)
          .value("undefined",   ::datatools::logger::PRIO_UNDEFINED)
          .value("fatal",       ::datatools::logger::PRIO_FATAL)
          .value("critical",    ::datatools::logger::PRIO_CRITICAL)
          .value("error",       ::datatools::logger::PRIO_ERROR)
          .value("warning",     ::datatools::logger::PRIO_WARNING)
          .value("notice",      ::datatools::logger::PRIO_NOTICE)
          .value("information", ::datatools::logger::PRIO_INFORMATION)
          .value("debug",       ::datatools::logger::PRIO_DEBUG)
          .value("trace",       ::datatools::logger::PRIO_TRACE)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_LOGGER_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
