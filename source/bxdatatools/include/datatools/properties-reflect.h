//! \file datatools/properties-reflect.h

#ifndef DATATOOLS_PROPERTIES_REFLECT_H
#define DATATOOLS_PROPERTIES_REFLECT_H

// Ouselves:
#include <datatools/properties.h>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// Export the introspection code for the 'datatools::properties' class:
DR_CLASS_EXPORT (::datatools::properties, "datatools::properties")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::properties, DT_UNUSED(tag_) )
      {
        // Declare and populate the introspection metaclass for the 'datatools::properties' class:
        DR_CLASS_DECLARE (::datatools::properties)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR1(std::string)
          .DR_CLASS_METHOD_CONST("size",
                                 ::datatools::properties,
                                 ::datatools::properties::size,
                                 int32_t)
          .DR_CLASS_METHOD_CONST("empty",
                                 ::datatools::properties,
                                 ::datatools::properties::empty,
                                 bool)
          .DR_CLASS_VOID_METHOD1("set_description",
                                 ::datatools::properties,
                                 ::datatools::properties::set_description,
                                 std::string)
          .DR_CLASS_METHOD_CONST("get_description",
                                 ::datatools::properties,
                                 ::datatools::properties::get_description,
                                 std::string)
          .DR_CLASS_METHOD1_CONST("key",
                                  ::datatools::properties,
                                  ::datatools::properties::key,
                                  std::string, int)
          /*
          // Not supported by Camp :
          .DR_CLASS_METHOD_CONST("keys",
          ::datatools::properties,
          ::datatools::properties::keys,
          std::vector<std::string>)
          .DR_CLASS_METHOD1_CONST("keys_not_starting_with",
          ::datatools::properties,
          ::datatools::properties::keys_not_starting_with,
          std::vector<std::string>,
          std::string)
          // Not supported by Camp :
          .DR_CLASS_METHOD1_CONST("keys_starting_with",
          ::datatools::properties,
          ::datatools::properties::keys_starting_with,
          std::vector<std::string>,
          std::string)
          // Not supported by Camp :
          .DR_CLASS_METHOD1_CONST("keys_not_ending_with",
          ::datatools::properties,
          ::datatools::properties::keys_not_ending_with,
          std::vector<std::string>,
          std::string)
          // Not supported by Camp :
          .DR_CLASS_METHOD1_CONST("keys_ending_with",
          ::datatools::properties,
          ::datatools::properties::keys_ending_with,
          std::vector<std::string>,
          std::string)
          */

          .DR_CLASS_METHOD1_CONST("has_key",
                                  ::datatools::properties,
                                  ::datatools::properties::has_key,
                                  bool,
                                  std::string)
          .DR_CLASS_VOID_METHOD1("lock",
                                 ::datatools::properties,
                                 ::datatools::properties::lock,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("unlock",
                                 ::datatools::properties,
                                 ::datatools::properties::unlock,
                                 std::string)
          .DR_CLASS_METHOD1_CONST("is_locked",
                                  ::datatools::properties,
                                  ::datatools::properties::is_locked,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_private",
                                  ::datatools::properties,
                                  ::datatools::properties::is_private,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_public",
                                  ::datatools::properties,
                                  ::datatools::properties::is_public,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_boolean",
                                  ::datatools::properties,
                                  ::datatools::properties::is_boolean,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_integer",
                                  ::datatools::properties,
                                  ::datatools::properties::is_integer,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_real",
                                  ::datatools::properties,
                                  ::datatools::properties::is_real,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_string",
                                  ::datatools::properties,
                                  ::datatools::properties::is_string,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_scalar",
                                  ::datatools::properties,
                                  ::datatools::properties::is_scalar,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("is_vector",
                                  ::datatools::properties,
                                  ::datatools::properties::is_vector,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("key_size",
                                  ::datatools::properties,
                                  ::datatools::properties::key_size,
                                  bool,
                                  std::string)
          .DR_CLASS_METHOD1_CONST("get_key_description",
                                  ::datatools::properties,
                                  ::datatools::properties::get_key_description,
                                  std::string,
                                  std::string)
          .DR_CLASS_VOID_METHOD2("set_key_description",
                                 ::datatools::properties,
                                 ::datatools::properties::set_key_description,
                                 std::string,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("erase",
                                 ::datatools::properties,
                                 ::datatools::properties::erase,
                                 std::string)
          .DR_CLASS_VOID_METHOD("erase_all",
                                ::datatools::properties,
                                ::datatools::properties::erase_all)
          .DR_CLASS_VOID_METHOD1("erase_all_starting_with",
                                 ::datatools::properties,
                                 ::datatools::properties::erase_all_starting_with,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("erase_all_not_starting_with",
                                 ::datatools::properties,
                                 ::datatools::properties::erase_all_not_starting_with,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("erase_all_ending_with",
                                 ::datatools::properties,
                                 ::datatools::properties::erase_all_ending_with,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("erase_all_not_ending_with",
                                 ::datatools::properties,
                                 ::datatools::properties::erase_all_not_ending_with,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("clean",
                                 ::datatools::properties,
                                 ::datatools::properties::clean,
                                 std::string)
          .DR_CLASS_VOID_METHOD("clear",
                                ::datatools::properties,
                                ::datatools::properties::clear)
          .DR_CLASS_VOID_METHOD("reset",
                                ::datatools::properties,
                                ::datatools::properties::reset)
          .DR_CLASS_VOID_METHOD3("store_flag",
                                 ::datatools::properties,
                                 ::datatools::properties::store_flag,
                                 std::string,std::string,bool)
          .DR_CLASS_VOID_METHOD1("update_flag",
                                 ::datatools::properties,
                                 ::datatools::properties::update_flag,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("set_flag",
                                 ::datatools::properties,
                                 ::datatools::properties::set_flag,
                                 std::string)
          .DR_CLASS_VOID_METHOD1("unset_flag",
                                 ::datatools::properties,
                                 ::datatools::properties::unset_flag,
                                 std::string)
          .DR_CLASS_VOID_METHOD4("store_boolean",
                                 ::datatools::properties,
                                 ::datatools::properties::store_boolean,
                                 std::string,bool,std::string,bool)
          .DR_CLASS_VOID_METHOD4("store_integer",
                                 ::datatools::properties,
                                 ::datatools::properties::store_integer,
                                 std::string,int,std::string,bool)
          .DR_CLASS_VOID_METHOD4("store_real",
                                 ::datatools::properties,
                                 ::datatools::properties::store_real,
                                 std::string,double,std::string,bool)
          .DR_CLASS_VOID_METHOD4("store_string",
                                 ::datatools::properties,
                                 ::datatools::properties::store_string,
                                 std::string,std::string,std::string,bool)
          .DR_CLASS_VOID_METHOD3("change_boolean",
                                 ::datatools::properties,
                                 ::datatools::properties::change_boolean,
                                 std::string,bool,int)
          .DR_CLASS_VOID_METHOD3("change_integer",
                                 ::datatools::properties,
                                 ::datatools::properties::change_integer,
                                 std::string,int,int)
          .DR_CLASS_VOID_METHOD3("change_real",
                                 ::datatools::properties,
                                 ::datatools::properties::change_real,
                                 std::string,double,int)
          .DR_CLASS_VOID_METHOD3("change_string",
                                 ::datatools::properties,
                                 ::datatools::properties::change_string,
                                 std::string,std::string,int)
          .DR_CLASS_VOID_METHOD2("change_boolean",
                                 ::datatools::properties,
                                 ::datatools::properties::change_boolean_scalar,
                                 std::string,bool)
          .DR_CLASS_VOID_METHOD2("change_integer",
                                 ::datatools::properties,
                                 ::datatools::properties::change_integer_scalar,
                                 std::string,int)
          .DR_CLASS_VOID_METHOD2("change_real",
                                 ::datatools::properties,
                                 ::datatools::properties::change_real_scalar,
                                 std::string,double)
          .DR_CLASS_VOID_METHOD2("change_string",
                                 ::datatools::properties,
                                 ::datatools::properties::change_string_scalar,
                                 std::string,std::string)
          .DR_CLASS_VOID_METHOD3("change_boolean_vector",
                                 ::datatools::properties,
                                 ::datatools::properties::change_boolean_vector,
                                 std::string,bool,int)
          .DR_CLASS_VOID_METHOD3("change_integer_vector",
                                 ::datatools::properties,
                                 ::datatools::properties::change_integer_vector,
                                 std::string,int,int)
          .DR_CLASS_VOID_METHOD3("change_real_vector",
                                 ::datatools::properties,
                                 ::datatools::properties::change_real_vector,
                                 std::string,double,int)
          .DR_CLASS_VOID_METHOD3("change_string_vector",
                                 ::datatools::properties,
                                 ::datatools::properties::change_string_vector,
                                 std::string,std::string,int)
          .DR_CLASS_VOID_METHOD2("update_boolean",
                                 ::datatools::properties,
                                 ::datatools::properties::update_boolean,
                                 std::string,bool)
          .DR_CLASS_VOID_METHOD2("update_integer",
                                 ::datatools::properties,
                                 ::datatools::properties::update_integer,
                                 std::string,int)
          .DR_CLASS_VOID_METHOD2("update_real",
                                 ::datatools::properties,
                                 ::datatools::properties::update_real,
                                 std::string,double)
          .DR_CLASS_VOID_METHOD2("update_string",
                                 ::datatools::properties,
                                 ::datatools::properties::update_string,
                                 std::string,std::string)
          .DR_CLASS_METHOD1_CONST("has_flag",
                                  ::datatools::properties,
                                  ::datatools::properties::has_flag,
                                  bool,std::string)
          .DR_CLASS_METHOD1_CONST("fetch_boolean",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_boolean_scalar,
                                  bool,std::string)
          .DR_CLASS_METHOD2_CONST("fetch_boolean_vector",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_boolean_vector,
                                  bool,std::string,int)
          .DR_CLASS_METHOD1_CONST("fetch_integer",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_integer_scalar,
                                  int,std::string)
          .DR_CLASS_METHOD2_CONST("fetch_integer_vector",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_integer_vector,
                                  int,std::string,int)
          .DR_CLASS_METHOD1_CONST("fetch_real",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_real_scalar,
                                  double,std::string)
          .DR_CLASS_METHOD2_CONST("fetch_real_vector",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_real_vector,
                                  double,std::string,int)
          .DR_CLASS_METHOD1_CONST("fetch_string",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_string_scalar,
                                  std::string,std::string)
          .DR_CLASS_METHOD2_CONST("fetch_string_vector",
                                  ::datatools::properties,
                                  ::datatools::properties::fetch_string_vector,
                                  std::string,std::string,int)
          .DR_CLASS_VOID_METHOD2_CONST("write_configuration",
                                       ::datatools::properties,
                                       ::datatools::properties::write_configuration,
                                       std::string, uint32_t)
          .DR_CLASS_VOID_METHOD2("read_configuration",
                                 ::datatools::properties,
                                 ::datatools::properties::read_configuration,
                                 std::string, uint32_t)

          ;
        return;
      }

    } // end of namespace introspection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_PROPERTIES_REFLECT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
