//! \file datatools/logger-reflect.h
#ifndef DATATOOLS_LOGGER_REFLECT_H
#define DATATOOLS_LOGGER_REFLECT_H

// Standard Library:
#include <iostream>

// This project:
#include <datatools/logger.h>
#include <datatools/reflection_macros.h>
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
          .DR_ENUM_PAIR("undefined",   ::datatools::logger::PRIO_UNDEFINED)
          .DR_ENUM_PAIR("fatal",       ::datatools::logger::PRIO_FATAL)
          .DR_ENUM_PAIR("critical",    ::datatools::logger::PRIO_CRITICAL)
          .DR_ENUM_PAIR("error",       ::datatools::logger::PRIO_ERROR)
          .DR_ENUM_PAIR("warning",     ::datatools::logger::PRIO_WARNING)
          .DR_ENUM_PAIR("notice",      ::datatools::logger::PRIO_NOTICE)
          .DR_ENUM_PAIR("information", ::datatools::logger::PRIO_INFORMATION)
          .DR_ENUM_PAIR("debug",       ::datatools::logger::PRIO_DEBUG)
          .DR_ENUM_PAIR("trace",       ::datatools::logger::PRIO_TRACE)
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
