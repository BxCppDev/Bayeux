//! \file  datatools/introspection/builder.h
//! \brief Method builder
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_INTROSPECTION_BUILDER_H
#define DATATOOLS_INTROSPECTION_BUILDER_H

// Standard library:
#include <string>
#include <iostream>

// Third Party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
// - Camp:
#include <camp/function.hpp>

// This project:
#include <datatools/introspection/method.h>
#include <datatools/ioutils.h>

namespace datatools {

  /// \brief Nested namespace of the Bayeux/datatools module library
  namespace introspection {

    // Function flags
    std::string function_builder_flag();
    std::string function_constness_flag();
    std::string function_description_tag();

    // General tags:
    std::string name_tag();
    std::string description_tag();
    std::string type_tag();
    std::string layout_tag();
    std::string vector_fixed_size_tag();
    std::string implicit_unit_tag();
    std::string explicit_unit_dimension_tag();
    std::string preferred_unit_tag();
    std::string default_value_tag();

    // Composed tag (format : "PREFIX.WHAT");
    std::string composed_tag(const std::string & prefix_, const std::string & what_);

    // Tag builders for function arguments:
    std::string function_arg_prefix(std::size_t iarg_);
    std::string function_arg_tag(std::size_t iarg_, const std::string & what_);

    // Tag builders for function returned parameter:
    std::string function_returned_prefix();
    std::string function_returned_tag(const std::string & what_);

    /*

     .DR_CLASS_TAG("arg[0].name", "x")
     .DR_CLASS_TAG("arg[0].description", "The length")
     .DR_CLASS_TAG("arg[0].type", "double")
     .DR_CLASS_TAG("arg[0].layout", "scalar")
     .DR_CLASS_TAG("arg[0].vector_fixed_size", 3)
     .DR_CLASS_TAG("arg[0].implicit_unit", "mm")
     .DR_CLASS_TAG("arg[0].explicit_unit_dimension", "length")
     .DR_CLASS_TAG("arg[0].preferred_unit", "cm")

     */

    /// \brief This class uses the reflection API to build the
    ///        description of a function or a class method. Special tags
    ///        are expected in the reflection API of the function/method in order
    ///        to generate the function introspective description.
    ///        The mapped function may have or not a unique returned value.
    ///        The mapped function can have an arbitrary number of input arguments.
    ///        Only scalar argument/returned values of simple types (bool,
    ///        integer, real, string) are supported for now.
    class method_builder
    {
    public:

      /// Default constructor
      method_builder();

      /// Constructor
      method_builder(const std::string & funcname_);

      /// Constructor
      method_builder(const std::string & classname_, const std::string & funcname_);

      /// Reset the builder
      void reset();

      /// Set the class name
      void set_classname(const std::string & classname_);

      /// Set the function name (if not provided, the name of the reflection function object
      /// will be used.
      void set_funcname(const std::string & funcname_);

      /// Set the function
      void set_function(const camp::Function & func_);

      /// Build a data description for a parameter
      void build_parameter_data_description(camp::Type param_type_,
                                            std::size_t param_index_,
                                            data_description & param_dd_);

      /// Build an input argument at given index
      void build_argument(std::size_t arg_index_, argument & arg_);

      /// Build the returned parameter
      void build_returned(argument & returned_);

      /// Build the description of the method from the reflection function object
      void build(method & method_);

      static void dump_func(const camp::Function & func_, std::ostream & out_ = std::clog);

    public:

      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:
      std::string _classname_; ///< Name of the class
      std::string _funcname_;  ///< Name of the function/method

      // Work:
      std::string _func_what_;
      std::string _func_full_name_;
      const camp::Function * _func_ = nullptr;

    };

    /// \brief This class uses a variable map from the command line to build a
    ///        list of  arguments for calling a function/method
    ///        through the reflection API.
    class method_args_builder
    {
    public:

      /// Default constructor
      method_args_builder(method & method_, const boost::program_options::variables_map & vmap_);

      /// Build the list of function arguments for the reflection API
      void build(camp::Args & args_);

    public:

      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:

      method * _method_ = nullptr;
      const boost::program_options::variables_map * _vmap_ = nullptr;
    };

  } // namespace introspection

} // namespace datatools

#endif // DATATOOLS_INTROSPECTION_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
