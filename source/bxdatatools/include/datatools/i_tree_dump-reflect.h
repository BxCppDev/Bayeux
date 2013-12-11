// -*- mode: c++; -*-
//! \file datatools/event_id-reflect.h
#ifndef DATATOOLS_I_TREE_DUMP_REFLECT_H_
#define DATATOOLS_I_TREE_DUMP_REFLECT_H_

// Standard Library

// Third Party
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/i_tree_dump.h>

// Export the introspection code for the 'datatools::i_tree_dumpable' class:
DR_CLASS_EXPORT (::datatools::i_tree_dumpable, "datatools::i_tree_dumpable")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::i_tree_dumpable, /*tag_*/)
      {
        // Declare and populate the reflection metaclass for the 'datatools::i_tree_dumpable' class:
        DR_CLASS_DECLARE (::datatools::i_tree_dumpable)
          .DR_CLASS_VOID_METHOD2_CONST("tree_print",
                                       ::datatools::i_tree_dumpable,
                                       ::datatools::i_tree_dumpable::tree_print,
                                       int, const std::string &)
          .DR_CLASS_VOID_METHOD3_CONST("smart_print",
                                       ::datatools::i_tree_dumpable,
                                       ::datatools::i_tree_dumpable::smart_print,
                                       int, const std::string &, const std::string &)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_I_TREE_DUMP_REFLECT_H_
