// datatools/introspection/builder.cc
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/introspection/builder.h>

// Third Party:
// - Boost:
#include <boost/lexical_cast.hpp>
// #include <boost/program_options/options_description.hpp>
// - Camp:
#include <camp/type.hpp>
#include <camp/detail/classmanager.hpp>
#include <camp/userobject.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/class.hpp>
#include <camp/errors.hpp>
#include <camp/enum.hpp>

// This project:
#include <datatools/introspection/argument.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/ui/validators.h>

namespace datatools {

  namespace introspection {

    std::string function_builder_flag()
    {
      return std::string("builder");
    }

    std::string function_constness_flag()
    {
      return std::string("const");
    }

    std::string function_description_tag()
    {
      return std::string("description");
    }

    std::string name_tag()
    {
      return std::string("name");
    }

    std::string description_tag()
    {
      return std::string("description");
    }

    std::string type_tag()
    {
      return std::string("type");
    }

    std::string layout_tag()
    {
      return std::string("layout");
    }

    std::string vector_fixed_size_tag()
    {
      return std::string("vector_fixed_size");
    }

    std::string implicit_unit_tag()
    {
      return std::string("implicit_unit");
    }

    std::string explicit_unit_dimension_tag()
    {
      return std::string("explicit_unit_dimension");
    }

    std::string preferred_unit_tag()
    {
      return std::string("preferred_unit");
    }

    std::string default_value_tag()
    {
      return std::string("default_value");
    }

    std::string composed_tag(const std::string & prefix_, const std::string & what_)
    {
      return prefix_ + "." + what_;
    }

    std::string function_arg_prefix(std::size_t iarg_)
    {
      return std::string("arg[") + boost::lexical_cast<std::string>(iarg_) + "]";
    }

    std::string function_arg_tag(std::size_t iarg_, const std::string & what_)
    {
      return composed_tag(function_arg_prefix(iarg_), what_);
    }

    std::string function_returned_prefix()
    {
      return std::string("returned");
    }

    std::string function_returned_tag(const std::string & what_)
    {
      return composed_tag(function_returned_prefix(), what_);
    }

    method_builder::method_builder()
    {
      return;
    }

    method_builder::method_builder(const std::string & funcname_)
    {
      set_funcname(funcname_);
      return;
    }

    method_builder::method_builder(const std::string & classname_, const std::string & funcname_)
    {
      set_classname(classname_);
      set_funcname(funcname_);
      return;
    }

    void method_builder::set_classname(const std::string & classname_)
    {
      _classname_ = classname_;
      return;
    }

    void method_builder::set_funcname(const std::string & funcname_)
    {
      _funcname_ = funcname_;
      return;
    }

    void method_builder::set_function(const camp::Function & func_)
    {
      _func_ = &func_;
      return;
    }

    void method_builder::reset()
    {
      _classname_.clear();
      _funcname_.clear();
      _func_what_.clear();
      _func_full_name_.clear();
      _func_ = nullptr;
      return;
    }

    void method_builder::build_parameter_data_description(camp::Type param_type_,
                                                          std::size_t param_index_,
                                                          data_description & param_dd_)
    {
      param_dd_.reset();
      std::string param_what = "returned";
      std::string param_tag_prefix;
      if (param_index_ == std::numeric_limits<std::size_t>::max()) {
        // parameter is the returned value
        param_tag_prefix = function_returned_prefix();
      } else {
        // parameter is an input argument
        param_tag_prefix = function_arg_prefix(param_index_);
        param_what = "argument #" + boost::lexical_cast<std::string>(param_index_);
      }
      if (param_type_ == camp::userType) {
        DT_THROW(std::logic_error,
                 "Unsupported user type for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
      }
      if (param_type_ == camp::arrayType) {
        DT_THROW(std::logic_error,
                 "Unsupported non scalar type for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
      }
      data_type   param_dt = DATA_TYPE_INVALID;
      data_layout param_layout = DATA_LAYOUT_SCALAR;
      std::string param_type_id;
      std::string param_type_tag   = composed_tag(param_tag_prefix, type_tag());
      std::string param_layout_tag = composed_tag(param_tag_prefix, layout_tag());
      // std::cerr << "DEVEL: " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "' : param type = " << param_type_ << std::endl;
      if (param_type_ == camp::boolType) {
        param_dt = DATA_TYPE_BOOLEAN;
        if (_func_->hasTag(param_type_tag)) {
          std::string type_label = _func_->tag(param_type_tag).to<std::string>();
          data_type check_dt = from_label_to_data_type(type_label);
          DT_THROW_IF(!is_boolean(check_dt), std::logic_error,
                      "Unmatching boolean type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
        }
      } else if (param_type_ == camp::intType) {
        // std::cerr << "DEVEL: " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "' : found intType" << std::endl;
        param_dt = DATA_TYPE_INT32;
        if (_func_->hasTag(param_type_tag)) {
          std::string type_label = _func_->tag(param_type_tag).to<std::string>();
          data_type check_dt = from_label_to_data_type(type_label);
          DT_THROW_IF(!is_integer(check_dt), std::logic_error,
                      "Unmatching integer type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
          param_dt = check_dt;
        }
      } else if (param_type_ == camp::realType) {
        param_dt = DATA_TYPE_DOUBLE;
        if (_func_->hasTag(param_type_tag)) {
          std::string type_label = _func_->tag(param_type_tag).to<std::string>();
          data_type check_dt = from_label_to_data_type(type_label);
          DT_THROW_IF(!is_real(check_dt), std::logic_error,
                      "Unmatching real type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
          param_dt = check_dt;
        }
      } else if (param_type_ == camp::stringType) {
        param_dt = DATA_TYPE_STRING;
        if (_func_->hasTag(param_type_tag)) {
          std::string type_label = _func_->tag(param_type_tag).to<std::string>();
          data_type check_dt = from_label_to_data_type(type_label);
          DT_THROW_IF(!is_string(check_dt), std::logic_error,
                      "Unmatching string type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
          param_dt = check_dt;
        }
      } else if (param_type_ == camp::enumType) {
        // std::cerr << "DEVEL: method_builder::build_parameter_data_description: "
        //           << "====> Enum Type"
        //           << "\n";
        param_dt = DATA_TYPE_ENUM;
        if (_func_->hasTag(param_type_tag)) {
          std::string type_id = _func_->tag(param_type_tag).to<std::string>();
          DT_THROW_IF(type_id.empty(), std::logic_error,
                      "Empty type identifier for enumeration type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
          param_type_id = type_id;
        } else {
          DT_THROW(std::logic_error,
                   "Missing type identifier for enumeration type tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
        }
      }
      if (_func_->hasTag(param_layout_tag)) {
        std::string param_layout_label = _func_->tag(param_layout_tag).to<std::string>();
        data_layout check_layout = DATA_LAYOUT_INVALID;
        DT_THROW_IF(!from_string(param_layout_label, check_layout), std::logic_error,
                    "Invalid layout tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
        param_layout = check_layout;
      }
      param_dd_.set_type(param_dt);
      if (!param_type_id.empty()) {
        param_dd_.set_type_id(param_type_id);
      }
      param_dd_.set_layout(param_layout);

      /*
        std::cerr << "DEVEL: " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "' : type   = " << to_string(param_dt) << std::endl;
        std::cerr << "DEVEL: " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "' : layout = " << to_string(param_layout) << std::endl;
      */
      if (param_layout == DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE) {
        std::string param_vector_fixed_size_tag = composed_tag(param_tag_prefix, vector_fixed_size_tag());
        DT_THROW_IF(_func_->hasTag(param_vector_fixed_size_tag),
                    std::logic_error,
                    "Missing vector fixed size tag for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
        std::size_t vfsize = _func_->tag(param_vector_fixed_size_tag).to<std::size_t>();
        DT_THROW_IF(vfsize == 0,
                    std::logic_error,
                    "Invalid vector fixed size for " << param_what << " in " << _func_what_ << " '" << _func_full_name_ << "'");
        param_dd_.set_vector_fixed_size(vfsize);
      }

      if (is_real(param_dt)) {
        // Unit support:
        unit_support_type usupport = UNIT_SUPPORT_INVALID;
        std::string param_implicit_unit_tag  = composed_tag(param_tag_prefix, implicit_unit_tag());
        std::string param_explicit_unit_dimension_tag  = composed_tag(param_tag_prefix, explicit_unit_dimension_tag());
        std::string param_preferred_unit_tag = composed_tag(param_tag_prefix, preferred_unit_tag());
        std::string param_implicit_unit;
        std::string param_explicit_unit_dimension;
        std::string param_preferred_unit;
        if (! is_valid(usupport) && _func_->hasTag(param_implicit_unit_tag)) {
          usupport = UNIT_SUPPORT_IMPLICIT_UNIT;
          param_implicit_unit = _func_->tag(param_implicit_unit_tag).to<std::string>();
        }
        if (! is_valid(usupport) && _func_->hasTag(param_explicit_unit_dimension_tag)) {
          usupport = UNIT_SUPPORT_EXPLICIT_DIMENSION;
          param_explicit_unit_dimension = _func_->tag(param_explicit_unit_dimension_tag).to<std::string>();
        }
        DT_LOG_TRACE(logging, "param_tag_prefix=" << param_tag_prefix);
        DT_LOG_TRACE(logging, "  usupport=" << usupport);
        DT_LOG_TRACE(logging, "  param_implicit_unit=" << param_implicit_unit);
        DT_LOG_TRACE(logging, "  param_explicit_unit_dimension=" << param_explicit_unit_dimension);
        if (! is_valid(usupport)) {
          usupport = UNIT_SUPPORT_NONE;
        }
        if (usupport != UNIT_SUPPORT_NONE) {
          if (_func_->hasTag(param_preferred_unit_tag)) {
            param_preferred_unit = _func_->tag(param_preferred_unit_tag).to<std::string>();
          }
        }
        DT_LOG_TRACE(logging, "  param_preferred_unit=" << param_preferred_unit);
        unit_info param_uinfo;
        if (usupport == UNIT_SUPPORT_IMPLICIT_UNIT) {
          DT_LOG_TRACE(logging, "  make_implicit_unit...");
          param_uinfo.make_implicit_unit(param_implicit_unit, param_preferred_unit);
        } else if (usupport == UNIT_SUPPORT_EXPLICIT_DIMENSION) {
          DT_LOG_TRACE(logging, "  make_explicit_unit_dimension...");
          param_uinfo.make_explicit_unit_dimension(param_explicit_unit_dimension, param_preferred_unit);
        } else {
          DT_LOG_TRACE(logging, "  make_none...");
          param_uinfo.make_none();
        }
        param_dd_.set_unit_info(param_uinfo);
      }
      param_dd_.initialize();
      return;
    }

    void method_builder::build_argument(std::size_t arg_index_, argument & arg_)
    {
      DT_LOG_TRACE_ENTERING(logging);
      arg_.reset();
      camp::Type arg_type = _func_->argType(arg_index_);
      std::string arg_name = std::string("arg") + boost::lexical_cast<std::string>(arg_index_);
      std::string arg_description = "";
      std::string arg_name_tag = function_arg_tag(arg_index_, name_tag());
      std::string arg_description_tag = function_arg_tag(arg_index_, description_tag());
      if (_func_->hasTag(arg_name_tag)) {
        arg_name = _func_->tag(arg_name_tag).to<std::string>();
        uint32_t validation_flags = datatools::NV_INSTANCE;
        DT_THROW_IF(!datatools::name_validation(arg_name, validation_flags),
                    std::logic_error,
                    "Invalid argument name '" << arg_name << "' in "
                    << _func_what_ << " '" << _func_full_name_ << "'!");
      }

      if (_func_->hasTag(arg_description_tag)) {
        arg_description = _func_->tag(arg_description_tag).to<std::string>();
      }
      arg_.set_name(arg_name);
      if (!arg_description.empty()) {
        arg_.set_description(arg_description);
      }
      datatools::introspection::data_description arg_dd;
      build_parameter_data_description(arg_type, arg_index_, arg_dd);
      arg_.set_data_description(arg_dd);
      arg_.set_access(ACCESS_INPUT);
      arg_.set_rank(arg_index_);
      arg_.initialize();
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    void method_builder::build_returned(argument & returned_)
    {
      DT_LOG_TRACE_ENTERING(logging);
      returned_.reset();
      camp::Type ret_type = _func_->returnType();
      std::string ret_name = "returned";
      std::string ret_description = "";
      std::string ret_name_tag = function_returned_tag(name_tag());
      std::string ret_description_tag = function_returned_tag(description_tag());
      if (_func_->hasTag(ret_name_tag)) {
        ret_name = _func_->tag(ret_name_tag).to<std::string>();
      }
      if (_func_->hasTag(ret_description_tag)) {
        ret_description = _func_->tag(ret_description_tag).to<std::string>();
      }
      returned_.set_name(ret_name);
      if (!ret_description.empty()) {
        returned_.set_description(ret_description);
      }
      datatools::introspection::data_description ret_dd;
      build_parameter_data_description(ret_type, std::numeric_limits<std::size_t>::max(), ret_dd);
      returned_.set_data_description(ret_dd);
      returned_.set_access(ACCESS_OUTPUT);
      returned_.initialize();
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    void method_builder::build(method & method_)
    {
      DT_LOG_TRACE_ENTERING(logging);
      DT_THROW_IF(_func_ == nullptr, std::logic_error, "No function from the reflection API is provided!");
      if (datatools::logger::is_debug(logging)) {
        dump_func(*_func_, std::cerr);
      }

      method_.reset();
      if (_funcname_.empty()) {
        _funcname_ = _func_->name();
      }
      _func_what_ = "function";
      if (!_classname_.empty()) {
        _func_full_name_ += _classname_ + "::";
        _func_what_ = "method";
      }
      _func_full_name_ += _funcname_;
      DT_THROW_IF(!_func_->hasTag(function_builder_flag()),
                  std::logic_error,
                  _func_what_ << "'" << _func_full_name_ << "'"
                  << " cannot be mapped through the reflective command interface !");
      method_.set_name(_funcname_);
      if (!_classname_.empty()) {
        method_.set_type_id(_classname_);
      }
      if (_func_->hasTag(function_constness_flag())) {
        method_.set_constness(true);
      }
      if (_func_->hasTag(function_description_tag())) {
        // std::cerr << "DEVEL: datatools::introspection::method_builder::build: "
        //           << "Found 'function_description_tag' for "
        //           << _func_what_ << " '" << _func_full_name_ << "'!" << std::endl;
        std::string func_desc;
        func_desc = _func_->tag(function_description_tag()).to<std::string>();
        // std::cerr << "DEVEL: datatools::introspection::method_builder::build: "
        //           << "Function description = '" << func_desc << "' for "
        //           << _func_what_ << " '" << _func_full_name_ << "'!" << std::endl;
        if (!func_desc.empty()) {
          // std::cerr << "DEVEL: datatools::introspection::method_builder::build: "
          //           << "Set terse description for "
          //           << _func_what_ << " '" << _func_full_name_ << "'!" << std::endl;
          method_.set_terse_description(func_desc);
        } else {
          DT_THROW(std::logic_error, "Empty function description tag for "
                   << _func_what_ << " '" << _func_full_name_ << "'!");
        }
      }
      // DT_LOG_DEBUG(logging, "Method build - step 1 done.");

      // Input arguments:
      std::size_t nargs = _func_->argCount();
      if (nargs) {
        DT_LOG_DEBUG(logging, "Build input arguments...");
        for (std::size_t iarg = 0; iarg < nargs; iarg++) {
          datatools::introspection::argument arg;
          build_argument(iarg, arg);
          method_.add_argument(arg);
        }
      }
      //DT_LOG_DEBUG(logging, "Method build - step 2 done.");

      // Returned type:
      camp::Type ret_type = _func_->returnType();
      if (ret_type != camp::noType) {
        DT_LOG_DEBUG(logging, "Build return type...");
        datatools::introspection::argument ret;
        // ret.set_rank(method_.NARG);
        build_returned(ret);
        method_.add_argument(ret);
      }
      //DT_LOG_DEBUG(logging, "Method build - step 3 done.");

      if (datatools::logger::is_debug(logging)) {
        method_.tree_dump(std::cerr, "Reflective command description : ", "[debug] ");
      }
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    // static
    void method_builder::dump_func(const camp::Function & func_, std::ostream & out_)
    {
      out_ << "CAMP API Function: '" << func_.name() << "'\n";
      out_ << "|-- Number of arguments : " << func_.argCount() << "\n";
      for (std::size_t iarg = 0; iarg < func_.argCount(); iarg++) {
        out_ << "|   ";
        if (iarg == func_.argCount() - 1) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Argument #" << iarg << " type : " << func_.argType(iarg);
        out_ << std::endl;
      }
      out_ << "`-- Return type : " << func_.returnType() << "\n";
      return;
    }

    method_args_builder::method_args_builder(method & method_, const boost::program_options::variables_map & vmap_)
      : _method_(&method_), _vmap_(&vmap_)
    {
      return;
    }

    void method_args_builder::build(camp::Args & args_)
    {
      DT_LOG_TRACE_ENTERING(logging);
      // Input arguments:
      std::vector<std::string> input_arg_names;
      _method_->build_list_of_input_argument_names(input_arg_names);

      for (std::size_t iarg = 0; iarg < input_arg_names.size(); iarg++) {
        DT_LOG_DEBUG(logging, "Processing method input argument/option at rank [" << iarg << "]...");

        const argument & arg = _method_->get_argument_by_rank(iarg);
        std::string arg_name = arg.get_name();
        data_type arg_type = arg.get_data_description().get_type();
        std::string arg_type_id;
        if (arg.get_data_description().has_type_id()) {
          arg_type_id = arg.get_data_description().get_type_id();
        }
        DT_LOG_DEBUG(logging, "  argument type = [" << arg_type << "]");
        camp::Value arg_val;
        bool found_arg = false;
        if ((*_vmap_).count(arg_name)) {
          DT_LOG_DEBUG(logging, "  found argument with name '" << arg_name << "' in the vmap");
          found_arg = true;
        }
        if (!found_arg) {
          // XXX Handle default arg value
          // if (arg.has_default_value_str()) {
          //   arg_value_repr = arg.get_default_value_str();
          // }
        }
        DT_LOG_DEBUG(logging, "Processing argument/option '" << arg.get_name() << "'...");
        if (! found_arg) {
          DT_THROW(std::logic_error, "Missing input argument/option '" << arg_name << "' at rank [" << iarg << "]!");
        }

        if (is_boolean(arg_type)) {
          DT_LOG_DEBUG(logging, "Processing boolean argument/option '" << arg_name << "'...");
          bool boolean_value = (*_vmap_)[arg_name].as<datatools::ui::wrapped_boolean>().value;
          arg_val = boolean_value;
        }

        if (is_integer(arg_type)) {
          DT_LOG_DEBUG(logging, " Processing integer argument/option '" << arg_name << "'...");
          int int_value = (*_vmap_)[arg_name].as<datatools::ui::wrapped_integer>().value;;
          arg_val = int_value;
        }

        if (is_real(arg_type)) {
          DT_LOG_DEBUG(logging, " Processing real argument/option '" << arg_name << "'...");
          if (arg.get_data_description().is_dimensionless()) {
            DT_LOG_DEBUG(logging, " Processing real without unit from argument/option '" << arg_name << "'...");
            datatools::ui::wrapped_real_without_unit wrwou
              = (*_vmap_)[arg_name].as<datatools::ui::wrapped_real_without_unit>();
            double a_value  = wrwou.value;
            DT_LOG_DEBUG(logging, " Parsed real without unit = [" << a_value << "]");
            arg_val = a_value;
          } else {
            DT_LOG_DEBUG(logging, " Processing real with unit from argument/option '" << arg_name << "'...");
            datatools::ui::wrapped_real_with_unit wrwu
              = (*_vmap_)[arg_name].as<datatools::ui::wrapped_real_with_unit>();
            double      a_value     = wrwu.value;
            bool        a_normal    = wrwu.normal;
            std::string unit_symbol = wrwu.unit_symbol;
            std::string unit_label  = wrwu.unit_label;
            DT_LOG_DEBUG(logging, " Value is       = '" << a_value << "'");
            DT_LOG_DEBUG(logging, " Normal is      = '" << a_normal << "'");
            DT_LOG_DEBUG(logging, " Unit symbol is = '" << unit_symbol << "'");
            DT_LOG_DEBUG(logging, " Unit label is  = '" << unit_label  << "'");
            const unit_info & uinfo = arg.get_data_description().get_unit_info();
            if (uinfo.has_implicit_unit()) {
              DT_LOG_DEBUG(logging, " Real argument with implicit unit '" << uinfo.get_implicit_unit_symbol() << "'");
              bool check_dimension = true;
              if (!a_normal && unit_symbol.empty()) {
                check_dimension = false;
              }
              const datatools::units::unit & u =
                datatools::units::registry::const_system_registry().get_unit_from_symbol(uinfo.get_implicit_unit_symbol());
              if (check_dimension) {
                DT_THROW_IF(u.get_dimension_label() != unit_label,
                            std::logic_error,
                            "Invalid unit '" << unit_symbol << "' (expected unit dimension '" << u.get_dimension_label() << "')!");
              }
              // Set the real value with implicit unit:
              if (a_normal) {
                arg_val = (a_value / u.get_value());
              } else {
                arg_val = a_value;
              }
            } else if (uinfo.has_explicit_unit_dimension()) {
              DT_LOG_DEBUG(logging, " Real argument with explicit unit dimension '" << uinfo.get_explicit_unit_dimension_label() << "'");
              bool check_dimension = true;
              if (!a_normal && unit_symbol.empty()) {
                check_dimension = false;
              }
              if (check_dimension) {
                DT_THROW_IF(uinfo.get_explicit_unit_dimension_label() != unit_label,
                            std::logic_error,
                            "Invalid unit '" << unit_symbol << "' (expected unit dimension '" << uinfo.get_explicit_unit_dimension_label() << "')!");
              }
              // Set the real value with explicit unit:
              arg_val = a_value;
            }
          }
        }

        if (is_enum(arg_type)) {
          // XXX
          DT_LOG_DEBUG(logging, " Processing enumeration argument/option '" << arg_name << "'...");
          std::string enum_str = (*_vmap_)[arg_name].as<std::string>();
          const camp::Enum & metaEnum = camp::enumByName(arg_type_id);
          DT_THROW_IF(! metaEnum.hasName(enum_str),
                      std::logic_error,
                      "Invalid enumeration value '" << enum_str << "' (enumeration type '" << arg_type_id << "')!");
          // arg_val = metaEnum.value(enum_str);
          arg_val = enum_str;
        }

        if (is_string(arg_type)) {
          DT_LOG_DEBUG(logging, " Processing string argument/option '" << arg_name << "'...");
          arg_val = (*_vmap_)[arg_name].as<std::string>();
        }

        args_ += arg_val;
      }
      if (datatools::logger::is_debug(logging)) {
        for (std::size_t i = 0; i < args_.count(); i++) {
          DT_LOG_DEBUG(logging, "arg[" << i << "] = " << args_[i]);
        }
      }
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

  } // namespace introspection

} // namespace datatools
