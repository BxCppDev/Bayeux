/// \file geomtools/base_hit-reflect.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-31
 * Last modified: 2013-05-31
 *
 * License:
 *
 * Description:
 *
 *   Base hit.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BASE_HIT_REFLECT_H
#define GEOMTOOLS_BASE_HIT_REFLECT_H 1

// Third party:
// - Bayeux/datatools
#include <datatools/reflection_macros.h>

// This project :
#include <geomtools/base_hit.h>

// Export the introspection code for the 'geomtools::base_hit' class:
DR_CLASS_EXPORT (::geomtools::base_hit, "geomtools::base_hit")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::base_hit, /*tag_*/)
      {
//                              std::cerr << "****** DEVEL ****** datatools::detail::reflection: "
//                                                                      << "geomtools::base_hit" << std::endl;

        DR_CLASS_DECLARE (::geomtools::base_hit)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_PROPERTY_GETTER_SETTER("hit_id",
                                           ::geomtools::base_hit::get_hit_id,
                                           ::geomtools::base_hit::set_hit_id)
          .DR_CLASS_PROPERTY_GETTER_SETTER("geom_id",
                                           ::geomtools::base_hit::get_geom_id,
                                           ::geomtools::base_hit::set_geom_id)
          .DR_CLASS_PROPERTY_GETTER_SETTER("auxiliaries",
                                           ::geomtools::base_hit::get_auxiliaries,
                                           ::geomtools::base_hit::set_auxiliaries)
          .DR_CLASS_METHOD_CONST("has_hit_id",
                                 ::geomtools::base_hit,
                                 ::geomtools::base_hit::has_hit_id,
                                 bool)
          .DR_CLASS_VOID_METHOD("invalidate_hit_id",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::invalidate_hit_id)
          .DR_CLASS_METHOD_CONST("has_geom_id",
                                 ::geomtools::base_hit,
                                 ::geomtools::base_hit::has_geom_id,
                                 bool)
          .DR_CLASS_METHOD("grab_geom_id",
                           ::geomtools::base_hit,
                           ::geomtools::base_hit::grab_geom_id,
                           ::geomtools::geom_id &)
          .DR_CLASS_METHOD("get_geom_id",
                           ::geomtools::base_hit,
                           ::geomtools::base_hit::get_geom_id,
                           const ::geomtools::geom_id &)
          .DR_CLASS_METHOD_CONST("has_auxiliaries",
                                 ::geomtools::base_hit,
                                 ::geomtools::base_hit::has_auxiliaries,
                                 bool)
          .DR_CLASS_VOID_METHOD("invalidate_geom_id",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::invalidate_geom_id)
          .DR_CLASS_VOID_METHOD("invalidate_auxiliaries",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::invalidate_auxiliaries)
          .DR_CLASS_VOID_METHOD("invalidate",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::invalidate)
          .DR_CLASS_VOID_METHOD("reset",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::reset)
          .DR_CLASS_METHOD_CONST("is_valid",
                                 ::geomtools::base_hit,
                                 ::geomtools::base_hit::is_valid,
                                 bool)
          .DR_CLASS_VOID_METHOD("clear",
                                ::geomtools::base_hit,
                                ::geomtools::base_hit::clear)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_BASE_HIT_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
