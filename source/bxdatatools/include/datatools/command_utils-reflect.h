//! \file datatools/command_utils-reflect.h
#ifndef DATATOOLS_COMMAND_UTILS_REFLECT_H
#define DATATOOLS_COMMAND_UTILS_REFLECT_H

// Third party:
// - CAMP:
#include <camp/enum.hpp>

// This project:
#include <datatools/command_utils.h>
#include <datatools/properties.h>
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// Export the introspection code:
DR_CLASS_EXPORT(::datatools::command, "datatools::command")
DR_TYPE_EXPORT(::datatools::command::error_code_type, "datatools::command::error_code_type")
DR_CLASS_EXPORT(::datatools::command::returned_info, "datatools::command::returned_info")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::command, DT_UNUSED(tag_) )
      {
        // Declare and populate the reflection metaclass:
        DR_CLASS_DECLARE (::datatools::command)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::command::error_code_type, DT_UNUSED(tag_))
      {
        DR_ENUM_DECLARE(::datatools::command::error_code_type)
          .value("deprecated",                ::datatools::command::CEC_DEPRECATED)
          .value("stop",                      ::datatools::command::CEC_STOP)
          .value("continue",                  ::datatools::command::CEC_CONTINUE)
          .value("success",                   ::datatools::command::CEC_SUCCESS)
          .value("failure",                   ::datatools::command::CEC_FAILURE)
          .value("parsing_failure",           ::datatools::command::CEC_PARSING_FAILURE)
          .value("context_invalid",           ::datatools::command::CEC_CONTEXT_INVALID)
          .value("not_implemented",           ::datatools::command::CEC_NOT_IMPLEMENTED)
          .value("abort",                     ::datatools::command::CEC_ABORT)
          .value("scope_invalid",             ::datatools::command::CEC_SCOPE_INVALID)
          .value("command_invalid",           ::datatools::command::CEC_COMMAND_INVALID)
          .value("command_no_priviledge",     ::datatools::command::CEC_COMMAND_NO_PRIVILEDGE)
          .value("command_invalid_context",   ::datatools::command::CEC_COMMAND_INVALID_CONTEXT)
          .value("command_invalid_syntax",    ::datatools::command::CEC_COMMAND_INVALID_SYNTAX)
          .value("command_invalid_number_of_arguments", ::datatools::command::CEC_COMMAND_INVALID_NUMBER_OF_ARGUMENTS)
          .value("parameter_invalid",         ::datatools::command::CEC_PARAMETER_INVALID)
          .value("parameter_invalid_key",     ::datatools::command::CEC_PARAMETER_INVALID_KEY)
          .value("parameter_no_priviledge",   ::datatools::command::CEC_PARAMETER_NO_PRIVILEDGE)
          .value("parameter_invalid_context", ::datatools::command::CEC_PARAMETER_INVALID_CONTEXT)
          .value("parameter_invalid_type",    ::datatools::command::CEC_PARAMETER_INVALID_TYPE)
          .value("parameter_unset_value",     ::datatools::command::CEC_PARAMETER_UNSET_VALUE)
          .value("parameter_invalid_value",   ::datatools::command::CEC_PARAMETER_INVALID_VALUE)
          .value("parameter_invalid_unit",    ::datatools::command::CEC_PARAMETER_INVALID_UNIT)
         ;
        return;
      }

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::command::returned_info, DT_UNUSED(tag_) )
      {
        // Declare and populate the reflection metaclass for the 'datatools::command::returned_info' class:
        DR_CLASS_DECLARE (::datatools::command::returned_info)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_PROPERTY_GETTER_SETTER("error_code",
                                           ::datatools::command::returned_info::get_error_code,
                                           ::datatools::command::returned_info::set_error_code)
          .DR_CLASS_PROPERTY_GETTER_SETTER("error_message",
                                           ::datatools::command::returned_info::get_error_message,
                                           ::datatools::command::returned_info::set_error_message)
          .DR_CLASS_PROPERTY_GETTER_SETTER("output",
                                           ::datatools::command::returned_info::get_output,
                                           ::datatools::command::returned_info::set_output)
          .DR_CLASS_METHOD_CONST("has_error_message",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::has_error_message,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_success",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_success,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_special",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_special,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_failure",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_failure,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_stop",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_stop,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_deprecated",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_deprecated,
                                 bool)
          .DR_CLASS_METHOD_CONST("is_continue",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::is_continue,
                                 bool)
          .DR_CLASS_VOID_METHOD("set_stop",
                                ::datatools::command::returned_info,
                                ::datatools::command::returned_info::set_stop)
          .DR_CLASS_VOID_METHOD("set_continue",
                                ::datatools::command::returned_info,
                                ::datatools::command::returned_info::set_continue)
          .DR_CLASS_METHOD1_CONST("error_is",
                                  ::datatools::command::returned_info,
                                  ::datatools::command::returned_info::error_is,
                                  bool,
                                  ::datatools::command::error_code_type)
          .DR_CLASS_VOID_METHOD("reset",
                                ::datatools::command::returned_info,
                                ::datatools::command::returned_info::reset)
          .DR_CLASS_METHOD_CONST("has_output",
                                 ::datatools::command::returned_info,
                                 ::datatools::command::returned_info::has_output,
                                 bool)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_COMMAND_UTILS_REFLECT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
