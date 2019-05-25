// datatools/configuration/ascii_io.cc
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

// Ourselves
#include <datatools/configuration/ascii_io.h>

// Third party
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

// This project
#include <datatools/configuration/parameter_model.h>


namespace datatools {
  namespace configuration {
    ascii_io::ascii_io(uint32_t flags_)
    {
      _format_version_   = current_format_version_id();
      _no_header_        = false;
      _with_description_ = false;
      _with_title_       = false;
      _dont_ignore_unknown_registries_ = false;
      _logging_          = logger::PRIO_FATAL;
      _no_header_ = (flags_ & IO_NO_HEADER);
      _with_description_ = (flags_ & IO_DESCRIPTION);
      _with_title_ = (flags_ & IO_TITLE);
      if (flags_ & IO_TRACE) {
        set_logging(logger::PRIO_TRACE);
      }
      _dont_ignore_unknown_registries_ = (flags_ & IO_DONT_IGNORE_UNKNOWN_REGISTRY);
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
            for (const auto& i : vrec_.get_daughters()) {
              const variant_record & dvrec = i.second.get_record();
              store_record(out_, dvrec);
            }
          } else {
            out_ << unset_label() << std::endl;
          }
        }

        if (vrec_.is_variant()) {
          std::vector<std::string> ranked_params;
          vrec_.build_list_of_ranked_parameter_records(ranked_params);
          for (const std::string& param_key : ranked_params) {
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
            for (auto& i : vrec_.grab_daughters()) {
              variant_record & dvrec = i.second.grab_record();
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
          for (const std::string& param_key : ranked_params) {
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
      for (const std::string& param_name : ranked_params) {
        const variant_record & rec = vreg_.get_parameter_record(param_name);
        top_records.push_back(&rec);
      }
      for (const variant_record* vr : top_records) {
        DT_LOG_TRACE(_logging_, "Top record = '" << vr->get_path() << "'");
        store_record(out_, *vr);
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
      for (const std::string& param_name : ranked_params) {
        variant_record & rec = vreg_.grab_parameter_record(param_name);
        top_records.push_back(&rec);
      }
      for (variant_record* vr : top_records) {
        DT_LOG_TRACE(_logging_, "Top record = '" << vr->get_path() << "'");
        int error = load_record(in_, *vr);
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
      for (const std::string& reg_name : reg_keys) {
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
  } // namespace configuration
} // namespace datatools
