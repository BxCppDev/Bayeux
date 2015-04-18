//! \file datatools/enriched_base-reflect.h
#ifndef DATATOOLS_ENRICHED_BASE_REFLECT_H
#define DATATOOLS_ENRICHED_BASE_REFLECT_H

// Third party:
// - CAMP:
#include <camp/enum.hpp>

// This project:
#include <datatools/enriched_base.h>
#include <datatools/properties.h>

// Export the introspection code for the 'datatools::enriched_base' class:
DR_CLASS_EXPORT(::datatools::enriched_base, "datatools::enriched_base")

namespace datatools {
  namespace detail {
    namespace reflection {

    DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::enriched_base, /*tag_*/)
    {
      // Declare and populate the reflection metaclass for the 'datatools::enriched_base' class:
      DR_CLASS_DECLARE (::datatools::enriched_base)
        .DR_CLASS_BASE(::datatools::i_serializable)
        .DR_CLASS_BASE(::datatools::i_tree_dumpable)
        .DR_CLASS_CONSTRUCTOR()
        .DR_CLASS_PROPERTY_GETTER_SETTER("name",
                                         ::datatools::enriched_base::get_name,
                                         ::datatools::enriched_base::set_name)
        .DR_CLASS_PROPERTY_GETTER_SETTER("display_name",
                                         ::datatools::enriched_base::get_display_name,
                                         ::datatools::enriched_base::set_display_name)
        .DR_CLASS_PROPERTY_GETTER_SETTER("terse_description",
                                         ::datatools::enriched_base::get_terse_description,
                                         ::datatools::enriched_base::set_terse_description)
        .DR_CLASS_PROPERTY_GETTER_SETTER("logging_priority",
                                         ::datatools::enriched_base::get_logging_priority,
                                         ::datatools::enriched_base::set_logging_priority)
        .DR_CLASS_PROPERTY_GETTER("auxiliaries",
                                  ::datatools::enriched_base::get_auxiliaries)

        .DR_CLASS_METHOD_CONST("has_name",
                               ::datatools::enriched_base,
                               ::datatools::enriched_base::has_name,
                               bool)
        .DR_CLASS_METHOD_CONST("has_display_name",
                               ::datatools::enriched_base,
                               ::datatools::enriched_base::has_display_name,
                               bool)
        .DR_CLASS_METHOD_CONST("has_terse_description",
                               ::datatools::enriched_base,
                               ::datatools::enriched_base::has_terse_description,
                               bool)
        .DR_CLASS_METHOD_CONST("has_auxiliaries",
                               ::datatools::enriched_base,
                               ::datatools::enriched_base::has_auxiliaries,
                               bool)
        .DR_CLASS_VOID_METHOD("clear",
                              ::datatools::enriched_base,
                              ::datatools::enriched_base::clear)
        .DR_CLASS_VOID_METHOD("reset_auxiliaries",
                              ::datatools::enriched_base,
                              ::datatools::enriched_base::reset_auxiliaries)
        .DR_CLASS_METHOD_CONST("is_debug",
                               ::datatools::enriched_base,
                               ::datatools::enriched_base::is_debug,
                               bool)
        .DR_CLASS_METHOD("get_auxiliaries",
                         ::datatools::enriched_base,
                         ::datatools::enriched_base::get_auxiliaries,
                         const datatools::properties &)
        .DR_CLASS_METHOD("grab_auxiliaries",
                         ::datatools::enriched_base,
                         ::datatools::enriched_base::grab_auxiliaries,
                         datatools::properties &)
        ;
      // std::cerr << "DEVEL: Number of metaenums = " << camp::enumCount() << std::endl;
      // for (int i = 0; i < camp::enumCount(); i++) {
      //   const camp::Enum & e = camp::enumByIndex(i);
      //   std::cerr << "DEVEL: Metaenum #" << i << " : " << e.name() << std::endl;
      // }
      return;
    }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_ENRICHED_BASE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
