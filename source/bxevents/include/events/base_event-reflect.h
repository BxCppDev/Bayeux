/// \file mctools/base_event-reflect.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-09-12
 * Last modified: 2017-09-12
 *
 * License: GPL3
 *
 * Description:
 *
 *   Base event reflection code.
 *
 */

#ifndef EVENTS_BASE_EVENT_REFLECT_H
#define EVENTS_BASE_EVENT_REFLECT_H 1

// Ourselves :
#include <events/base_event.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'events::base_event' class:
DR_CLASS_EXPORT (::events::base_event, "events::base_event")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::events::base_event, tag_)
      {
        DR_CLASS_DECLARE (::events::base_event)
          .DR_CLASS_BASE(::geomtools::base_hit)
          .DR_CLASS_CONSTRUCTOR()

          // Properties :
          .DR_CLASS_PROPERTY_GETTER_SETTER("position",
                                           ::events::base_event::get_position,
                                           ::events::base_event::set_position)
          .DR_CLASS_PROPERTY_GETTER_SETTER("time",
                                           ::events::base_event::get_time,
                                           ::events::base_event::set_time)
          .DR_CLASS_PROPERTY_GETTER_SETTER("category",
                                           ::events::base_event::get_category,
                                           ::events::base_event::set_category)

          // Methods:
          .DR_CLASS_METHOD_CONST("is_valid",
                                 ::events::base_event,
                                 ::events::base_event::is_valid,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_category",
                                 ::events::base_event,
                                 ::events::base_event::has_category,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_position",
                                 ::events::base_event,
                                 ::events::base_event::has_position,
                                 bool)
          .DR_CLASS_METHOD_CONST("get_time",
                                 ::events::base_event,
                                 ::events::base_event::get_time,
                                 int)

          .DR_CLASS_VOID_METHOD("invalidate_category",
                                ::events::base_event,
                                ::events::base_event::invalidate_category)
          .DR_CLASS_VOID_METHOD("invalidate_position",
                                ::events::base_event,
                                ::events::base_event::invalidate_position)
          .DR_CLASS_VOID_METHOD("invalidate_time",
                                ::events::base_event,
                                ::events::base_event::invalidate_time)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // EVENTS_BASE_EVENT_REFLECT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
