// math.cc
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

// Ourselves:
#include <mygsl/math.h>

// Standard library:
#include <cmath>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>

namespace mygsl {

  void tree_dump(const plain_function_dict_type & dict_,
                 std::ostream & out_,
                 const std::string & title_,
                 const std::string & indent_)
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << ':' << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Registered plain functions : "
         << '[' << dict_.size() << ']' << std::endl;

    for (plain_function_dict_type::const_iterator i = dict_.begin();
         i != dict_.end();
         i++) {
      plain_function_dict_type::const_iterator j = i;
      j++;
      out_ << indent_;
      if (j == dict_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      const plain_function_entry & pfe = i->second;
      out_ << "Function '" << i->first << " : type='";
      if (pfe.type == PLAIN_FUNCTION_SIMPLE) {
        out_ << "simple";
      } else if (pfe.type == PLAIN_FUNCTION_WITH_PARAMS) {
        out_ << "with parameters";
      }
      out_ << "'";
      out_ << " address=[";
      if (pfe.type == PLAIN_FUNCTION_SIMPLE) {
        out_ << pfe.func_simple;
      } else if (pfe.type == PLAIN_FUNCTION_WITH_PARAMS) {
        out_ << pfe.func_with_params;
      }
      out_ << "]";
      out_ << std::endl;
    }

    return;
  }

  bool has_plain_function(const plain_function_dict_type & dict_,
                          const std::string & name_)
  {
    plain_function_dict_type::const_iterator found = dict_.find(name_);
    if (found == dict_.end()) {
      return false;
    }
    if (found->second.type == PLAIN_FUNCTION_INVALID) {
      return false;
    }
    return true;
  }

  void register_plain_function_simple(plain_function_dict_type & dict_,
                                      const std::string & name_,
                                      const plain_function_type & func_)
  {
    // std::cerr << "DEVEL: register_plain_function_simple: "
    //           << "registering function with name='" << name_ << "'..."
    //           << std::endl;
    DT_THROW_IF(has_plain_function(dict_, name_),
                std::logic_error,
                "Dictionary already has a plain function named '" << name_ << "'!");
    {
      plain_function_entry dummy;
      dict_[name_] = dummy;
    }
    plain_function_entry & pfe = dict_.find(name_)->second;
    pfe.name = name_;
    pfe.type = PLAIN_FUNCTION_SIMPLE;
    pfe.func_simple = func_;
    pfe.func_with_params = 0;
    // std::cerr << "DEVEL: register_plain_function_simple: "
    //           << "registered function with name='" << pfe.name << "'"
    //           << std::endl;
    return;
  }

  void register_plain_function_with_params(plain_function_dict_type & dict_,
                                           const std::string & name_,
                                           const plain_function_with_parameters_type & func_with_params_)
  {
    std::string name(name_);
    DT_THROW_IF(has_plain_function(dict_, name),
                std::logic_error,
                "Dictionary already has a plain function named '" << name << "'!");
    {
      plain_function_entry dummy;
      dict_[name] = dummy;
    }
    plain_function_entry & pfe = dict_.find(name)->second;
    pfe.name = name;
    pfe.type = PLAIN_FUNCTION_WITH_PARAMS;
    pfe.func_simple = 0;
    pfe.func_with_params = func_with_params_;
    return;
  }

  void basic_plain_function_registration(plain_function_dict_type & dict_)
  {
    register_plain_function_simple(dict_, "cos",   std::cos);
    register_plain_function_simple(dict_, "acos",  std::acos);
    register_plain_function_simple(dict_, "sin",   std::sin);
    register_plain_function_simple(dict_, "asin",  std::asin);
    register_plain_function_simple(dict_, "tan",   std::tan);
    register_plain_function_simple(dict_, "atan",  std::atan);
    register_plain_function_simple(dict_, "exp",   std::exp);
    register_plain_function_simple(dict_, "log",   std::log);
    register_plain_function_simple(dict_, "log10", std::log10);
    register_plain_function_simple(dict_, "sqrt",  std::sqrt);
    register_plain_function_simple(dict_, "sinh",  std::sinh);
    register_plain_function_simple(dict_, "asinh", asinh);
    register_plain_function_simple(dict_, "cosh",  std::cosh);
    register_plain_function_simple(dict_, "acosh", acosh);
    register_plain_function_simple(dict_, "pow2",  gsl_pow_2);
    register_plain_function_simple(dict_, "pow3",  gsl_pow_3);
    register_plain_function_simple(dict_, "pow4",  gsl_pow_4);
    register_plain_function_simple(dict_, "pow5",  gsl_pow_5);
    register_plain_function_simple(dict_, "pow6",  gsl_pow_6);
    register_plain_function_simple(dict_, "pow7",  gsl_pow_7);
    register_plain_function_simple(dict_, "pow8",  gsl_pow_8);
    register_plain_function_simple(dict_, "pow9",  gsl_pow_9);
    // tree_dump(dict_, std::clog, "Plain function register: ", "[notice] ");
    return;
  }

  plain_function_dict_type & _plain_function_dict()
  {
    static boost::scoped_ptr<plain_function_dict_type> _dict;
    if (!_dict) {
      _dict.reset(new plain_function_dict_type);
      basic_plain_function_registration(*_dict);
    }
    return *_dict;
  }

  plain_function_entry::plain_function_entry()
  {
    type = PLAIN_FUNCTION_INVALID;
    func_simple = 0;
    func_with_params = 0;
    return;
  }

  void plain_function_entry::reset()
  {
    name.clear();
    type = PLAIN_FUNCTION_INVALID;
    func_simple = 0;
    func_with_params = 0;
    return;
  }

  plain_function_dict_type & grab_plain_function_dict()
  {
    return _plain_function_dict();
  }

  const plain_function_dict_type & get_plain_function_dict()
  {
    return const_cast<plain_function_dict_type &>(_plain_function_dict());
  }

  bool plain_function_is_known(const std::string & name_)
  {
    const plain_function_dict_type & dict = get_plain_function_dict();
    // std::cerr << "DEVEL: plain_function_is_known: dictionary size=[" << dict.size() << "]"
    //           << std::endl;
    // for (plain_function_dict_type::const_iterator i = dict.begin();
    //      i != dict.end();
    //      i++) {
    //   std::cerr << "DEVEL: plain_function_is_known: "
    //             << "registered function '" << i->first << "'"
    //             << std::endl;
    // }
    plain_function_dict_type::const_iterator found = dict.find(name_);
    if (found == dict.end()) {
      // std::cerr << "DEVEL: plain_function_is_known: "
      //           << "plain function with name='" << name_ << "' not found!"
      //           << std::endl;
      return false;
    }
    // std::cerr << "DEVEL: plain_function_is_known: name='" << name_ << "' "
    //           << "type=" << found->second.type << std::endl;
    return found->second.type != PLAIN_FUNCTION_INVALID;
  }

  bool is_plain_function_simple(const std::string & name_)
  {
    const plain_function_dict_type & dict = get_plain_function_dict();
    plain_function_dict_type::const_iterator found = dict.find(name_);
    if (found == dict.end()) {
      return false;
    }
    return found->second.type == PLAIN_FUNCTION_SIMPLE;
  }

  bool is_plain_function_with_params(const std::string & name_)
  {
    const plain_function_dict_type & dict = get_plain_function_dict();
    plain_function_dict_type::const_iterator found = dict.find(name_);
    if (found == dict.end()) {
      return false;
    }
    return found->second.type == PLAIN_FUNCTION_WITH_PARAMS;
  }

  plain_function_type get_simple_function(const std::string & name_)
  {
    const plain_function_dict_type & dict = get_plain_function_dict();
    plain_function_dict_type::const_iterator found = dict.find(name_);
    DT_THROW_IF(found == dict.end(), std::logic_error,
                "No plain function named '" << name_ << "'!");
    const plain_function_entry & pfe = found->second;
    DT_THROW_IF(pfe.type == PLAIN_FUNCTION_INVALID, std::logic_error,
                "Invalid plain function named '" << name_ << "'!");
    return pfe.func_simple;
  }

} // end of namespace mygsl
