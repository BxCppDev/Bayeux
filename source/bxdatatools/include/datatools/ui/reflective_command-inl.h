/// \file datatools/ui/reflective_command-inl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-07-12
 * Last modified : 2016-07-12
 */

#ifndef DATATOOLS_UI_REFLECTIVE_COMMAND_INL_H
#define DATATOOLS_UI_REFLECTIVE_COMMAND_INL_H

// This project:
#include <datatools/detail/reflection_export.h>

// Third Party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
// - Camp:
#include <camp/classget.hpp>
#include <camp/userobject.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/class.hpp>
#include <camp/errors.hpp>
#include <camp/enum.hpp>
#include <camp/type.hpp>

// This project:
#include <datatools/introspection/builder.h>
#include <datatools/introspection/argument.h>
#include <datatools/ui/wrappers.h>
#include <datatools/ui/validators.h>
#include <datatools/units.h>

namespace datatools {

  namespace ui {

    template <class Type>
    reflective_command<Type>::reflective_command()
      : target_command<Type>()
    {
      return;
    }

    template <class Type>
    reflective_command<Type>::reflective_command(Type & target_,
                                                 const std::string & func_name_,
                                                 const std::string & name_,
                                                 const std::string & description_,
                                                 const version_id & vid_)
      : target_command<Type>(target_,
                             (name_.empty() ? func_name_ : name_),
                             description_, vid_)
    {
      set_func_name(func_name_);
      return;
    }

    template <class Type>
    reflective_command<Type>::~reflective_command()
    {
      if (this->reflective_command<Type>::is_initialized()) {
        this->reflective_command<Type>::reset();
      }
      return;
    }

    template <class Type>
    void reflective_command<Type>::set_func_name(const std::string & func_name_)
    {
      _func_name_ = func_name_;
      return;
    }

    template <class Type>
    void reflective_command<Type>::_init(const datatools::properties & config_)
    {
      datatools::logger::priority logging = this->reflective_command<Type>::get_logging_priority();
      this->base_command::_init(config_);
      bool forbid_short_ui_options = false;
      std::string classname(datatools::detail::reflection::guid<Type>());
      if (datatools::logger::is_debug(logging)) {
        DT_LOG_DEBUG(logging, "classname = '" << classname << "'");
        DT_LOG_DEBUG(logging, "List of registered metaclasses:");
        for (std::size_t i = 0; i < camp::classCount(); i++) {
          const camp::Class & metaclass = camp::classByIndex(i);
          DT_LOG_DEBUG(logging, "Class: '" << metaclass.name() << "'");
        }
      }

      try {
        camp::classByName(classname);
      } catch (std::exception & error) {
        DT_THROW(std::logic_error,
                 "No class '" << classname << "' with reflection support: " << error.what());
      }
      const camp::Class & target_class = camp::classByName(classname);

      std::string funcname = _func_name_;
      if (funcname.empty()) {
        funcname = this->reflective_command<Type>::get_name();
      }

      DT_THROW_IF(!target_class.hasFunction(funcname),
                  std::logic_error,
                  "Class '" << classname << "' has no function '" << funcname << "'!");
      _func_ = &target_class.function(funcname);

      /// Build the method from the Camp API based reflexion + specific tags:
      DT_LOG_TRACE(logging, "Builder...");
      datatools::introspection::method_builder builder;
      builder.logging = logging;
      builder.set_classname(classname);
      builder.set_funcname(funcname);
      builder.set_function(*_func_);
      builder.build(_method_);
      DT_LOG_TRACE(logging, "Method is built.");
      if (datatools::logger::is_trace(logging)) {
        _method_.tree_dump(std::clog, "Reflective command description : ", "[trace]: ");
      }
      // _method_.tree_dump(std::cerr, "Method : ", "DEVEL: ");
      if (!base_command::has_display_name()) {
        if (_method_.has_display_name()) {
          base_command::set_display_name(_method_.get_display_name());
        }
      }
      if (!base_command::has_terse_description()) {
        if (_method_.has_terse_description()) {
          base_command::set_terse_description(_method_.get_terse_description());
        }
      }
      base_command::opts_type & opts
        = this->reflective_command<Type>::_grab_opts();

      // Positional arguments description:
      base_command::args_type & args
        = this->reflective_command<Type>::_grab_args();

      // Options description:
      namespace po = boost::program_options;
      po::options_description_easy_init easy_init
        = opts.add_options();

      // Input arguments:
      std::vector<std::string> input_arg_names;
      _method_.build_list_of_input_argument_names(input_arg_names);

      // Output arguments:
      std::vector<std::string> output_arg_names;
      _method_.build_list_of_output_argument_names(output_arg_names);
      DT_THROW_IF(output_arg_names.size() > 1,
                  std::logic_error,
                  "Class '" << classname << "' function '" << funcname << "' has too many output parameters!");

      for (std::size_t iarg = 0; iarg < input_arg_names.size(); iarg++) {
        const datatools::introspection::argument & arg = _method_.get_argument_by_rank(iarg);
        std::string arg_name = arg.get_name();
        std::string arg_value_name = "value";
        std::string arg_help = "";
        if (arg.has_description()) {
          arg_help = arg.get_description();
        }
        if (arg_help.back() != '\n') arg_help += '\n';
        const datatools::introspection::data_description & arg_dd = arg.get_data_description();
        datatools::introspection::data_type dd_type = arg_dd.get_type();
        std::string dd_type_id;

        if (arg_dd.has_type_id()) {
          dd_type_id = arg_dd.get_type_id();
        }
        std::string type_label;
        std::string type_details;
        if (datatools::introspection::is_boolean(dd_type)) {
          type_label = "boolean";
        } else if (datatools::introspection::is_integer(dd_type)) {
          type_label = "integer";
        } else if (datatools::introspection::is_string(dd_type)) {
          type_label = "string";
        } else if (datatools::introspection::is_real(dd_type)) {
          type_label = "real";
          if (arg_dd.is_dimensionless()) {
            type_details = "dimensionless";
          } else {
            std::ostringstream type_details_oss;
            type_details_oss << "dimension: '" << arg_dd.get_unit_dimension_label() << "'";
            if (arg_dd.has_preferred_unit()) {
              type_details_oss << ", preferred unit: '" << arg_dd.get_preferred_unit_symbol() << "'";
            }
            type_details = type_details_oss.str();
          }
        } else if (datatools::introspection::is_enum(dd_type)) {
          type_label = "enumeration";
          if (!dd_type_id.empty()) {
            type_details = dd_type_id;
          }
        }

        // Argument description :
        std::ostringstream rang_desc;
        rang_desc << "Rank: " << iarg << "\n";
        arg_help += rang_desc.str();

        std::ostringstream type_desc;
        type_desc << "Type: '" << type_label << "'";
        if (!type_details.empty()) {
          type_desc << " (" << type_details << ")";
        }
        type_desc << "\n";
        if (type_desc.str().length()) {
          arg_help += type_desc.str();
        }
        if (datatools::introspection::is_boolean(dd_type)) {
          easy_init(arg_name.c_str(),
                    po::value<datatools::ui::wrapped_boolean>()
                    ->value_name(arg_value_name),
                    arg_help.c_str()
                    );
        } else if (datatools::introspection::is_integer(dd_type)) {
          easy_init(arg_name.c_str(),
                    po::value<datatools::ui::wrapped_integer>()
                    ->value_name(arg_value_name),
                    arg_help.c_str()
                    );
          forbid_short_ui_options = true;
        } else if (datatools::introspection::is_string(dd_type)) {
          easy_init(arg_name.c_str(),
                    po::value<std::string>()
                    ->value_name(arg_value_name),
                    arg_help.c_str()
                    );
        } else if (datatools::introspection::is_real(dd_type)) {
          if (arg_dd.is_dimensionless()) {
            easy_init(arg_name.c_str(),
                      po::value<datatools::ui::wrapped_real_without_unit>()
                      ->value_name(arg_value_name),
                      arg_help.c_str()
                      );
          } else {
            easy_init(arg_name.c_str(),
                      po::value<datatools::ui::wrapped_real_with_unit>()
                      ->value_name(arg_value_name),
                      arg_help.c_str()
                      );
          }
          forbid_short_ui_options = true;
        } else if (datatools::introspection::is_enum(dd_type)) {
          if (!dd_type_id.empty()) {
            std::ostringstream supported_value_desc;
            supported_value_desc << "Allowed values are:\n";
            const camp::Enum & metaenum = camp::enumByName(dd_type_id);
            DT_LOG_TRACE(logging, "Enum name = '" << metaenum.name());
            for (std::size_t ivalue = 0; ivalue < metaenum.size(); ivalue++) {
              const camp::Enum::Pair & value_pair = metaenum.pair(ivalue);
              supported_value_desc << "  - \"" << value_pair.name << "\"\n";
            }
            if (supported_value_desc.str().length()) {
              arg_help += supported_value_desc.str();
            }
          }
          easy_init(arg_name.c_str(),
                    po::value<std::string>()
                    ->value_name(arg_value_name),
                    arg_help.c_str());
        } else {
          DT_THROW(std::logic_error, "Class '" << classname << "' function '" << funcname << "': "
                   << "unsupported data type for input argument #" << iarg << "!");
        }

        // Positional:
        args.add(arg_name.c_str(), 1);
      }

      if (forbid_short_ui_options) {
        this->base_command::_forbid_short_ui_options();
      }

      return;
    }

    template <class Type>
    void reflective_command<Type>::_fini()
    {
      // datatools::logger::priority logging = this->reflective_command<Type>::get_logging_priority();
      _method_.reset();
      _func_ = nullptr;
      _func_name_.clear();
      this->base_command::_fini();
      return;
    }

    template <class Type>
    void reflective_command<Type>::_run(datatools::command::returned_info & cri_,
                                        uint32_t /*flags_*/)
    {
      datatools::logger::priority logging = this->reflective_command<Type>::get_logging_priority();
      DT_LOG_TRACE_ENTERING(logging);
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      try {
        DT_LOG_TRACE(logging, "Entering try block...");
        camp::UserObject targetObj(this->reflective_command<Type>::_grab_target());
        if (!_func_->callable(targetObj)) {
          cri_.set_error_code(datatools::command::CEC_CONTEXT_INVALID);
        }
        DT_LOG_TRACE(logging,
                     "Building args from the vmap...");
        camp::Args args;
        datatools::introspection::method_args_builder arg_builder(_method_,
                                                                  this->reflective_command<Type>::_grab_vmap());
        arg_builder.logging = logging;
        arg_builder.build(args);
        DT_LOG_TRACE(logging,
                     "Calling function '" << _func_->name() << "'...");
        camp::Value returned_value = _func_->call(targetObj, args);
        if (returned_value.type() != camp::noType) {
          DT_LOG_TRACE(logging,
                       "Returned value = '" << returned_value << "'...");
          repr(std::clog, returned_value, _method_.get_unique_returned());
          std::clog << std::endl;
        }
      } catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  this->reflective_command<Type>::get_name() + ": " + error.what());
      }

      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    // static
    template <class Type>
    void reflective_command<Type>::repr(std::ostream & out_,
                                        const camp::Value & value_,
                                        const datatools::introspection::argument & arg_)
    {
      const datatools::introspection::data_description & arg_dd = arg_.get_data_description();
      if (value_.type() == camp::boolType) {
        bool value = value_;
        datatools::io::write_boolean(out_, value, true);
      } else if (value_.type() == camp::intType) {
        int value = value_;
        datatools::io::write_integer(out_, value);
      } else if (value_.type() == camp::realType) {
        double value = value_;
        if (arg_dd.is_dimensionless()) {
          datatools::io::write_real_number(out_,
                                           value,
                                           datatools::io::REAL8_PRECISION);
        } else {
          std::string display_unit;
          if (arg_dd.has_preferred_unit()) {
            display_unit = arg_dd.get_preferred_unit_symbol();
          } else if (arg_dd.has_implicit_unit()) {
            display_unit = arg_dd.get_implicit_unit_symbol();
          } else if (arg_dd.has_explicit_unit_dimension()) {
            const std::string & unit_dimension = arg_dd.get_explicit_unit_dimension_label();
            display_unit = datatools::units::get_default_unit_symbol_from_label(unit_dimension);
          }
          if (arg_dd.has_implicit_unit()) {
            double unit_value = datatools::units::get_unit(arg_dd.get_implicit_unit_symbol());
            value *= unit_value;
          }
          datatools::io::write_real_number(out_,
                                           value,
                                           datatools::io::REAL8_PRECISION,
                                           display_unit,
                                           "",
                                           1.0,
                                           datatools::io::writer_tilda_sep);
        }
      } else if (value_.type() == camp::stringType) {
        std::string value = value_;
        datatools::io::write_quoted_string(std::clog, value);
      } else if (value_.type() == camp::enumType) {
        std::clog << value_;
      } else {
        DT_THROW(std::logic_error, "Unsupported CAMP type=[" << value_.type() << "]!");
      }
      return;
    }

  } // end of namespace ui

} // end of namespace datatools

#endif // DATATOOLS_UI_REFLECTIVE_COMMAND_INL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
