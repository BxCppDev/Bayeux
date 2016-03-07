//! \file datatools/base_service-reflect.h
#ifndef DATATOOLS_BASE_SERVICE_REFLECT_H
#define DATATOOLS_BASE_SERVICE_REFLECT_H

// Ourselves:
#include <datatools/base_service.h>

// Third party:
// - CAMP:
#include <camp/enum.hpp>

// This project:
#include <datatools/properties.h>
#include <datatools/compiler_macros.h>

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'datatools::base_service' class:
DR_CLASS_EXPORT(::datatools::base_service, "datatools::base_service")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::base_service, DT_UNUSED(tag_) )
      {
        // Declare and populate the reflection metaclass for the 'datatools::base_service' class:
        DR_CLASS_DECLARE(::datatools::base_service)
          .DR_CLASS_BASE(::datatools::enriched_base)
          .DR_CLASS_METHOD("initialize_simple",
                            ::datatools::base_service,
                            ::datatools::base_service::initialize_simple,
                            int)
          .DR_CLASS_METHOD1("initialize_standalone",
                            ::datatools::base_service,
                            ::datatools::base_service::initialize_standalone,
                            int,
                            const datatools::properties &)
          .DR_CLASS_METHOD("service_class_id",
                           ::datatools::base_service,
                           ::datatools::base_service::service_class_id,
                           std::string)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_BASE_SERVICE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
