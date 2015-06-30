//! \file datatools/base_service-reflect.h
#ifndef DATATOOLS_BASE_SERVICE_REFLECT_H
#define DATATOOLS_BASE_SERVICE_REFLECT_H

// Third party:
// - CAMP:
#include <camp/enum.hpp>

// This project:
#include <datatools/base_service.h>
#include <datatools/properties.h>

// Export the introspection code for the 'datatools::base_service' class:
DR_CLASS_EXPORT(::datatools::base_service, "datatools::base_service")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::base_service, /*tag_*/)
      {
        // Declare and populate the reflection metaclass for the 'datatools::base_service' class:
        DR_CLASS_DECLARE(::datatools::base_service)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_PROPERTY_GETTER_SETTER("name",
                                           ::datatools::base_service::get_name,
                                           ::datatools::base_service::set_name)
          .DR_CLASS_PROPERTY_GETTER_SETTER("display_name",
                                           ::datatools::base_service::get_display_name,
                                           ::datatools::base_service::set_display_name)
          .DR_CLASS_PROPERTY_GETTER_SETTER("description",
                                           ::datatools::base_service::get_description,
                                           ::datatools::base_service::set_description)
          .DR_CLASS_PROPERTY_GETTER_SETTER("logging_priority",
                                           ::datatools::base_service::get_logging_priority,
                                           ::datatools::base_service::set_logging_priority)
          .DR_CLASS_METHOD_CONST("has_name",
                                 ::datatools::base_service,
                                 ::datatools::base_service::has_name,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_display_name",
                                 ::datatools::base_service,
                                 ::datatools::base_service::has_display_name,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_description",
                                 ::datatools::base_service,
                                 ::datatools::base_service::has_description,
                                 bool)
          .DR_CLASS_METHOD("initialize_simple",
                            ::datatools::base_service,
                            ::datatools::base_service::initialize_simple,
                            int)
          .DR_CLASS_METHOD1("initialize_standalone",
                            ::datatools::base_service,
                            ::datatools::base_service::initialize_standalone,
                            int,
                            const datatools::properties &)
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
