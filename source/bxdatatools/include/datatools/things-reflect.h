// -*- mode: c++; -*-
//! \file datatools/things-reflect.h
#ifndef DATATOOLS_THINGS_REFLECT_H_
#define DATATOOLS_THINGS_REFLECT_H_

// Standard Library

// Third Party
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/things.h>

// Export the reflection code for the 'datatools::things' class:
DR_CLASS_EXPORT (::datatools::things, "datatools::things")

namespace datatools {
  namespace detail {
    namespace reflection {

    DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::things,tag_)
    {
      // Declare and populate the introspection metaclass for the 'datatools::things' class:
      DR_CLASS_DECLARE (::datatools::things)
        .DR_CLASS_BASE(::datatools::i_serializable)
        .DR_CLASS_BASE(::datatools::i_tree_dumpable)
        .DR_CLASS_CONSTRUCTOR()
        .DR_CLASS_CONSTRUCTOR2(std::string, std::string)
        .DR_CLASS_PROPERTY_GETTER_SETTER ("name",
                                          ::datatools::things::get_name_impl,
                                          ::datatools::things::set_name_impl)
        .DR_CLASS_PROPERTY_GETTER_SETTER ("description",
                                          ::datatools::things::get_description_impl,
                                          ::datatools::things::set_description_impl)
        .DR_CLASS_METHOD_CONST("has_name",
                               ::datatools::things,
                               ::datatools::things::has_name_impl,
                               bool)
        .DR_CLASS_METHOD_CONST("has_description",
                               ::datatools::things,
                               ::datatools::things::has_description_impl,
                               bool)
        .DR_CLASS_METHOD_CONST("size",
                               ::datatools::things,
                               ::datatools::things::size,
                               int32_t)
        .DR_CLASS_METHOD_CONST("empty",
                               ::datatools::things,
                               ::datatools::things::empty,
                               bool)
        .DR_CLASS_VOID_METHOD("reset",
                              ::datatools::things,
                              ::datatools::things::reset)
        .DR_CLASS_VOID_METHOD("clear",
                              ::datatools::things,
                              ::datatools::things::clear)
        .DR_CLASS_METHOD1_CONST("has",
                                ::datatools::things,
                                ::datatools::things::has,
                                bool,
                                const std::string &)
        .DR_CLASS_METHOD2_CONST("has_serial_tag",
                                ::datatools::things,
                                ::datatools::things::has_serial_tag,
                                bool,
                                const std::string &,const std::string &)
        .DR_CLASS_METHOD1_CONST("is_constant",
                                ::datatools::things,
                                ::datatools::things::is_constant,
                                bool,
                                const std::string &)
        .DR_CLASS_METHOD1_CONST("is_mutable",
                                ::datatools::things,
                                ::datatools::things::is_mutable,
                                bool,
                                const std::string &)
        .DR_CLASS_VOID_METHOD2("set_constant",
                               ::datatools::things,
                               ::datatools::things::set_constant,
                               const std::string &, bool)
        .DR_CLASS_VOID_METHOD2("set_entry_description",
                               ::datatools::things,
                               ::datatools::things::set_entry_description,
                               const std::string &, const std::string &)
        .DR_CLASS_METHOD1_CONST("get_entry_description",
                                ::datatools::things,
                                ::datatools::things::get_entry_description,
                                const std::string &,
                                const std::string &)
        .DR_CLASS_VOID_METHOD1("remove",
                               ::datatools::things,
                               ::datatools::things::remove,
                               const std::string &)
        .DR_CLASS_METHOD1_CONST("get_entry_name",
                                ::datatools::things,
                                ::datatools::things::get_entry_name,
                                const std::string &,
                                int)
        .DR_CLASS_METHOD2_CONST("entry_is_a",
                                ::datatools::things,
                                ::datatools::things::entry_is_a,
                                bool,
                                const std::string &, const std::string &)
        .DR_CLASS_METHOD1_CONST("get_entry_serial_tag",
                                ::datatools::things,
                                ::datatools::things::get_entry_serial_tag,
                                std::string,
                                const std::string &)
        .DR_CLASS_METHOD1_CONST("get_entry_introspection_id",
                                ::datatools::things,
                                ::datatools::things::get_entry_introspection_id,
                                std::string,
                                const std::string &)
        .DR_CLASS_METHOD1_CONST("get_entry",
                                ::datatools::things,
                                ::datatools::things::get_entry,
                                const datatools::i_serializable &,
                                const std::string &)
        .DR_CLASS_METHOD1("grab_entry",
                          ::datatools::things,
                          ::datatools::things::grab_entry,
                          datatools::i_serializable &,
                          const std::string &)
        .DR_CLASS_METHOD4("add_entry",
                          ::datatools::things,
                          ::datatools::things::add_entry_impl,
                          datatools::i_serializable &,
                          const std::string &, const std::string &,
                          const std::string &, bool)
        /*
         */
        ;
      return;
    }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_THINGS_REFLECT_H_
