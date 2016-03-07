/// \file geomtools/i_placement-reflect.h

#ifndef GEOMTOOLS_I_PLACEMENT_REFLECT_H
#define GEOMTOOLS_I_PLACEMENT_REFLECT_H

// Third party:
// - Bayeux/datatools:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>


// Export the introspection code for the 'geomtools::i_placement' class:
DR_CLASS_EXPORT (::geomtools::i_placement, "geomtools::i_placement")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::i_placement, DT_UNUSED(tag_) )
      {
        // Declare and populate the reflection metaclass for the 'geomtools::i_placement' class:
        DR_CLASS_DECLARE (::geomtools::i_placement)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_METHOD_CONST("is_multiple",
                                 ::geomtools::i_placement,
                                 ::geomtools::i_placement::is_multiple,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_replica",
                                 ::geomtools::i_placement,
                                 ::geomtools::i_placement::is_replica,
                                 bool)
          .DR_CLASS_METHOD1_CONST("get_placement",
                                  ::geomtools::i_placement,
                                  ::geomtools::i_placement::get_placement_by_index,
                                  ::geomtools::placement,
                                  int)
          /* .DR_CLASS_VOID_METHOD2_CONST("compute_placement", */
          /*                               ::geomtools::i_placement, */
          /*                               ::geomtools::i_placement::compute_placement, */
          /*                               ::geomtools::placement &, int) */
          .DR_CLASS_METHOD_CONST("has_only_one_rotation",
                                 ::geomtools::i_placement,
                                 ::geomtools::i_placement::has_only_one_rotation,
                                 bool)
          .DR_CLASS_METHOD_CONST("get_dimension",
                                 ::geomtools::i_placement,
                                 ::geomtools::i_placement::get_dimension,
                                 size_t)
          .DR_CLASS_METHOD_CONST("get_number_of_items",
                                 ::geomtools::i_placement,
                                 ::geomtools::i_placement::get_number_of_items,
                                 size_t)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_PLACEMENT_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
