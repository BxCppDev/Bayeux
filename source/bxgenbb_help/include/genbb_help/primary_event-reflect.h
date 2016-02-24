/// \file genbb_help/primary_event-reflect.h

#ifndef GENBB_HELP_PRIMARY_EVENT_REFLECT_H
#define GENBB_HELP_PRIMARY_EVENT_REFLECT_H

// This project:
#include <genbb_help/primary_event.h>

// - Bayeux/datatools:
#include <datatools/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'genbb::primary_event' class:
DR_CLASS_EXPORT (::genbb::primary_event, "genbb::primary_event")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::genbb::primary_event, /*tag_*/)
      {
        // Declare and populate the reflection metaclass for the 'genbb::primary_event' class:
        DR_CLASS_DECLARE(::genbb::primary_event)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_PROPERTY_GETTER_SETTER("time",
                                           ::genbb::primary_event::get_time,
                                           ::genbb::primary_event::set_time)
          .DR_CLASS_PROPERTY_GETTER_SETTER("vertex",
                                           ::genbb::primary_event::get_vertex,
                                           ::genbb::primary_event::set_vertex)
          .DR_CLASS_PROPERTY_GETTER_SETTER("genbb_weight",
                                           ::genbb::primary_event::get_genbb_weight,
                                           ::genbb::primary_event::set_genbb_weight)
          .DR_CLASS_PROPERTY_GETTER_SETTER("label",
                                           ::genbb::primary_event::get_label,
                                           ::genbb::primary_event::set_label)
          .DR_CLASS_PROPERTY_GETTER_SETTER("classification",
                                           ::genbb::primary_event::get_classification,
                                           ::genbb::primary_event::set_classification)
          .DR_CLASS_PROPERTY_GETTER_SETTER("auxiliaries",
                                           ::genbb::primary_event::get_auxiliaries,
                                           ::genbb::primary_event::set_auxiliaries)
          .DR_CLASS_PROPERTY_GETTER("number_of_particles",
                                    ::genbb::primary_event::get_number_of_particles)
          .DR_CLASS_PROPERTY_GETTER("total_kinetic_energy",
                                    ::genbb::primary_event::get_total_kinetic_energy)
          .DR_CLASS_METHOD_CONST("is_valid",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::is_valid,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_time",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::has_time,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_vertex",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::has_vertex,
                                 bool)
          .DR_CLASS_VOID_METHOD("reset",
                                ::genbb::primary_event,
                                ::genbb::primary_event::reset)
          .DR_CLASS_VOID_METHOD("reset_label",
                                ::genbb::primary_event,
                                ::genbb::primary_event::reset_label)
          .DR_CLASS_VOID_METHOD("reset_classification",
                                ::genbb::primary_event,
                                ::genbb::primary_event::reset_classification)
          .DR_CLASS_VOID_METHOD("compute_classification",
                                ::genbb::primary_event,
                                ::genbb::primary_event::compute_classification)
          .DR_CLASS_VOID_METHOD3("rotate",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::rotate,
                                 double, double, double)
          .DR_CLASS_VOID_METHOD2("shift_particles_time",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::shift_particles_time,
                                 double, int)
          .DR_CLASS_METHOD1_CONST("get_particle",
                                  ::genbb::primary_event,
                                  ::genbb::primary_event::get_particle,
                                  const ::genbb::primary_particle &,
                                  int)
          .DR_CLASS_VOID_METHOD1("add_particle",
                                 ::genbb::primary_event,
                                 ::genbb::primary_event::add_particle,
                                 const ::genbb::primary_particle &)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GENBB_HELP_PRIMARY_EVENT_REFLECT_H

// Local Variables: --
// mode: c++ --
// End: --
