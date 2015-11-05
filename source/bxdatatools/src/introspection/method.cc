// datatools/introspection/method.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

// Ourselves:
#include <datatools/introspection/method.h>

// Standard library:
#include <string>
#include <sstream>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

// This project:
#include <datatools/exception.h>

namespace datatools {

  namespace introspection {

    // Tags for accessing the corresponding indices of resource_set :
    struct argument_tag_rank {};
    struct argument_tag_name {};

    //! Multi index container of resources
    typedef boost::multi_index_container<
      argument,
      boost::multi_index::indexed_by<
        // The first key is the unique argument rank:
        boost::multi_index::ordered_unique<
          boost::multi_index::tag<argument_tag_rank>,
          boost::multi_index::const_mem_fun<argument,
                                            std::size_t,
                                            &argument::get_rank
                                            >
          >,

        // The second key is the unique argument name:
        boost::multi_index::ordered_unique<
          boost::multi_index::tag<argument_tag_name>,
          boost::multi_index::const_mem_fun<argument,
                                            const std::string &,
                                            &argument::get_name
                                            >
          >
        >
      > argument_set;

    typedef argument_set::index<argument_tag_rank>::type argument_set_by_rank;
    typedef argument_set::index<argument_tag_name>::type argument_set_by_name;

    struct method::pimpl_type
    {
      //! Default constructor
      pimpl_type();
      argument_set arguments; //!< Set of arguments
    };

    method::pimpl_type::pimpl_type()
    {
      return;
    }

    method::method()
    {
      _priv_.reset(new pimpl_type);
      return;
    }

    method::~method()
    {
      _priv_.reset();
      return;
    }

    bool method::has_constness() const
    {
      return _constness_;
    }

    bool method::is_constness() const
    {
      return _constness_.get();
    }

    void method::set_constness(bool c_)
    {
      _constness_ = c_;
      return;
    }

    bool method::has_type_id() const
    {
      return _type_id_;
    }

    void method::set_type_id(const std::string & tid_)
    {
      _type_id_ = tid_;
      return;
    }

    const std::string & method::get_type_id() const
    {
      return _type_id_.get();
    }

    bool method::has_arguments() const
    {
      return _priv_->arguments.size() > 0;
    }

    void method::add_argument(const argument & arg_, const std::string & arg_name_)
    {
      argument arg = arg_;
      if (!arg.has_rank()) {
        arg.set_rank(_priv_->arguments.size());
      }
      if (!arg.has_name()) {
        if (arg_name_.empty()) {
          std::ostringstream arg_name_oss;
          arg_name_oss << "arg" << _priv_->arguments.size();
          arg.set_name(arg_name_oss.str());
        } else {
          arg.set_name(arg_name_);
        }
      }
      DT_THROW_IF(!arg_.is_valid(), std::logic_error,
                  "In method '" << this->get_name() << "' "
                  << "argument '" << arg.get_name() << "' (at rank ["
                  << arg.get_rank() << "]) is not valid!");
      DT_THROW_IF(has_argument_by_rank(arg.get_rank()),
                  std::logic_error,
                  "In method '" << this->get_name() << "' "
                  << "argument rank [" << arg.get_rank() << "] is already used!");
      DT_THROW_IF(has_argument_by_name(arg.get_name()),
                  std::logic_error,
                  "In method '" << this->get_name() << "' "
                  << "argument name '" << arg.get_name() << "' is already used!");
      arg.set_method(*this);
      _priv_->arguments.insert(arg);
      return;
    }

    bool method::has_argument_by_rank(std::size_t rank_) const
    {
      const argument_set_by_rank & rank_index = _priv_->arguments.get<argument_tag_rank>();
      argument_set_by_rank::const_iterator found = rank_index.find(rank_);
      return found != rank_index.end();
    }

    bool method::has_argument_by_name(const std::string & name_) const
    {
      const argument_set_by_name & name_index = _priv_->arguments.get<argument_tag_name>();
      argument_set_by_name::const_iterator found = name_index.find(name_);
      return found != name_index.end();
    }

    const argument & method::get_argument_by_name(const std::string & name_) const
    {
      const argument_set_by_name & name_index = _priv_->arguments.get<argument_tag_name>();
      argument_set_by_name::const_iterator found = name_index.find(name_);
      DT_THROW_IF(found == name_index.end(), std::logic_error,
                  "Cannot find argument with name='" << name_ << "'!");
      return *found;
    }

    const argument & method::get_argument_by_rank(std::size_t rank_) const
    {
      const argument_set_by_rank & rank_index = _priv_->arguments.get<argument_tag_rank>();
      argument_set_by_rank::const_iterator found = rank_index.find(rank_);
      DT_THROW_IF(found == rank_index.end(), std::logic_error,
                  "Cannot find argument with rank=[" << rank_ << "]!");
      return *found;
    }

    void method::build_list_of_input_argument_names(std::vector<std::string> & input_arg_names_) const
    {
      input_arg_names_.clear();
      const argument_set_by_name & name_index = _priv_->arguments.get<argument_tag_name>();
      for (argument_set_by_name::const_iterator iarg = name_index.begin();
           iarg != name_index.end();
           iarg++) {
        const argument & arg = *iarg;
        if (arg.is_input()) {
          input_arg_names_.push_back(arg.get_name());
        }
      }
      return;
    }

    void method::build_list_of_output_argument_names(std::vector<std::string> & output_arg_names_) const
    {
      output_arg_names_.clear();
      const argument_set_by_name & name_index = _priv_->arguments.get<argument_tag_name>();
      for (argument_set_by_name::const_iterator iarg = name_index.begin();
           iarg != name_index.end();
           iarg++) {
        const argument & arg = *iarg;
        if (arg.is_output()) {
          output_arg_names_.push_back(arg.get_name());
        }
      }
      return;
    }

    bool method::is_valid() const
    {
      if (!has_name()) return false;
      return true;
    }

    void method::initialize()
    {
      const datatools::properties dummy;
      initialize(dummy);
      return;
    }

    void method::initialize(const datatools::properties & config_)
    {
      this->datatools::enriched_base::initialize(config_, false);

      if (!has_constness()) {
        if (config_.has_key("constness")) {
          set_constness(config_.fetch_boolean("constness"));
        }
      }

      if (!has_type_id()) {
        if (config_.has_key("type_id")) {
          set_type_id(config_.fetch_string("type_id"));
        }
      }

      if (!has_arguments()) {
        std::vector<std::string> arg_names;
        if (config_.has_key("arguments")) {
          config_.fetch("arguments", arg_names);
        }
        for (int iag = 0; iag < (int) arg_names.size(); iag++) {
          const std::string & arg_name = arg_names[iag];
          std::ostringstream arg_config_prefix;
          arg_config_prefix << "arguments" << '.' << arg_name << '.';
          datatools::properties arg_config;
          config_.export_and_rename_starting_with(arg_config, arg_config_prefix.str(), "");
          argument arg;
          arg.set_name(arg_name);
          arg.initialize(arg_config);
          add_argument(arg);
        }
      }

      return;
    }

    void method::reset()
    {
      _priv_->arguments.clear();
      return;
    }

    void method::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
    {
      this->datatools::enriched_base::tree_dump(out_, title_, indent_, true);

      if (has_constness()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Constness : " << _constness_.get()
             << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Arguments : " << _priv_->arguments.size()
           << std::endl;
      {
        size_t counter = 0;
        const argument_set_by_name & name_index = _priv_->arguments.get<argument_tag_name>();
        for (argument_set_by_name::const_iterator iarg = name_index.begin();
             iarg != name_index.end();
             iarg++) {
          const argument & arg = *iarg;
          out_ << indent_ << i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << i_tree_dumpable::skip_tag;
          if (++counter == _priv_->arguments.size()) {
            out_ << i_tree_dumpable::last_tag;
            indent2 << i_tree_dumpable::last_skip_tag;
          } else {
            out_ << i_tree_dumpable::tag;
            indent2 << i_tree_dumpable::skip_tag;
          }
          out_ << "Argument #" << (counter - 1) << ": " << std::endl;
          arg.tree_dump(out_, "", indent2.str());
        }
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << is_valid() << std::endl;

      return;
    }

    void method::make_scalar_setter(const std::string & arg_name_,
                                    data_type           arg_type_,
                                    const std::string & arg_info_,
                                    const std::string & arg_info2_,
                                    const std::string & method_desc_,
                                    const std::string & method_name_)
    {
      reset();
      if (method_name_.empty()) {
        set_name("set");
      } else {
        set_name(method_name_);
      }
      if (!method_desc_.empty()) {
        set_terse_description(method_desc_);
      }
      set_constness(false);
      argument arg;
      arg.set_name(arg_name_);
      {
        std::ostringstream desc_oss;
        desc_oss << "Set the value of " << arg_name_ << std::ends;
        arg.set_description(desc_oss.str());
      }
      arg.set_rank(0);
      arg.set_access(ACCESS_INPUT);
      arg.grab_data_description().make_scalar(arg_type_, arg_info_, arg_info2_);
      arg.initialize();
      add_argument(arg);
      this->initialize();
      return;
    }

    void method::make_scalar_getter(const std::string & arg_name_,
                                    data_type arg_type_,
                                    const std::string & arg_info_,
                                    const std::string & arg_info2_,
                                    const std::string & method_desc_,
                                    const std::string & method_name_)
    {
      reset();
      if (method_name_.empty()) {
        set_name("get");
      } else {
        set_name(method_name_);
      }
      if (!method_desc_.empty()) {
        set_terse_description(method_desc_);
      }
      set_constness(true);
      argument arg;
      arg.set_name(arg_name_);
      {
        std::ostringstream desc_oss;
        desc_oss << "Get the value of " << arg_name_ << std::ends;
        arg.set_description(desc_oss.str());
      }
      arg.set_rank(0);
      arg.set_access(ACCESS_OUTPUT);
      arg.grab_data_description().make_scalar(arg_type_, arg_info_, arg_info2_);
      arg.initialize();
      add_argument(arg);
      this->initialize();
      return;
    }

  } // namespace introspection

} // namespace datatools
