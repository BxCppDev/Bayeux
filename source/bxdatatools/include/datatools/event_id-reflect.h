//! \file datatools/event_id-reflect.h
#ifndef DATATOOLS_EVENT_ID_REFLECT_H
#define DATATOOLS_EVENT_ID_REFLECT_H

// Standard Library:
#include <iostream>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/event_id.h>
#include <datatools/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'datatools::event_id' class:
DR_CLASS_EXPORT(::datatools::event_id, "datatools::event_id")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::event_id, /*tag_*/)
      {
        // Declare and populate the reflection metaclass for the 'datatools::event_id' class:
        DR_CLASS_DECLARE (::datatools::event_id)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR1(int)
          .DR_CLASS_CONSTRUCTOR2(int,int)
          .DR_CLASS_PROPERTY_GETTER_SETTER("run_number",
                                           ::datatools::event_id::get_run_number,
                                           ::datatools::event_id::set_run_number)
          .DR_CLASS_PROPERTY_GETTER_SETTER("event_number",
                                           ::datatools::event_id::get_event_number,
                                           ::datatools::event_id::set_event_number)
          .DR_CLASS_VOID_METHOD("reset",
                                ::datatools::event_id,
                                ::datatools::event_id::reset)
          .DR_CLASS_METHOD_CONST("is_valid",\
                                 ::datatools::event_id,\
                                 ::datatools::event_id::is_valid,\
                                 bool)
          .DR_CLASS_VOID_METHOD2("set",
                                 ::datatools::event_id,
                                 ::datatools::event_id::set,
                                 int , int)
          .DR_CLASS_METHOD2_CONST("has",
                                  ::datatools::event_id,
                                  ::datatools::event_id::has,
                                  bool, int , int)
          .DR_CLASS_METHOD2_CONST("match",
                                  ::datatools::event_id,
                                  ::datatools::event_id::match,
                                  bool, int , int)
          .DR_CLASS_VOID_METHOD("reset",
                                ::datatools::event_id,
                                ::datatools::event_id::reset)
          .DR_CLASS_METHOD_CONST("to_string",
                                 ::datatools::event_id,
                                 ::datatools::event_id::to_string,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("from_string",
                                 ::datatools::event_id,
                                 ::datatools::event_id::from_string,
                                 const std::string &)
          .DR_CLASS_VOID_METHOD2_CONST("to_smart_string",
                                       ::datatools::event_id,
                                       ::datatools::event_id::to_smart_string,
                                       const std::string &, const std::string&)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_EVENT_ID_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
