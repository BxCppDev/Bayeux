//! \file  mygsl/math.h
//! \brief A composite function from two unary functors
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_MATH_H
#define MYGSL_MATH_H

// Standard library:
#include <string>
#include <map>
#include <iostream>

namespace mygsl {

  //! Typedef for a simple function C-style function :
  typedef double (*plain_function_type)(double);

  //! Typedef for a function C-style function with anonymous parameters :
  typedef double (*plain_function_with_parameters_type)(double, void*);

  //! brief Describe the signature of function
  enum plain_function_flag {
    PLAIN_FUNCTION_INVALID = 0,     //!< Invalid plain function
    PLAIN_FUNCTION_SIMPLE = 1,      //!< Simple plain function
    PLAIN_FUNCTION_WITH_PARAMS = 2  //!< Simple plain function
  };

  struct plain_function_entry {
    plain_function_entry();
    void reset();
    std::string         name; //< Registration name of the plain function
    plain_function_flag type; //!< Type of the plain function
    plain_function_type func_simple; //!< Address of the simple plain function
    plain_function_with_parameters_type func_with_params; //!< Address of the plain function with parameters
  };

  //! \brief Type alias for a dictionary of registered plain functions
  typedef std::map<std::string, plain_function_entry> plain_function_dict_type;

  //! Basic registration of some plain functions in a dictionary
  void basic_plain_function_registration(plain_function_dict_type & dict_);

  //! Registration of a plain simple function
  void register_plain_function_simple(plain_function_dict_type & dict_,
                                      const std::string & name_,
                                      const plain_function_type & func_);

  //! Registration of a plain function with parameters
  void register_plain_function_with_params(plain_function_dict_type & dict_,
                                           const std::string & name_,
                                           const plain_function_with_parameters_type & func_with_params_);

  //! Smart print of a dictionary of registered plain functions
  void tree_dump(const plain_function_dict_type & dict_,
                 std::ostream & out_ = std::clog,
                 const std::string & title_ = "",
                 const std::string & indent_ = "");

  plain_function_dict_type & grab_plain_function_dict();

  const plain_function_dict_type & get_plain_function_dict();

  bool plain_function_is_known(const std::string & name_);

  bool is_plain_function_simple(const std::string & name_);

  bool is_plain_function_with_params(const std::string & name_);

  plain_function_type get_simple_function(const std::string & name_);

} // end of namespace mygsl

#define MYGSL_MATH_REGISTER_FUNCTION(FunctionName, FunctionAddress, FunctionType) \
  {                                                                     \
    if (FunctionType == mygsl::PLAIN_FUNCTION_SIMPLE) {                 \
      register_plain_function_simple(mygsl::grab_plain_function_dict(), FunctionName, FunctionAddress); \
    } else if (FunctionType == mygsl::PLAIN_FUNCTION_WITH_PARAMS) {     \
      register_plain_function_with_params(mygsl::grab_plain_function_dict(), FunctionName, FunctionAddress); \
    }                                                                   \
  }                                                                     \
  /**/

#endif // MYGSL_MATH_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
