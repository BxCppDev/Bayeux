// datatools/configuration/io.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/configuration/io.h>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/kernel.h>

namespace datatools {

  namespace configuration {

    // static
    const std::string & ascii_io::unset_label()
    {
      static const std::string _label("__unset__");
      return _label;
    }

    ascii_io::ascii_io(uint32_t flags_)
    {
      _with_description_ = false;
      _with_title_ = false;
      _logging_ = logger::PRIO_FATAL;
      // std::cerr << "ascii_io::ascii_io: logging (1) = '" <<  logger::get_priority_label(_logging_) << "'" << std::endl;
      if (flags_ & IO_DESCRIPTION) {
        _with_description_ = true;
      }
      if (flags_ & IO_TITLE) {
        _with_title_ = true;
      }
      if (flags_ & IO_TRACE) {
        std::cerr << "ascii_io::ascii_io: " << "set_logging(PRIO_TRACE)" << std::endl;
        set_logging(logger::PRIO_TRACE);
      }
      // std::cerr << "ascii_io::ascii_io: logging (2) = '" <<  logger::get_priority_label(_logging_) << "'" << std::endl;
      // print(std::cerr, "ascii_io::ascii_io");
      return;
    }

    logger::priority ascii_io::get_logging() const
    {
      return _logging_;
    }

    void ascii_io::set_logging(logger::priority prio_)
    {
      _logging_ = prio_;
      return;
    }

    void ascii_io::store_record(std::ostream & out_, const variant_record & vrec_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
      if (vrec_.is_active()) {

        if (vrec_.is_parameter() && vrec_.get_parameter_model().is_variable()) {
          std::string value_str;
          command::returned_info cri = vrec_.value_to_string(value_str);
          out_ << vrec_.get_path();
          /*
            out_ << " : " << get_label_from_type(vrec_.get_parameter_model().get_type());
            if (vrec_.get_parameter_model().is_real() && vrec_.get_parameter_model().has_real_unit_label()) {
            out_ << " as " << vrec_.get_parameter_model().get_real_unit_label();
            }
          */
          out_ << " = ";
          if (cri.is_success()) {
            out_ << value_str << std::endl;
            for (variant_record::daughter_dict_type::const_iterator i = vrec_.get_daughters().begin();
                 i != vrec_.get_daughters().end();
                 i++) {
              const variant_record * rec = i->second;
              store_record(out_, *rec);
            }
          } else {
            out_ << unset_label() << std::endl;
          }
        }

        if (vrec_.is_variant()) {
          // out_ << "#@variant " << get_path() << "=" << "active" << std::endl;
          for (variant_record::daughter_dict_type::const_iterator i = vrec_.get_daughters().begin();
               i != vrec_.get_daughters().end();
               i++) {
            const variant_record * rec = i->second;
            store_record(out_, *rec);
          }
        }

      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return;
    }

    int ascii_io::load_record(std::istream & in_, variant_record & vrec_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
      // print(std::cerr);
      if (vrec_.is_active()) {

        if (vrec_.is_parameter() && vrec_.get_parameter_model().is_variable()) {
          std::string line;
          do {
            line.clear();
            if (!in_ || in_.eof()) {
              break;
            }
            std::getline(in_, line);
            boost::trim(line);
            DT_LOG_TRACE(_logging_, "line = '" << line << "'");
            if (line.empty()) {
              continue;
            } else if (line[0] == '#') {
              line.clear();
              continue;
            }
          } while (line.empty());
          DT_LOG_TRACE(_logging_, "About to process line = '" << line << "'");
          if (line.empty()) {
            return 1;
          } else {
            std::istringstream line_iss(line);
            std::string path;
            char equal = 0;
            line_iss >> path >> std::ws >> equal >> std::ws;
            DT_THROW_IF(equal != '=', std::logic_error, "Invalid parameter format!");
            std::string value_str;
            std::getline(line_iss, value_str);
            DT_LOG_TRACE(_logging_, "path = '" << path << "'");
            DT_LOG_TRACE(_logging_, "value_str = '" << value_str << "'");
            command::returned_info cri;
            if (value_str == unset_label()) {
              cri = vrec_.unset_value();
            } else {
              cri = vrec_.string_to_value(value_str);
              DT_THROW_IF (! cri.is_success(),
                           std::logic_error,
                           "Failed to convert variant parameter '" << vrec_.get_name()
                           << "' from '" << value_str << "' : "
                           << cri.get_error_message());
            }
            if (cri.is_success()) {
              for (variant_record::daughter_dict_type::iterator i = vrec_.grab_daughters().begin();
                   i != vrec_.grab_daughters().end();
                   i++) {
                variant_record * rec = i->second;
                int error = load_record(in_, *rec);
                if (error) {
                  DT_LOG_FATAL(_logging_, "Failed to load variant record '" << rec->get_name() << "'!");
                  return 1;
                }
              }
            }
          }
        }

        if (vrec_.is_variant()) {
          for (variant_record::daughter_dict_type::iterator i = vrec_.grab_daughters().begin();
               i != vrec_.grab_daughters().end();
               i++) {
            variant_record * rec = i->second;
            int error = load_record(in_, *rec);
            if (error) {
              DT_LOG_FATAL(_logging_, "Failed to load parameter record '" << rec->get_name() << "'!");
              return 1;
            }
          }
        }

      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return 0;
    }

    void ascii_io::store_registry(std::ostream & out_, const variant_registry & vreg_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
      std::vector<const variant_record *> top_records;
      for (variant_registry::record_dict_type::const_iterator i = vreg_.get_records().begin();
           i != vreg_.get_records().end();
           i++) {
        const variant_record & rec = i->second;
        if (!rec.has_parent()) {
          top_records.push_back(&rec);
        }
      }

      for (size_t i = 0; i < top_records.size(); i++) {
        DT_LOG_TRACE(_logging_, "Top record = '" << top_records[i]->get_path() << "'");
        store_record(out_, *top_records[i]);
      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return;
    }

    int ascii_io::load_registry(std::istream & in_, variant_registry & vreg_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
      std::vector<variant_record *> top_records;
      for (variant_registry::record_dict_type::iterator i = vreg_.grab_records().begin();
           i != vreg_.grab_records().end();
           i++) {
        variant_record & rec = i->second;
        if (!rec.has_parent()) {
          top_records.push_back(&rec);
        }
      }
      for (size_t i = 0; i < top_records.size(); i++) {
        DT_LOG_TRACE(_logging_, "Top record = '" << top_records[i]->get_path() << "'");
        int error = load_record(in_, *top_records[i]);
        if (error) {
          return 1;
        }
        /*
          char c = 0;
          in_.get(c);
          in_.putback(c);
          if (c == '[') {
          break;
          }
        */
      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return 0;
    }

    void ascii_io::store_repository(std::ostream & out_, const variant_repository & vrep_) const
    {
      for (variant_repository::registry_dict_type::const_iterator i = vrep_.get_registries().begin();
           i != vrep_.get_registries().end();
           i++) {
        const std::string & re_name = i->first;
        const variant_repository::registry_entry & re = i->second;
        out_ << "[" << re_name << "]" << std::endl;
        store_registry(out_, re.get_registry());
      }
      return;
    }

    int ascii_io::load_repository(std::istream & in_, variant_repository & vrep_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
      std::string line;
      std::string current_registry_name;
      variant_registry * current_registry_ptr = 0;
      while (in_) {
        int line_count = 0;
        std::getline(in_, line);
        line_count++;
        DT_LOG_TRACE(_logging_, "line = '" << line << "'");
        if (line.empty()) {
          continue;
        } else if (line[0] == '#') {
          continue;
        } else {
          std::string word;
          std::istringstream line_iss(line);
          line_iss >> word;
          if (word.size() == 0) {
            continue;
          }
          DT_LOG_TRACE(_logging_, "word = '" << word << "'");
          if (boost::starts_with(word, "[") && boost::ends_with(word, "]")) {
            current_registry_name = word.substr(1, word.length()-2);
            DT_LOG_TRACE(_logging_, "current_registry_name = '" << current_registry_name << "'");
            DT_THROW_IF (!vrep_.has_registry(current_registry_name), std::logic_error,
                         "Variant repository has no registry named '" << current_registry_name << "'!");
            current_registry_ptr = &vrep_.grab_registry(current_registry_name);
            int error = load_registry(in_, *current_registry_ptr);
            if (error) {
              return 1;
            }
          }
        }
        if (! in_) {
          break;
        }
      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return 0;
    }

    void ascii_io::print(std::ostream & out_, const std::string & title_) const
    {
      out_ << "ascii_io::print: " << title_ << std::endl;
      out_ << "|-- with_description = " << _with_description_ << std::endl;
      out_ << "|-- with_title = " << _with_title_ << std::endl;
      out_ << "`-- logging = '" << datatools::logger::get_priority_label(_logging_)
           << "'" << std::endl;
      return;
    }

    /* --------------------------------- */

    variant_preprocessor::variant_preprocessor(unsigned int flags_)
    {
      _trace_ = false;
      _remove_quotes_ = false;
      _repository_ = 0;
      if (flags_ & FLAG_TRACE) {
        set_trace(true);
      }
      if (flags_ & FLAG_REMOVE_QUOTES) {
        set_remove_quotes(true);
      }
      _set_default_kernel_repository();
      return;
    }

    bool variant_preprocessor::is_trace() const
    {
      return _trace_;
    }

    void variant_preprocessor::set_trace(bool trace_)
    {
      _trace_ = trace_;
      return;
    }

    bool variant_preprocessor::is_remove_quotes() const
    {
      return _remove_quotes_;
    }

    void variant_preprocessor::set_remove_quotes(bool rq_)
    {
      _remove_quotes_ = rq_;
      return;
    }

    bool variant_preprocessor::has_repository() const
    {
      return _repository_ != 0;
    }

    void variant_preprocessor::set_repository(const variant_repository & repository_)
    {
      _repository_ = &repository_;
      return;
    }

    const variant_repository & variant_preprocessor::get_repository() const
    {
      DT_THROW_IF(!has_repository(), std::logic_error, "No defined variant repository!");
      return *_repository_;
    }

    void variant_preprocessor::_set_default_kernel_repository()
    {
      if (datatools::kernel::is_instantiated()) {
        const datatools::kernel & dtkl = datatools::kernel::const_instance();
        if (dtkl.has_variant_repository()) {
          const variant_repository & dkvr = dtkl.get_variant_repository();
          set_repository(dkvr);
        }
      }
      return;
    }

    command::returned_info variant_preprocessor::resolve_variant(const std::string & variant_desc_,
                                                                 bool & variant_active_,
                                                                 bool & variant_reverse_) const
    {
      command::returned_info cri(command::CEC_SUCCESS);
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");

      // Initialize output:
      variant_active_ = false;
      variant_reverse_ = false;

      // Working area:
      std::string variant_desc = variant_desc_;
      bool variant_default = false;
      bool variant_found   = false;
      bool has_variant_default = false;
      bool variant_reverse = false;

      std::vector<std::string> variant_tokens;
      boost::split(variant_tokens, variant_desc, boost::is_any_of("|"));
      if (variant_tokens.size() == 0) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Invalid variant only directive '" << variant_desc_ << "'!" );
        return cri;
      }
      if (variant_tokens.size() > 0 ) {
        variant_desc = variant_tokens[0];
      }
      if (variant_tokens.size() > 1 ) {
        std::string variant_default_str = variant_tokens[1];
        std::istringstream boolean_in(variant_default_str);
        if (! io::read_boolean(boolean_in, variant_default)) {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                    "Invalid default boolean value for variant only directive '" << variant_desc_ << "'!");
          return cri;
        }
        has_variant_default = true;
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_desc = '" << variant_desc << "'");
      if (variant_desc[0] == '!') {
        variant_reverse = true;
        variant_desc = variant_desc.substr(1);
        // Should we trim 'variant_desc' ?
        if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_desc (reverse) = '" << variant_desc_ << "'");
      }
      std::vector<std::string> variant_tokens2;
      boost::split(variant_tokens2, variant_desc, boost::is_any_of(":"));
      if (variant_tokens2.size() != 2) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Cannot parse variant path tokens from '"
                                  << variant_desc_ << "' !");
        return cri;
      }
      std::string variant_registry_name = variant_tokens2[0];
      if (variant_registry_name.empty()) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Missing variant only path from '" << variant_desc_ << "' !");
        return cri;
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_registry_name = '" << variant_registry_name << "'");

      std::string variant_path = variant_tokens2[1];
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_path = '" << variant_path << "'");
      // Search for registrated variant parameter in the associated variant repository:
      bool variant_active = false;
      if (has_repository()) {
        const variant_repository & rep = get_repository();
        command::returned_info cri = rep.cmd_is_active_variant(variant_registry_name,
                                                               variant_path,
                                                               variant_active);
        if (cri.is_success()) {
          variant_found = true;
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Found variant only from '" << variant_desc_ << "'");
        } else {
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Cannot found variant only from '" << variant_desc_ << "'");
          variant_found = false;
          // DT_THROW_IF(true, std::logic_error,
          //          "Cannot resolve configuration variant '" << variant_desc
          //          << "' from the variant repository!");
        }
      } else {
        //DT_LOG_WARNING(logging,  "Bayeux/datatools kernel's variant repository is not used!");
        // DT_THROW_IF(true, std::logic_error,
        //             "Variant repository is not used!");
      }
      if (!variant_found) {
        if (!has_variant_default) {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_COMMAND_INVALID_CONTEXT,
                                    "No available default value for variant only directive '" << variant_desc_ << "' !");
          return cri;
        }
        variant_active = variant_default;
      }
      if (_trace_) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_registry_name = '" << variant_registry_name << "'");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_path          = '" << variant_path << "'");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_found         = '" << variant_found << "'");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "has_variant_default   = '" << has_variant_default << "'");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_default       = '" << variant_default << "'");
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_reverse       = '" << variant_reverse << "'");
      }
      if (cri.is_success()) {
        // Validate output:
        variant_active_  = variant_active;
        variant_reverse_ = variant_reverse;
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
      return cri;
    }

    command::returned_info variant_preprocessor::preprocess_parameter(const std::string & parameter_token_,
                                                                      std::string & parameter_effective_token_) const
    {
      static const std::string variant_default_sep = "|";
      command::returned_info cri(command::CEC_SUCCESS);
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering");
      parameter_effective_token_.clear();

      // Working string:
      std::string effective_property_value_str;

      std::vector<std::string> variant_tokens;
      boost::split(variant_tokens, parameter_token_, boost::is_any_of(variant_default_sep));
      if (variant_tokens.size() == 0) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Cannot parse variant parameter path from '" << parameter_token_ << "' !");
        return cri;
      }

      std::string variant_path = variant_tokens[0];
      bool        has_variant_def_value = false;
      std::string variant_def_value;
      if (variant_tokens.size() > 1) {
        variant_def_value = variant_tokens[1];
        has_variant_def_value = true;
      }
      std::vector<std::string> variant_path_tokens;
      boost::split(variant_path_tokens, variant_path, boost::is_any_of(":"));
      DT_THROW_IF (variant_path_tokens.size() != 2,
                   std::logic_error,
                   "Cannot parse variant command token '"
                   << parameter_token_ << "' !");
      std::string variant_registry_name = variant_path_tokens[0];
      std::string variant_parameter_path = variant_path_tokens[1];
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_registry_name  = '" << variant_registry_name << "'");
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "variant_parameter_path = '" << variant_parameter_path << "'");
      bool variant_parameter_found = false;
      std::string variant_parameter_value;
      // Search for registrated variant parameter in the associated variant repository:
      if (has_repository()) {
        const variant_repository & rep = get_repository();
        command::returned_info cri2 = rep.cmd_get_parameter(variant_registry_name,
                                                            variant_parameter_path, variant_parameter_value);
        if (cri2.is_success()) {
          effective_property_value_str = variant_parameter_value;
          variant_parameter_found = true;
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Found value for variant parameter '" << variant_path << "' = '"
                                    << variant_parameter_value << "'");
        } else {
          // DT_LOG_WARNING(logging, "Bayeux/datatools kernel's variant repository could not provide the value of "
          //                << "parameter '" << variant_path << "' : " << cri.get_error_message());
        }
      }
      if (! variant_parameter_found) {
        if (!has_variant_def_value) {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARAMETER_INVALID_CONTEXT,
                                    "No available default value for variant parameter '" << variant_path << "' !");
        } else {
          effective_property_value_str = variant_def_value;
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Use default value for variant parameter '" << variant_path << "' = '"
                                    << effective_property_value_str << "'");
        }
      }
      if (cri.is_success()) {
        parameter_effective_token_ = effective_property_value_str;
        if (is_remove_quotes()) {
          if (datatools::is_quoted(parameter_effective_token_, '"')) {
            datatools::remove_quotes(parameter_effective_token_, '"');
          } else if (datatools::is_quoted(parameter_effective_token_, '\'')) {
            datatools::remove_quotes(parameter_effective_token_, '\'');
          }
        }
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
      return cri;
    }


    std::string
    variant_preprocessor::preprocess_string(const std::string & source_) const
    {
      std::string token;
      command::returned_info cri = preprocess(source_, token);
      DT_THROW_IF(cri.is_failure(),
                  std::logic_error,
                  "String variant preprocessing failed: "
                  << cri.get_error_message());
      datatools::remove_quotes(token, '"');
      return token;
    }

    bool
    variant_preprocessor::preprocess_boolean(const std::string & source_) const
    {
      bool value;
      std::string bstr;
      command::returned_info cri = preprocess(source_, bstr);
      DT_THROW_IF(cri.is_failure(),
                  std::logic_error,
                  "String variant preprocessing failed: "
                  << cri.get_error_message());
      std::istringstream iss(bstr);
      DT_THROW_IF(!io::read_boolean(iss, value), std::logic_error,
                  "Cannot preprocess boolean variant parameter!");
      return value;
    }

    int
    variant_preprocessor::preprocess_integer(const std::string & source_) const
    {
      int value;
      std::string istr;
      command::returned_info cri = preprocess(source_, istr);
      DT_THROW_IF(cri.is_failure(),
                  std::logic_error,
                  "String variant preprocessing failed: "
                  << cri.get_error_message());
      std::istringstream iss(istr);
      DT_THROW_IF(!io::read_integer(iss, value), std::logic_error,
                  "Cannot preprocess integer variant parameter!");
      return value;
    }

    double
    variant_preprocessor::preprocess_real(const std::string & source_) const
    {
      double value;
      std::string rstr;
      command::returned_info cri = preprocess(source_, rstr);
      DT_THROW_IF(cri.is_failure(),
                  std::logic_error,
                  "String variant preprocessing failed: "
                  << cri.get_error_message());
      std::istringstream iss(rstr);
      bool normal;
      DT_THROW_IF(!io::read_real_number(iss, value, normal), std::logic_error,
                  "Cannot preprocess integer variant parameter!");
      return value;
    }

    void variant_preprocessor::preprocess_args_options(const std::vector<std::string> & args_,
                                                       std::vector<std::string> & preprocessed_args_)
    {
      for (int i = 0; i < (int) args_.size(); i++) {
        const std::string & arg = args_[i];
        std::string target;
        command::returned_info cri = preprocess(arg, target);
        DT_THROW_IF(cri.is_failure(),
                    std::logic_error,
                    "Variant preprocessing of '" << arg << "' failed: "
                    << cri.get_error_message());
        if (datatools::is_quoted(target, '"')) {
          datatools::remove_quotes(target, '"');
        }
        preprocessed_args_.push_back(target);
        if (is_trace()) {
          DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                       "target = '" << target << "'");
        }
      }
      return;
    }

    void variant_preprocessor::preprocess_args_options(int argc_, char ** argv_,
                                                       std::vector<std::string> & preprocessed_args_)
    {
      std::string app_name;
      std::vector<std::string> args;
      datatools::io::convert_command_line_args(argc_, argv_, app_name, args);
      preprocess_args_options(args, preprocessed_args_);
      return;
    }

    command::returned_info
    variant_preprocessor::preprocess(const std::string & source_, std::string & target_) const
    {
      command::returned_info cri(command::CEC_SUCCESS);
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE,  "Entering...");
      target_.clear();
      std::size_t pos = 0;
      std::string line = source_;
      std::ostringstream target_out;
      static const std::string variant_open_tag = "@variant(";
      static const std::string variant_close_tag = ")";

      while (pos != std::string::npos) {
        if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "line = '" << line << "'");
        std::size_t found = line.find(variant_open_tag, pos);
        if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "open tag found=" << found);
        if (found == std::string::npos) {
          target_out << line;
          break;
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "pos=" << pos);
        } else {
          target_out << line.substr(pos, found);
          line = line.substr(found);
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "line = '" << line << "'");
          std::size_t found2 = line.find(variant_close_tag, 0);
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "close tag found2 = " << found2);
          if (found2 == std::string::npos) {
            DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE, "Cannot find variant close tag in '" << line << "' !");
            break;
          }
          std::string parameter_token = line.substr(variant_open_tag.length(), found2 - variant_open_tag.length());
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "parameter_token = '" << parameter_token << "'");
          std::string parameter_effective_token;
          cri = preprocess_parameter(parameter_token, parameter_effective_token);
          if (cri.is_failure()) {
            break;
          }
          if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "parameter_effective_token = '" << parameter_effective_token << "'")
          target_out << parameter_effective_token;
          line = line.substr(found2 + 1);
        }
        pos = 0;
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "while end");
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "pos=" << pos);
      if (cri.is_success()) {
        target_ = target_out.str();
      }
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "target_ = '" << target_ << "'");
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "cri = '" << cri  << "'");
      if (_trace_) DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
      return cri;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
