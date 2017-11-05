/// \file mctools/base_step_hit-reflect.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-06-01
 * Last modified: 2013-06-01
 *
 * License:
 *
 * Description:
 *
 *   Base step hit reflection code.
 *
 */

#ifndef MCTOOLS_BASE_STEP_HIT_REFLECT_H
#define MCTOOLS_BASE_STEP_HIT_REFLECT_H 1

// Ourselves :
#include <mctools/base_step_hit.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'mctools::base_step_hit' class:
DR_CLASS_EXPORT (::mctools::base_step_hit, "mctools::base_step_hit")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {

  namespace detail {

    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::mctools::base_step_hit, tag_)
      {
        DR_CLASS_DECLARE (::mctools::base_step_hit)
          .DR_CLASS_BASE(::geomtools::base_hit)
          .DR_CLASS_CONSTRUCTOR()

          // Properties :
          .DR_CLASS_PROPERTY_GETTER_SETTER("position_start",
                                           ::mctools::base_step_hit::get_position_start,
                                           ::mctools::base_step_hit::set_position_start)
          .DR_CLASS_PROPERTY_GETTER_SETTER("position_stop",
                                           ::mctools::base_step_hit::get_position_stop,
                                           ::mctools::base_step_hit::set_position_stop)
          .DR_CLASS_PROPERTY_GETTER_SETTER("momentum_start",
                                           ::mctools::base_step_hit::get_momentum_start,
                                           ::mctools::base_step_hit::set_momentum_start)
          .DR_CLASS_PROPERTY_GETTER_SETTER("momentum_stop",
                                           ::mctools::base_step_hit::get_momentum_stop,
                                           ::mctools::base_step_hit::set_momentum_stop)
          .DR_CLASS_PROPERTY_GETTER_SETTER("time_start",
                                           ::mctools::base_step_hit::get_time_start,
                                           ::mctools::base_step_hit::set_time_start)
          .DR_CLASS_PROPERTY_GETTER_SETTER("time_stop",
                                           ::mctools::base_step_hit::get_time_stop,
                                           ::mctools::base_step_hit::set_time_stop)
          .DR_CLASS_PROPERTY_GETTER_SETTER("energy_deposit",
                                           ::mctools::base_step_hit::get_energy_deposit,
                                           ::mctools::base_step_hit::set_energy_deposit)
          .DR_CLASS_PROPERTY_GETTER_SETTER("particle_name",
                                           ::mctools::base_step_hit::get_particle_name,
                                           ::mctools::base_step_hit::set_particle_name)

          // Methods:
          .DR_CLASS_METHOD_CONST("is_primary_particle",
                                 ::mctools::base_step_hit,
                                 ::mctools::base_step_hit::is_primary_particle,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_track_id",
                                 ::mctools::base_step_hit,
                                 ::mctools::base_step_hit::has_track_id,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_parent_track_id",
                                 ::mctools::base_step_hit,
                                 ::mctools::base_step_hit::has_parent_track_id,
                                 bool)
          .DR_CLASS_METHOD_CONST("get_track_id",
                                 ::mctools::base_step_hit,
                                 ::mctools::base_step_hit::get_track_id,
                                 int)
          .DR_CLASS_METHOD_CONST("get_parent_track_id",
                                 ::mctools::base_step_hit,
                                 ::mctools::base_step_hit::get_parent_track_id,
                                 int)

          .DR_CLASS_VOID_METHOD("invalidate_position_start",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_position_start)
          .DR_CLASS_VOID_METHOD("invalidate_position_stop",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_position_stop)
          .DR_CLASS_VOID_METHOD("invalidate_momentum_start",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_momentum_start)
          .DR_CLASS_VOID_METHOD("invalidate_momentum_stop",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_momentum_stop)
          .DR_CLASS_VOID_METHOD("invalidate_time_start",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_time_start)
          .DR_CLASS_VOID_METHOD("invalidate_time_stop",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_time_stop)
          .DR_CLASS_VOID_METHOD("invalidate_energy_deposit",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_energy_deposit)
          .DR_CLASS_VOID_METHOD("invalidate_particle_name",
                                ::mctools::base_step_hit,
                                ::mctools::base_step_hit::invalidate_particle_name)
          ;
        return;
      }

    } // end of namespace reflection

  } // end of namespace detail

} // end of namespace datatools

#endif // GEOMTOOLS_BASE_STEP_HIT_REFLECT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
