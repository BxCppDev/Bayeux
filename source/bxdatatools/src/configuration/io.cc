// datatools/configuration/io.cc
/*
 * Copyright (C) 2014-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard library:
#include <memory>
#include <deque>

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
#include <datatools/configuration/ui/variant_repository_cli.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/kernel.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace configuration {

    // static
    const std::string & ascii_io::unset_label()
    {
      static const std::string _label("__unset__");
      return _label;
    }

    // static
    const std::string & ascii_io::error_label()
    {
      static const std::string _label("__error__");
      return _label;
    }

    // static
    const std::string & ascii_io::format_label()
    {
      static const std::string _label("datatools::configuration::variant");
      return _label;
    }

    // static
    version_id ascii_io::current_format_version_id()
    {
      static std::unique_ptr<version_id> _vid;
      if (_vid.get() == nullptr) {
        _vid.reset(new version_id(1,0));
      }
      return *_vid;
    }

    ascii_io::ascii_io(uint32_t flags_)
    {
      _format_version_   = current_format_version_id();
      _no_header_        = false;
      _with_description_ = false;
      _with_title_       = false;
      _dont_ignore_unknown_registries_ = false;
      _logging_          = logger::PRIO_FATAL;
      if (flags_ & IO_NO_HEADER) {
        _no_header_ = true;
      }
      if (flags_ & IO_DESCRIPTION) {
        _with_description_ = true;
      }
      if (flags_ & IO_TITLE) {
        _with_title_ = true;
      }
      if (flags_ & IO_TRACE) {
        set_logging(logger::PRIO_TRACE);
      }
      if (flags_ & IO_DONT_IGNORE_UNKNOWN_REGISTRY) {
        _dont_ignore_unknown_registries_ = true;
      }
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
          out_ << " = ";
          if (cri.is_success()) {
            out_ << value_str << std::endl;
            for (variant_record::daughter_dict_type::const_iterator i = vrec_.get_daughters().begin();
                 i != vrec_.get_daughters().end();
                 i++) {
              const variant_record & dvrec = i->second.get_record();
              store_record(out_, dvrec);
            }
          } else {
            out_ << unset_label() << std::endl;
          }
        }

        if (vrec_.is_variant()) {
          // out_ << "#@variant " << get_path() << "=" << "active" << std::endl;
          std::vector<std::string> ranked_params;
          vrec_.build_list_of_ranked_parameter_records(ranked_params);
          for (std::size_t ipar = 0; ipar < ranked_params.size(); ipar++) {
            const std::string & param_key = ranked_params[ipar];
            const variant_record & dvrec = vrec_.get_daughter(param_key);
            store_record(out_, dvrec);
          }
        }

      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return;
    }

    int ascii_io::load_record(std::istream & in_, variant_record & vrec_) const
    {
      DT_LOG_TRACE(_logging_, "Entering...");
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
          }

          {
            std::istringstream iss(line);
            std::string check;
            iss >> check;
            if (check[0] == '[') {
              DT_LOG_FATAL(_logging_, "Detected a start block tag '[' in line '" << line << "'!");
              return 1;
            }
          }
          size_t equal_pos = line.find('=');
          if (equal_pos == std::string::npos) {
            DT_LOG_FATAL(_logging_, "Cannot find '=' separator in line '" << line << "'!");
            return 1;
          }
          std::string path_str = line.substr(0, equal_pos);
          std::istringstream path_iss(path_str);
          std::string path;
          path_iss >> path;
          std::string value_str = line.substr(equal_pos + 1);
          boost::trim(path);
          DT_LOG_TRACE(_logging_, "path = '" << path << "'");
          DT_THROW_IF(path != vrec_.get_path(),
                      std::logic_error,
                      "Variant parameter path '" << path << "' is different from the expected one '" << vrec_.get_path()
                      << "'!");
          boost::trim(value_str);
          DT_LOG_TRACE(_logging_, "value_str = '" << value_str << "'");
          command::returned_info cri;
          if (value_str == unset_label()) {
            cri = vrec_.unset_value();
          } else {
            cri = vrec_.string_to_value(value_str);
            DT_THROW_IF(! cri.is_success(),
                        std::logic_error,
                        "Failed to convert variant parameter '" << vrec_.get_path()
                        << "' from '" << value_str << "' : "
                        << cri.get_error_message());
          }
          if (cri.is_success()) {
            for (variant_record::daughter_dict_type::iterator i = vrec_.grab_daughters().begin();
                 i != vrec_.grab_daughters().end();
                 i++) {
              variant_record & dvrec = i->second.grab_record();
              int error = load_record(in_, dvrec);
              if (error) {
                DT_LOG_FATAL(_logging_, "Failed to load variant record '" << dvrec.get_path() << "'!");
                return 1;
              }
            }
          }
        }

        if (vrec_.is_variant()) {
          std::vector<std::string> ranked_params;
          vrec_.build_list_of_ranked_parameter_records(ranked_params);
          for (std::size_t ipar = 0; ipar < ranked_params.size(); ipar++) {
            const std::string & param_key = ranked_params[ipar];
            variant_record & dvrec = vrec_.grab_daughter(param_key);
            int error = load_record(in_, dvrec);
            if (error) {
              DT_LOG_FATAL(_logging_, "Failed to load parameter record '" << dvrec.get_path() << "'!");
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
      std::vector<std::string> ranked_params;
      vreg_.list_of_ranked_parameters(ranked_params);
      std::vector<const variant_record *> top_records;
      for (std::size_t i = 0; i < ranked_params.size(); i++) {
        const std::string & param_name = ranked_params[i];
        const variant_record & rec = vreg_.get_parameter_record(param_name);
        top_records.push_back(&rec);
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
      std::vector<std::string> ranked_params;
      vreg_.list_of_ranked_parameters(ranked_params);
      std::vector<variant_record *> top_records;
      for (std::size_t i = 0; i < ranked_params.size(); i++) {
        const std::string & param_name = ranked_params[i];
        variant_record & rec = vreg_.grab_parameter_record(param_name);
        top_records.push_back(&rec);
      }
      for (size_t i = 0; i < top_records.size(); i++) {
        DT_LOG_TRACE(_logging_, "Top record = '" << top_records[i]->get_path() << "'");
        int error = load_record(in_, *top_records[i]);
        if (error) {
          return 1;
        }
      }
      DT_LOG_TRACE(_logging_, "Exiting.");
      return 0;
    }

    void ascii_io::store_repository(std::ostream & out_, const variant_repository & vrep_) const
    {
      if (!_no_header_) {
        out_ << "#@format=" << format_label() << std::endl;
        out_ << "#@format.version=" << _format_version_.to_string() << std::endl;
        if (vrep_.has_organization()) {
          out_ << "#@organization=" << vrep_.get_organization() << std::endl;
        }
        if (vrep_.has_application()) {
          out_ << "#@application=" << vrep_.get_application() << std::endl;
        }
        out_ << std::endl;
      }
      std::vector<std::string> reg_keys;
      vrep_.build_ordered_registry_keys(reg_keys);
      for (unsigned int ireg = 0; ireg < reg_keys.size(); ireg++) {
        const std::string & reg_name = reg_keys[ireg];
        if (!vrep_.is_active_registry(reg_name)) {
          continue;
        }
        out_ << "[registry=\"" << reg_name << "\"]" << std::endl;
        // Legacy format for the registry header:
        // out_ << "[" << reg_name << "]" << std::endl;
        store_registry(out_, vrep_.get_registry(reg_name));
        out_ << std::endl;
      }
      return;
    }

    int ascii_io::load_repository(std::istream & in_, variant_repository & vrep_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::string line;
      std::string current_registry_name;
      datatools::version_id in_format_version;
      bool started_sections = false;
      bool skip_section = false;
      variant_registry * current_registry_ptr = nullptr;
      std::vector<std::string> registries;
      vrep_.build_ordered_registry_keys(registries);
      // Not correct: the list of remaining registries should be dynamic in case
      // of registries depending on parameters in other registries
      // std::deque<std::string> remaining_registries;
      // for (auto r : registries) {
      //   remaining_registries.push_back(r);
      // }
      std::deque<std::string> processed_registries;
      std::string last_processed_registry;
      while (in_) {
        int line_count = 0;
        std::getline(in_, line);
        line_count++;
        DT_LOG_TRACE(_logging_, "line = '" << line << "'");
        if (line.empty()) {
          // Skip empty line:
          continue;
        } else if (line[0] == '#') {
          // Process comment line:
          if (line.length() >= 2 && line[1] == '@') {
            DT_THROW_IF(started_sections, std::logic_error,
                        "Metacomment '" << line << "' is not allowed within a registry section!");
            unsigned long equal_pos = line.find('=');
            if (equal_pos != line.npos) {
              std::string tag = boost::trim_right_copy(line.substr(2, equal_pos - 2));
              DT_LOG_TRACE(_logging_,"tag = '" << tag << "'");
              std::string tag_value = line.substr(equal_pos + 1);
              boost::trim(tag_value);
              DT_LOG_TRACE(_logging_,"tag_value = '" << tag_value << "'");
              if (tag == "format") {
                if (!tag_value.empty()) {
                  std::string format_lbl = tag_value;
                  DT_LOG_TRACE(_logging_,"format = '" << format_lbl << "'");
                  if (!_no_header_) {
                    DT_THROW_IF(format_lbl != format_label(), std::logic_error,
                                "Invalid format label '" << format_lbl << "'!");
                  }
                }
              }
              if (tag == "format.version") {
                if (!tag_value.empty()) {
                  std::string in_format_version_repr = tag_value;
                  DT_LOG_TRACE(_logging_,"format_version_repr = '" << in_format_version_repr << "'");
                  datatools::version_id in_vid;
                  bool ok = in_vid.from_string(in_format_version_repr);
                  DT_THROW_IF(!ok, std::logic_error,
                              "Invalid format version identifier '" << in_format_version_repr << "'!");
                  if (!_no_header_) {
                    in_format_version = in_vid;
                    // We dont' check it !
                  }
                }
              }
              if (tag == "organization") {
                if (!tag_value.empty()) {
                  if (!_no_header_) {
                    vrep_.set_organization(tag_value);
                  }
                }
              }
              if (tag == "application") {
                if (!tag_value.empty()) {
                  if (!_no_header_) {
                    vrep_.set_application(tag_value);
                  }
                }
              }
            }
          }
          continue;
        } else {
          boost::trim(line);
          std::string word;
          std::istringstream line_iss(line);
          line_iss >> word;
          if (word.size() == 0 || word[0] == '#') {
            continue;
          }
          DT_LOG_TRACE(_logging_, "word = '" << word << "'");
          if (boost::starts_with(line, "[") && boost::ends_with(line, "]")) {
            if (skip_section) {
              skip_section = false;
            }
            // Detect a registry section:
            std::string reg_name_repr = line.substr(1, word.length()-2);
            unsigned int equal_pos = reg_name_repr.find('=');
            if (equal_pos > 0 && equal_pos < reg_name_repr.length()) {
              std::string reg_name_tag = reg_name_repr.substr(0, equal_pos);
              boost::trim(reg_name_tag);
              DT_LOG_TRACE(_logging_, "reg_name_tag = '" << reg_name_tag << "'");
              DT_THROW_IF(reg_name_tag != "registry", std::logic_error,
                          "Invalid registry tag '" << reg_name_tag << "' in line '" << line << "'!");
              current_registry_name = reg_name_repr.substr(equal_pos+1);
              boost::trim(current_registry_name);
              datatools::remove_quotes(current_registry_name);
              DT_LOG_TRACE(_logging_, "parsed current_registry_name = '" << current_registry_name << "'");
            } else {
              // Legacy format:
              current_registry_name = word.substr(1, word.length()-2);
              DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                             "Deprecated variant registry format '" << line << "'! Please use '[registry=\""
                             << current_registry_name << "\"]' syntax !");
            }
            DT_LOG_TRACE(_logging_, "current_registry_name = '" << current_registry_name << "'");
            if (!vrep_.has_registry(current_registry_name)) {
              if (_dont_ignore_unknown_registries_) {
                DT_THROW(std::logic_error,
                          "Variant repository has no known registry named '" << current_registry_name << "'!");
              }
              DT_LOG_WARNING(_logging_,
                             "Ignoring profile section '" << current_registry_name
                             << "' (unknown registry) while loading variant repository...");
              skip_section = true;
            } else {
              // if (current_registry_name != remaining_registries.front()) {
              //   DT_THROW(std::logic_error,
              //            "Variant repository does not expect registry with name '" << current_registry_name << "' at this rank (expected: '" << remaining_registries.front() << "')!");
              // }
              skip_section = false;
            }
            started_sections = true;
            if (!skip_section) {
              current_registry_ptr = &vrep_.grab_registry(current_registry_name);
              int error = load_registry(in_, *current_registry_ptr);
              last_processed_registry = current_registry_name;
              processed_registries.push_back(last_processed_registry);
              // remaining_registries.pop_front();
              current_registry_ptr = nullptr;
              DT_LOG_DEBUG(_logging_, "Registry '" << processed_registries.back() << "' has been processed.");
              // DT_LOG_DEBUG(_logging_, "Next registry to be processed '" << remaining_registries.front() << "'.");
              if (error) {
                DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                             "Error while loading the '" << current_registry_name << "' variant registry!");
                return 1;
              }
            }
          } else {
            // Not a start section line:
            if (started_sections && skip_section) {
              continue;
            }
            if (current_registry_ptr == nullptr) {
              DT_THROW(std::logic_error, "Unexpected line '" << line << "'!");
            }
          }
        }
        if (! in_) {
          break;
        }
      }
      // if (remaining_registries.size()) {
      //   DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
      //                "There are remaining registries to be processed while loading variant repository (next expected: '" << remaining_registries.front() << "')!");
      //   return 1;
      // }
      DT_LOG_TRACE_EXITING(_logging_);
      return 0;
    }

    void ascii_io::print(std::ostream & out_, const std::string & title_) const
    {
      out_ << "ascii_io::print: " << title_ << std::endl;
      out_ << "|-- no_header = " << _no_header_ << std::endl;
      out_ << "|-- with_description = " << _with_description_ << std::endl;
      out_ << "|-- with_title = " << _with_title_ << std::endl;
      out_ << "|-- format_version = " << _format_version_.to_string() << std::endl;
      out_ << "`-- logging = '" << datatools::logger::get_priority_label(_logging_)
           << "'" << std::endl;
      return;
    }

    /* --------------------------------- */

    void variant_reporting::dump(std::ostream& out_) const
    {
      out_ << "variant_reporting::dump: \n";
      out_ << "|-- " << "Repository : " << _repository_ << "\n";
      out_ << "`-- " << "Counters   : " << _parameter_stats_.size() << "\n";
      return;
    }

    logger::priority variant_reporting::get_logging() const
    {
      return _logging_;
    }

    void variant_reporting::set_logging(logger::priority prio_)
    {
      _logging_ = prio_;
      return;
    }

    variant_reporting::variant_reporting()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _repository_ = nullptr;
      return;
    }

    variant_reporting::~variant_reporting()
    {
      reset_repository();
      return;
    }

    bool variant_reporting::has_repository() const
    {
      return _repository_ != nullptr;
    }

    void variant_reporting::set_repository(const variant_repository & repository_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(has_repository(), std::logic_error, "Repository is already set!");
      _repository_ = &repository_;
      _init_repository_();
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_reporting::reset_repository()
    {
      _parameter_stats_.clear();
      _repository_ = nullptr;
      return;
    }

    void variant_reporting::_init_repository_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::vector<std::string> regkeys;
      _repository_->build_ordered_registry_keys(regkeys);
      for (std::size_t ireg = 0; ireg < regkeys.size(); ireg++) {
        const std::string & regname = regkeys[ireg];
        if (!_repository_->is_active_registry(regname)) continue;
        const variant_registry & reg = _repository_->get_registry(regname);
        std::vector<std::string> setpars;
        reg.list_of_parameters(setpars, variant_registry::LIST_ACTIVE_ONLY);
        for (std::size_t ipar = 0; ipar < setpars.size(); ipar++) {
          const std::string & parname = setpars[ipar];
          std::string fullparpath = regname + ":" + parname;
          _parameter_stats_[fullparpath] = 0;
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    const variant_repository & variant_reporting::get_repository() const
    {
      DT_THROW_IF(!has_repository(), std::logic_error, "No variant repository is defined!");
      return *_repository_;
    }

    void variant_reporting::reset()
    {
      _parameter_stats_.clear();
      return;
    }

    void variant_reporting::print_report(std::ostream & out_, uint32_t /* flags_ */) const
    {
      std::vector<std::string> unused_params;
      out_ << "# datatools::configuration::variant_reporting::print_report:\n";
      out_ << "[counters]\n";
      out_ << "# List of usage counters associated to set variant parameters:\n";
      for (auto stat : _parameter_stats_) {
        out_ << stat.first << " = " << stat.second;
        out_ << std::endl;
        if (stat.second == 0) unused_params.push_back(stat.first);
      }
      out_ << "\n";
      if (unused_params.size()) {
        out_ << "[unused]\n";
        out_ << "# The following variant parameters were set but not used:\n";
        for (auto fullparpath : unused_params) {
          out_ << fullparpath << std::endl;
        }
        out_ << "\n";
      }
      return;
    }

    void variant_reporting::add(const std::string & path_, std::size_t increment_)
    {
      statistics_type::iterator found = _parameter_stats_.find(path_);
      if (found == _parameter_stats_.end()) {
        _parameter_stats_[path_] = increment_;
      } else {
        found->second += increment_;
      }
      return;
    }

    /* --------------------------------- */

    variant_preprocessor::variant_preprocessor(unsigned int flags_)
    {
      _logging_ = logger::PRIO_FATAL;
      _remove_quotes_ = false;
      _repository_ = nullptr;
      if (flags_ & FLAG_TRACE) {
        set_logging(logger::PRIO_TRACE);
      }
      if (flags_ & FLAG_REMOVE_QUOTES) {
        set_remove_quotes(true);
      }
      _set_default_kernel_repository();
      return;
    }

    logger::priority variant_preprocessor::get_logging() const
    {
      return _logging_;
    }

    void variant_preprocessor::set_logging(logger::priority prio_)
    {
      _logging_ = prio_;
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
      return _repository_ != nullptr;
    }

    void variant_preprocessor::set_repository(const variant_repository & repository_)
    {
      _repository_ = &repository_;
      return;
    }

    void variant_preprocessor::reset_repository()
    {
      _repository_ = nullptr;
    }

    const variant_repository & variant_preprocessor::get_repository() const
    {
      DT_THROW_IF(!has_repository(), std::logic_error, "No variant repository is defined!");
      return *_repository_;
    }

    bool variant_preprocessor::repository_is_active() const
    {
      return has_repository() && _repository_->get_number_of_registries() > 0;
    }

    void variant_preprocessor::_set_default_kernel_repository()
    {
      if (datatools::kernel::is_instantiated()) {
        const datatools::kernel & dtkl = datatools::kernel::const_instance();
        if (dtkl.has_effective_variant_repository()) {
          const variant_repository & dkvr = dtkl.get_effective_variant_repository();
          set_repository(dkvr);
        }
      }
      return;
    }

    command::returned_info variant_preprocessor::resolve_variant(const std::string & variant_desc_,
                                                                 bool & variant_active_,
                                                                 bool & variant_reverse_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      command::returned_info cri(command::CEC_SUCCESS);

      // Initialize output:
      variant_active_ = false;
      variant_reverse_ = false;

      // Working area:
      std::string variant_desc = variant_desc_;
      bool has_variant_default = false;
      bool variant_default = false;
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
      DT_LOG_TRACE(_logging_, "variant_desc = '" << variant_desc << "'");
      if (variant_desc[0] == '!') {
        variant_reverse = true;
        variant_desc = variant_desc.substr(1);
        // Should we trim 'variant_desc' ?
        DT_LOG_TRACE(_logging_, "variant_desc (reverse) = '" << variant_desc << "'");
      }
      std::vector<std::string> variant_tokens2;
      boost::split(variant_tokens2, variant_desc, boost::is_any_of(":"));
      if (variant_tokens2.size() != 2) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Cannot parse variant path tokens from '"
                                  << variant_desc << "' !");
        return cri;
      }
      std::string variant_registry_name = variant_tokens2[0];
      if (variant_registry_name.empty()) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                  "Missing variant only path from '" << variant_desc << "' !");
        return cri;
      }
      DT_LOG_TRACE(_logging_, "variant_registry_name = '" << variant_registry_name << "'");

      std::string variant_path = variant_tokens2[1];
      DT_LOG_TRACE(_logging_, "variant_path = '" << variant_path << "'");
      // Search for registrated variant parameter in the associated variant repository:
      bool variant_active = false;
      // NOTE: 2017-06-08 FM: Should we use : if (!has_repository()) ...
      if (!repository_is_active()) {
        DT_LOG_TRACE(_logging_, "repository is not active");
        // If variant repository is not activated:
        if (has_variant_default) {
          DT_LOG_TRACE(_logging_, "using variant default value");
          // We fallback to the default value if any. Syntax is:
          //   {variant-description}|true
          //   {variant-description}|false
          variant_active_ = variant_default;
          variant_reverse_ = false;
          return cri;
        } else {
          DT_LOG_TRACE(_logging_, "no variant default value");
          // It is an error: no variant repository nor fallback default value is available:
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_CONTEXT_INVALID,
                                    "Inactive variant repository while processing '" << variant_desc << "' !");
          return cri;
        }
      } else {
        DT_LOG_TRACE(_logging_, "repository is active");
      }
      const variant_repository & rep = get_repository();
      if (!rep.has_registry(variant_registry_name)) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                  "Unknown registry '" << variant_registry_name << "' while processing '" << variant_desc << "' !");
        return cri;
      }
      const variant_registry & reg = rep. get_registry(variant_registry_name);
      if (!reg.variant_exists(variant_path)) {
        DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARAMETER_INVALID_KEY,
                                  "Unknown variant record '" << variant_path << " in registry '" << variant_registry_name << "' while processing '" << variant_desc << "' !");
        return cri;
      }
      variant_active = reg.is_active_variant(variant_path);
      DT_LOG_TRACE(_logging_, "variant_registry_name = '" << variant_registry_name << "'");
      DT_LOG_TRACE(_logging_, "variant_path          = '" << variant_path << "'");
      DT_LOG_TRACE(_logging_, "variant_active        = " << variant_active);
      DT_LOG_TRACE(_logging_, "variant_reverse       = " << variant_reverse);
      // Validate output:
      variant_active_  = variant_active;
      variant_reverse_ = variant_reverse;
      DT_LOG_TRACE_EXITING(_logging_);
      return cri;
    }

    command::returned_info variant_preprocessor::preprocess_parameter(const std::string & parameter_token_,
                                                                      std::string & parameter_effective_token_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      static const std::string variant_default_sep = "|";
      // Working string:
      std::string effective_property_value_str;
      command::returned_info cri(command::CEC_SUCCESS);
      try {
        parameter_effective_token_.clear();
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
        if (variant_path_tokens.size() != 2) {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE,
                                    "Cannot parse variant command token '" << parameter_token_ << "'!");
          DT_THROW(std::logic_error,
                   "Cannot parse variant command token '" << parameter_token_ << "'!");
        }
        std::string variant_registry_name  = variant_path_tokens[0];
        std::string variant_parameter_path = variant_path_tokens[1];
        DT_LOG_TRACE(_logging_, "variant_registry_name  = '" << variant_registry_name << "'");
        DT_LOG_TRACE(_logging_, "variant_parameter_path = '" << variant_parameter_path << "'");
        std::string variant_parameter_value;
        if (!repository_is_active()) {
          if (has_variant_def_value) {
            parameter_effective_token_ = variant_def_value;
            return cri;
          } else {
            DT_COMMAND_RETURNED_ERROR(cri, command::CEC_CONTEXT_INVALID,
                                      "Inactive variant repository while processing '" << parameter_token_ << "'!");
            return cri;
          }
        }
        DT_LOG_TRACE(_logging_, "repository is active");
        // Search for registrated variant parameter in the associated variant repository:
        const variant_repository & rep = get_repository();
        ui::variant_repository_cli vrepCli(const_cast<variant_repository &>(rep));
        vrepCli.set_logging(_logging_);
        DT_LOG_TRACE(_logging_, "Invoking cmd_get_parameter...");
        command::returned_info cri2 = vrepCli.cmd_get_parameter(variant_registry_name,
                                                                variant_parameter_path,
                                                                variant_parameter_value);
        if (cri2.is_failure()) {
          cri = cri2;
          return cri;
        }
        DT_LOG_TRACE(_logging_, "Success.");
        effective_property_value_str = variant_parameter_value;
        DT_LOG_TRACE(_logging_, "Found value for variant parameter '" << variant_path << "' = '"
                     << variant_parameter_value << "'");

        // if (! variant_parameter_found && cri.is_success()) {
        //   // If no error before, we try to fallback to a default value:
        //   if (!has_variant_def_value) {
        //     DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARAMETER_UNSET_VALUE,
        //                               "No available default value for variant parameter '" << variant_path << "' !");
        //   } else {
        //     effective_property_value_str = variant_def_value;
        //     DT_LOG_TRACE(_logging_, "Use default value for variant parameter '" << variant_path << "' = '"
        //                               << effective_property_value_str << "'");
        //   }
        // }
        parameter_effective_token_ = effective_property_value_str;
        // Reporting here...
        DT_LOG_TRACE(_logging_, "Reporting for variant_path = '" << variant_path << "'");
        if (_repository_->has_reporting()) {
          DT_LOG_TRACE(_logging_, "Repository '" << _repository_->get_name() << " has reporting!");
          variant_repository & mutable_repository = const_cast<variant_repository&>(get_repository());
          variant_reporting & rptg = mutable_repository.grab_reporting();
          rptg.add(variant_path);
        } else {
          DT_LOG_TRACE(_logging_, "Repository '" << _repository_->get_name() << " has no reporting!");
        }
        if (is_remove_quotes()) {
          if (datatools::is_quoted(parameter_effective_token_, '"')) {
            datatools::remove_quotes(parameter_effective_token_, '"');
          } else if (datatools::is_quoted(parameter_effective_token_, '\'')) {
            datatools::remove_quotes(parameter_effective_token_, '\'');
          }
        }
      } catch (std::exception & error) {
        cri.set_error_message(error.what());
      }
      DT_LOG_TRACE_EXITING(_logging_);
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
        DT_LOG_TRACE(_logging_, "target = '" << target << "'");
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
      datatools::logger::priority logging = _logging_;
      // logging = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE_ENTERING(logging);
      command::returned_info cri(command::CEC_SUCCESS);
      target_.clear();
      std::size_t pos = 0;
      std::string line = source_;
      std::ostringstream target_out;
      static const std::string variant_open_tag = "@variant(";
      static const std::string variant_close_tag = ")";
      while (pos != std::string::npos) {
        DT_LOG_TRACE(logging, "line = '" << line << "'");
        std::size_t found = line.find(variant_open_tag, pos);
        DT_LOG_TRACE(logging, "open tag found=" << found);
        if (found == std::string::npos) {
          target_out << line;
          break;
          DT_LOG_TRACE(logging, "pos=" << pos);
        } else {
          target_out << line.substr(pos, found);
          line = line.substr(found);
          DT_LOG_TRACE(logging, "line = '" << line << "'");
          std::size_t found2 = line.find(variant_close_tag, 0);
          DT_LOG_TRACE(logging, "close tag found2 = " << found2);
          if (found2 == std::string::npos) {
            DT_COMMAND_RETURNED_ERROR(cri, command::CEC_PARSING_FAILURE, "Cannot find variant close tag in '" << line << "' !");
            break;
          }
          std::string parameter_token = line.substr(variant_open_tag.length(), found2 - variant_open_tag.length());
          DT_LOG_TRACE(logging, "parameter_token = '" << parameter_token << "'");
          std::string parameter_effective_token;
          cri = preprocess_parameter(parameter_token, parameter_effective_token);
          if (cri.is_failure()) {
            break;
          }
          DT_LOG_TRACE(logging, "parameter_effective_token = '" << parameter_effective_token << "'")
                         target_out << parameter_effective_token;
          line = line.substr(found2 + variant_close_tag.length());
        }
        pos = 0;
      }
      DT_LOG_TRACE(logging, "while end");
      DT_LOG_TRACE(logging, "pos=" << pos);
      if (cri.is_success()) {
        target_ = target_out.str();
      }
      DT_LOG_TRACE(logging, "target_ = '" << target_ << "'");
      DT_LOG_TRACE(logging, "cri = '" << cri  << "'");
      DT_LOG_TRACE_EXITING(logging);
      return cri;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
