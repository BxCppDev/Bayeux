// -*- mode: c++; -*-
/// \file geomtools/geom_id-reflect.h

#ifndef GEOMTOOLS_GEOM_ID_REFLECT_H
#define GEOMTOOLS_GEOM_ID_REFLECT_H

// This project:
#include <geomtools/geom_id.h>

// Export the introspection code for the 'geomtools::geom_id' class:
DR_CLASS_EXPORT (::geomtools::geom_id, "geomtools::geom_id")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::geom_id, /*tag_*/)
      {
//                              std::cerr << "****** DEVEL ****** datatools::detail::reflection: "
//                                                                      << "geomtools::geom_id" << std::endl;
        DR_CLASS_DECLARE (::geomtools::geom_id)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR2(uint32_t,uint32_t)
          .DR_CLASS_CONSTRUCTOR3(uint32_t,uint32_t,uint32_t)
          .DR_CLASS_CONSTRUCTOR4(uint32_t,uint32_t,uint32_t,uint32_t)
          .DR_CLASS_CONSTRUCTOR5(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t)
          .DR_CLASS_PROPERTY_GETTER_SETTER("type",
                                           ::geomtools::geom_id::get_type,
                                           ::geomtools::geom_id::set_type)
          .DR_CLASS_PROPERTY_GETTER_SETTER("depth",
                                           ::geomtools::geom_id::get_depth,
                                           ::geomtools::geom_id::set_depth)
          .DR_CLASS_METHOD1_CONST("is_type",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::is_type,
                                  bool,
                                  uint32_t)
          .DR_CLASS_METHOD1_CONST("is_any",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::is_any,
                                  bool,
                                  int)
          .DR_CLASS_METHOD1_CONST("is_invalid",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::is_invalid,
                                  bool,
                                  int)
          .DR_CLASS_METHOD_CONST("is_valid",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::is_valid,
                                  bool)
          .DR_CLASS_METHOD_CONST("is_complete",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::is_complete,
                                 bool)
          .DR_CLASS_METHOD1_CONST("get",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::get,
                                  uint32_t,
                                  int)
          .DR_CLASS_VOID_METHOD2("set",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::set,
                                 int,
                                 uint32_t)
          .DR_CLASS_VOID_METHOD1("set_any",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::set_any,
                                 int)
          .DR_CLASS_VOID_METHOD1("set_invalid",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::set_invalid,
                                 int)
          .DR_CLASS_VOID_METHOD("invalidate",
                                ::geomtools::geom_id,
                                ::geomtools::geom_id::invalidate)
          .DR_CLASS_VOID_METHOD("reset",
                                ::geomtools::geom_id,
                                ::geomtools::geom_id::reset)
          .DR_CLASS_VOID_METHOD1("inherits_from",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::inherits_from,
                                 const ::geomtools::geom_id &)
          .DR_CLASS_VOID_METHOD1("extract_from",
                                 ::geomtools::geom_id,
                                 ::geomtools::geom_id::extract_from,
                                 const ::geomtools::geom_id &)
          .DR_CLASS_METHOD2_CONST("match",
                                  ::geomtools::geom_id,
                                  ::geomtools::geom_id::match_impl,
                                  bool,
                                  const ::geomtools::geom_id &,
                                  bool)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_GEOM_ID_REFLECT_H
