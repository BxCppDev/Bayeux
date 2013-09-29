// -*- mode: c++; -*-
//! \file datatools/advanced_object-reflect.h
#ifndef DATATOOLS_ADVANCED_OBJECT_REFLECT_H_
#define DATATOOLS_ADVANCED_OBJECT_REFLECT_H_

// Standard Library

#include <camp/enum.hpp>

// Datatools
#include <datatools/advanced_object.h>
#include <datatools/properties.h>

// Export the introspection code for the 'datatools::advanced_object' class:
DR_CLASS_EXPORT(::datatools::advanced_object, "datatools::advanced_object")

namespace datatools {
  namespace detail {
    namespace reflection {

    DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::advanced_object, tag_)
    {
      // std::cerr << "DEVEL: ***** Registering class '::datatools::advanced_object'... " << std::endl;
      // Declare and populate the reflection metaclass for the 'datatools::advanced_object' class:
      DR_CLASS_DECLARE (::datatools::advanced_object)
        .DR_CLASS_BASE(::datatools::i_serializable)
        .DR_CLASS_BASE(::datatools::i_tree_dumpable)
        .DR_CLASS_CONSTRUCTOR()
        .DR_CLASS_PROPERTY_GETTER_SETTER("name",
                                         ::datatools::advanced_object::get_name,
                                         ::datatools::advanced_object::set_name)
        .DR_CLASS_PROPERTY_GETTER_SETTER("display_name",
                                         ::datatools::advanced_object::get_display_name,
                                         ::datatools::advanced_object::set_display_name)
        .DR_CLASS_PROPERTY_GETTER_SETTER("terse_description",
                                         ::datatools::advanced_object::get_terse_description,
                                         ::datatools::advanced_object::set_terse_description)
        .DR_CLASS_PROPERTY_GETTER_SETTER("logging_priority",
                                         ::datatools::advanced_object::get_logging_priority,
                                         ::datatools::advanced_object::set_logging_priority)
        .DR_CLASS_PROPERTY_GETTER("auxiliaries",
                                  ::datatools::advanced_object::get_auxiliaries)

        .DR_CLASS_METHOD_CONST("has_name",
                               ::datatools::advanced_object,
                               ::datatools::advanced_object::has_name,
                               bool)
        .DR_CLASS_METHOD_CONST("has_display_name",
                               ::datatools::advanced_object,
                               ::datatools::advanced_object::has_display_name,
                               bool)
        .DR_CLASS_METHOD_CONST("has_terse_description",
                               ::datatools::advanced_object,
                               ::datatools::advanced_object::has_terse_description,
                               bool)
        .DR_CLASS_METHOD_CONST("has_auxiliaries",
                               ::datatools::advanced_object,
                               ::datatools::advanced_object::has_auxiliaries,
                               bool)
        .DR_CLASS_VOID_METHOD("clear_advanced",
                              ::datatools::advanced_object,
                              ::datatools::advanced_object::clear_advanced)
        .DR_CLASS_VOID_METHOD("reset_auxiliaries",
                              ::datatools::advanced_object,
                              ::datatools::advanced_object::reset_auxiliaries)
        .DR_CLASS_METHOD_CONST("is_debug",
                               ::datatools::advanced_object,
                               ::datatools::advanced_object::is_debug,
                               bool)
        .DR_CLASS_METHOD("get_auxiliaries",
                         ::datatools::advanced_object,
                         ::datatools::advanced_object::get_auxiliaries,
                         const datatools::properties &)
        .DR_CLASS_METHOD("grab_auxiliaries",
                         ::datatools::advanced_object,
                         ::datatools::advanced_object::grab_auxiliaries,
                         datatools::properties &)
        ;
      // std::cerr << "DEVEL: Number of metaenums = " << camp::enumCount() << std::endl;
      // for (int i = 0; i < camp::enumCount(); i++) {
      //   const camp::Enum & e = camp::enumByIndex(i);
      //   std::cerr << "DEVEL: Metaenum #" << i << " : " << e.name() << std::endl;
      // }
      // std::cerr << "DEVEL: ***** Registering class '::datatools::advanced_object' done." << std::endl;
      return;
    }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_ADVANCED_OBJECT_REFLECT_H_
