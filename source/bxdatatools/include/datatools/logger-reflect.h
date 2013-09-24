// -*- mode: c++; -*-
//! \file datatools/logger-reflect.h
#ifndef DATATOOLS_LOGGER_REFLECT_H_
#define DATATOOLS_LOGGER_REFLECT_H_

// Standard Library
#include <iostream>

// Datatools
#include <datatools/logger.h>

// Export the introspection code for the 'datatools::logger::priority' class:
DR_TYPE_EXPORT(::datatools::logger::priority, "datatools::logger::priority")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::logger::priority, tag_)
        {
          // std::cerr << "DEVEL: ***** Registering enum '::datatools::logger::priority'... " << std::endl;
          // std::cerr << "DEVEL: Number of metaenums = " << camp::enumCount() << std::endl;
          // for (int i = 0; i < camp::enumCount(); i++) {
          //   const camp::Enum & e = camp::enumByIndex(i);
          //   std::cerr << "DEVEL: Metaenum #" << i << " : " << e.name() << std::endl;
          // }
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
          // std::cerr << "DEVEL: ***** Registering enum '::datatools::logger::priority' done. " << std::endl;
          return;
        }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_LOGGER_REFLECT_H_
