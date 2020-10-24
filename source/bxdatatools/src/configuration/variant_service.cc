// datatools/configuration/variant_service.cc
//
// Copyright (c) 2016 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Université de Caen
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

// - Ourselves:
#include <datatools/configuration/variant_service.h>

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/configuration/io.h>
#include <datatools/kernel.h>
#include <datatools/configuration/ui/variant_repository_cli.h>

#if DATATOOLS_WITH_QT_GUI == 1
// Specific GUI support:
// - Qt:
#include <QStyleFactory>
#include <QApplication>
// - Bayeux/datatools:
#include <datatools/qt/interface.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

namespace datatools {

  namespace configuration {

    variant_service::config::config()
    {
      profile_load_dont_ignore_unknown = false;
#if DATATOOLS_WITH_QT_GUI == 1
      gui = false;
#endif // DATATOOLS_WITH_QT_GUI == 1
      tui = false;
      return;
    }

    bool variant_service::config::is_active() const
    {
      if (!config_filename.empty()) return true;
      if (registry_rules.size()) return true;
      return false;
    }

    void variant_service::config::print(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }
      static const std::string item_tag = "|-- ";
      static const std::string item_ltag = "`-- ";

      out_ << indent_ << item_tag << "Label : ";
      if (!label.empty()) {
        out_ << "'" << label << "'";
      } else {
        out_ << "<none>";
      }
      out_ << "\n";

      out_ << indent_ << item_tag << "Logging : '" << logging << "'\n";

      out_ << indent_ << item_tag << "Configuration file : ";
      if (!config_filename.empty()) {
        out_ << "'" << config_filename << "'";
      } else {
        out_ << "<none>";
      }
      out_ << "\n";

      out_ << indent_ << item_tag << "Additional registries : ";
      if (registry_rules.size()) {
        out_ << "[" << registry_rules.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      // out_ << indent_ << item_tag << "Dependencies for additional registries : ";
      // if (registry_dependencies.size()) {
      //   out_ << "[" << registry_dependencies.size() << "]";
      // } else {
      //   out_ << "<none>";
      // }
      // out_ << std::endl;

      out_ << indent_ << item_tag << "Load profile : ";
      if (!profile_load.empty()) {
        out_ << "'" << profile_load << "'";
        if (profile_load_dont_ignore_unknown) {
          out_ << " (ignore unknown)";
        }
      } else {
        out_ << "<none>";
      }
      out_ << "\n";

      out_ << indent_ << item_tag << "Settings : ";
      if (settings.size()) {
        out_ << "[" << settings.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

#if DATATOOLS_WITH_QT_GUI == 1
      out_ << indent_ << item_tag << "GUI : " << (gui ? "yes" : "no") << "\n";
#endif // DATATOOLS_WITH_QT_GUI == 1

      out_ << indent_ << item_tag << "TUI : " << (tui ? "yes" : "no") << "\n";

      out_ << indent_ << item_tag << "Store profile : ";
      if (!profile_store.empty()) {
        out_ << "'" << profile_store << "'";
      } else {
        out_ << "<none>";
      }
      out_ << "\n";

      out_ << indent_ << item_tag << "Reporting file : ";
      if (!reporting_filename.empty()) {
        out_ << "'" << reporting_filename << "'";
      } else {
        out_ << "<none>";
      }
      out_ << "\n";

      out_ << indent_ << item_ltag << "Active : " << (is_active() ? "yes" : "no") << "\n";

      return;
    }

    // static
    void variant_service::init_options(boost::program_options::options_description & opts_,
                                        config & cfg_,
                                        uint32_t flags_)
    {
      namespace bpo = boost::program_options;
      bpo::options_description_easy_init easy_init = opts_.add_options();

      // Default behaviour: collecting all variant related options
      bool parse_label           = true;
      bool parse_logging         = true;
      bool parse_config_filename = true;
      bool parse_registry_rules  = true;
      bool parse_registry_dependencies = true;
      bool parse_profile_load    = true;
      bool parse_profile_load_dont_ignore_unknown = true;
      bool parse_settings        = true;
      bool parse_profile_store   = true;
      bool parse_reporting       = true;

      // Inhibition of the parsinf for specific options:
      if (flags_ & NO_LABEL) parse_label = false;
      if (flags_ & NO_LOGGING) parse_logging = false;
      if (flags_ & NO_CONFIG_FILENAME) parse_config_filename = false;
      if (flags_ & NO_REGISTRY_RULES) parse_registry_rules = false;
      if (flags_ & NO_REGISTRY_DEPENDENCIES) parse_registry_dependencies = false;
      if (flags_ & NO_PROFILE_LOAD) parse_profile_load = false;
      if (flags_ & PROFILE_LOAD_DONT_IGNORE_UNKNOWN) parse_profile_load_dont_ignore_unknown = false;
      if (flags_ & NO_SETTINGS) parse_settings = false;
      if (flags_ & NO_PROFILE_STORE) parse_profile_store = false;
      if (flags_ & NO_REPORTING) parse_reporting = false;
#if DATATOOLS_WITH_QT_GUI == 1
      bool parse_gui = true;
      if (flags_ & NO_GUI) parse_gui = false;
#endif // DATATOOLS_WITH_QT_GUI == 1
      bool parse_tui = true;
      if (flags_ & NO_TUI) parse_tui = false;

      if (parse_label) {
        easy_init("variant-label",
                  bpo::value<std::string>(&cfg_.label)->value_name("[name]"),
                  "label of the variant service");
      }

      if (parse_logging) {
        easy_init("variant-logging",
                  bpo::value<std::string>(&cfg_.logging)->value_name("[level]"),
                  "logging level of the variant service");
      }

      if (parse_config_filename) {
        easy_init("variant-config",
                  bpo::value<std::string>(&cfg_.config_filename)->value_name("[file]"),
                  "file from which to load the main configuration of the variant repository");
      }

      if (parse_registry_rules) {
        easy_init("variant-registry",
                  bpo::value<std::vector<std::string> >(&cfg_.registry_rules)->value_name("[rule]"),
                  "rules for additional variant registries");
      }

      if (parse_registry_dependencies) {
        easy_init("variant-registry-dependencies",
                  bpo::value<std::vector<std::string> >(&cfg_.registry_dependencies)->value_name("[rule]"),
                  "dependencies for additional variant registries");
      }

      if (parse_profile_load) {
        easy_init("variant-load",
                  bpo::value<std::string>(&cfg_.profile_load)->value_name("[file]"),
                  "file from which to load a specific variant profile at startup");
      }

      if (parse_profile_load_dont_ignore_unknown) {
        easy_init("variant-load-no-unknown",
                  bpo::value<bool>(&cfg_.profile_load_dont_ignore_unknown)->zero_tokens()->default_value(false),
                  "do not ignore unknown registry sections at variant profile loading");
      }

      if (parse_settings) {
        easy_init("variant-set",
                  bpo::value<std::vector<std::string> >(&cfg_.settings)->value_name("[setting]"),
                  "setting rules for variant parameters applied at startup");
      }

#if DATATOOLS_WITH_QT_GUI == 1
      if (parse_gui) {
        easy_init("variant-gui",
                  bpo::value<bool>(&cfg_.gui)->zero_tokens()->default_value(false),
                  "launch the variant GUI editor at startup");
      }
#endif // DATATOOLS_WITH_QT_GUI == 1

      if (parse_tui) {
        easy_init("variant-tui",
                  bpo::value<bool>(&cfg_.tui)->zero_tokens()->default_value(false),
                  "launch the variant TUI editor at startup (not implemented yet)");
      }

      if (parse_profile_store) {
        easy_init("variant-store",
                  bpo::value<std::string>(&cfg_.profile_store)->value_name("[file]"),
                  "file in which to store the effective current variant profile");
      }

      if (parse_reporting) {
        easy_init("variant-reporting",
                  bpo::value<std::string>(&cfg_.reporting_filename)->value_name("[file]"),
                  "file in which to print the variant usage report");
      }

      return;
    }

    variant_service::variant_service(uint32_t /* flags_ */)
    {
      _tui_ = false;
#if DATATOOLS_WITH_QT_GUI == 1
      _gui_ = false;
#endif // DATATOOLS_WITH_QT_GUI == 1
      _logging_ = datatools::logger::PRIO_FATAL;
      _started_ = false;
      _dont_ignore_unknown_at_load_ = false;
      return;
    }

    variant_service::~variant_service()
    {
      if (is_started()) {
        stop();
      }
      return;
    }

    bool variant_service::is_started() const
    {
      return _started_;
    }

    void variant_service::set_label(const std::string & label_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _label_ = label_;
      return;
    }

    const std::string & variant_service::get_label() const
    {
      return _label_;
    }

    void variant_service::set_logging(datatools::logger::priority prio_)
    {
      _logging_ = prio_;
      return;
    }

    void variant_service::set_repository_config(const std::string & config_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _repository_config_ = config_;
      return;
    }

    void variant_service::add_registry_rule(const std::string & registry_rule_)
    {
      _registry_rules_.push_back(registry_rule_);
      return;
    }

    void variant_service::add_registry_dependency(const std::string & registry_dependency_rule_)
    {
      _registry_deps_.push_back(registry_dependency_rule_);
      return;
    }

    bool variant_service::has_registry_record(const std::string & registry_name_) const
    {
      registry_record_dict_type::const_iterator found = _registries_.find(registry_name_);
      return found != _registries_.end();
    }

    variant_service::registry_record &
    variant_service::_add_registry_record(const registry_record & registry_record_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      DT_THROW_IF(!registry_record_.is_valid(), std::logic_error,
                  "Attempt to add an invalid registry!");
      DT_THROW_IF(has_registry_record(registry_record_._name_),
                  std::logic_error,
                  "A registry with name '" << registry_record_._name_ << "' is already defined!");
      _registries_[registry_record_._name_] = registry_record_;
      return _registries_.find(registry_record_._name_)->second;
    }

    void variant_service::set_input_profile(const std::string & ip_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _input_profile_ = ip_;
      return;
    }

    void variant_service::set_dont_ignore_unknown_at_load(bool flag_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _dont_ignore_unknown_at_load_ = flag_;
      return;
    }

    void variant_service::set_output_profile(const std::string & op_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _output_profile_ = op_;
      return;
    }

    bool variant_service::has_reporting_file() const
    {
      return !_reporting_file_.empty();
    }

    void variant_service::set_reporting_file(const std::string & urf_)
    {
      _reporting_file_ = urf_;
      return;
    }

    void variant_service::set_settings(const std::vector<std::string> & settings_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _settings_ = settings_;
      return;
    }

#if DATATOOLS_WITH_QT_GUI == 1
    void variant_service::set_gui(bool gui_)
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started!");
      _gui_ = gui_;
      return;
    }
#endif // DATATOOLS_WITH_QT_GUI == 1

    variant_service::registry_record::registry_record()
    {
      _rank_ = -1;
      return;
    }

    bool variant_service::registry_record::is_valid() const
    {
      if (_name_.empty()) return false;
      if (_rank_ < -1) return false;
      if (_config_file_.empty()) return false;
      return true;
    }

    int variant_service::registry_record::init_registry(int & max_rank_,
                                                         const std::string & registry_rule_)
    {
      std::string variant_registry_rule = registry_rule_;
      std::string variant_name;
      std::string variant_display_name;
      std::string variant_mgr_config_file;
      int         variant_rank = -1;
      size_t apos = variant_registry_rule.find('=');
      DT_THROW_IF(apos == std::string::npos, std::logic_error,
                  "Invalid variant registry loading rule format '" << variant_registry_rule << "'!");
      {
        // Extract name and optional rank:
        // format:
        //  "name=path/to/variant/registry/config/file"
        //  "name[+]=path/to/variant/registry/config/file"
        //  "name[7]=path/to/variant/registry/config/file"
        std::string variant_name_traits_rule = variant_registry_rule.substr(0, apos);
        boost::trim(variant_name_traits_rule);
        size_t opos = variant_name_traits_rule.find('[');
        if (opos != std::string::npos) {
          DT_THROW_IF(variant_name_traits_rule.back() != ']', std::logic_error,
                      "Missing traits token closing ']' in variant registry rule '" << variant_registry_rule << "'!");
          variant_name_traits_rule.pop_back();
          variant_name = variant_name_traits_rule.substr(0, opos);
          std::string traits_str = variant_name_traits_rule.substr(opos + 1);
          boost::trim(traits_str);
          std::vector<std::string> traits_rules;
          boost::split(traits_rules, traits_str, boost::is_any_of(","));
          DT_THROW_IF(traits_rules.size() == 0, std::logic_error,
                      "No trait definition in variant registry rule '" << variant_registry_rule << "'!");
          for (size_t itrait = 0; itrait < traits_rules.size(); itrait++) {
            std::string trait_def = traits_rules[itrait];
            boost::trim(trait_def);
            // Only rank is parsed for now:
            std::string rank_str = trait_def;
            int max_rank = max_rank_;
            if (rank_str == "+") {
              // Compute the next rank above the max rank:
              variant_rank = ++max_rank;
            } else {
              try {
                variant_rank = boost::lexical_cast<int>(rank_str);
                DT_THROW_IF(variant_rank <= max_rank,
                            std::logic_error,
                            "Cannot use rank [" << variant_rank << "] for registry '" << variant_name
                            << "' which is lower than or equal to the maximum used rank [" << max_rank << "]!");
                max_rank = variant_rank;
              } catch (std::exception & error) {
                DT_THROW(std::logic_error, "Cannot parse registry rank from '" << rank_str << "': " << error.what());
              }
            }
            // Update max rank:
            max_rank_ = max_rank;
          }
        } else {
          variant_name = variant_name_traits_rule;
        }
        boost::trim(variant_name);
      }
      variant_mgr_config_file = variant_registry_rule.substr(apos + 1);
      datatools::fetch_path_with_env(variant_mgr_config_file);
      // DT_LOG_TRACE(_logging_, "Register variant registry '" << variant_name << "' at rank [" << variant_rank << "]...");

      // Set attributes:
      _name_         = variant_name;
      _display_name_ = variant_display_name;
      _description_  = "";
      _rank_         = variant_rank;
      _config_file_  = variant_mgr_config_file;

      return 0;
    }

    int variant_service::registry_record::add_dependency(const std::string & variant_dep_rule_)
    {
      std::string registry_name;
      std::string variant_dependency;
      size_t apos = variant_dep_rule_.find('=');
      DT_THROW_IF(apos == std::string::npos, std::logic_error,
                  "Invalid variant dependency rule '" << variant_dep_rule_ << "'!");
      {
        registry_name = variant_dep_rule_.substr(0, apos);
        boost::trim(registry_name);
        if (_name_ != registry_name) {
          return 1;
        }
        variant_dependency = variant_dep_rule_.substr(apos + 1);
        boost::trim(variant_dependency);
        DT_THROW_IF(variant_dependency.empty(),
                    std::logic_error,
                    "Missing variant dependency for registry '" << registry_name << "'!");
        _dependencies_.push_back(variant_dependency);
      }
      return 0;
    }

    const std::string & variant_service::registry_record::get_name() const
    {
      return _name_;
    }

    const std::string & variant_service::registry_record::get_display_name() const
    {
      return _display_name_;
    }

    const std::string & variant_service::registry_record::get_description() const
    {
      return _description_;
    }

    int variant_service::registry_record::get_rank() const
    {
      return _rank_;
    }

    const std::string & variant_service::registry_record::get_config_file() const
    {
      return _config_file_;
    }

    const std::vector<std::string> &
    variant_service::registry_record::get_dependencies() const
    {
      return _dependencies_;
    }

    void variant_service::configure(const config & cfg_, bool lock_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(is_started(), std::logic_error, "variant service is already started!");
      datatools::logger::priority prio = datatools::logger::get_priority(cfg_.logging);
      if (prio != datatools::logger::PRIO_UNDEFINED) {
        set_logging(prio);
      }
      set_repository_config(cfg_.config_filename);
      for (size_t i = 0; i < cfg_.registry_rules.size(); i++) {
        const std::string & reg_rule = cfg_.registry_rules[i];
        add_registry_rule(reg_rule);
      }
      for (size_t i = 0; i < cfg_.registry_dependencies.size(); i++) {
        const std::string & reg_dep = cfg_.registry_dependencies[i];
        add_registry_dependency(reg_dep);
      }
      set_input_profile(cfg_.profile_load);
      set_settings(cfg_.settings);
      set_output_profile(cfg_.profile_store);
#if DATATOOLS_WITH_QT_GUI == 1
      set_gui(cfg_.gui);
#endif // DATATOOLS_WITH_QT_GUI == 1
      if (!cfg_.reporting_filename.empty()) {
        set_reporting_file(cfg_.reporting_filename);
      }
      _do_build_();
      if (lock_) {
        if (!_repository_.is_locked()) {
          _repository_.lock();
        }
      } else {
        if (_repository_.is_locked()) {
          _repository_.unlock();
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_build_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      // Initialize the repository:
      _do_variant_config_();

      // Load a profile:
      _do_variant_load_profile_();

      // Apply specific parameter settings:
      _do_variant_apply_settings_();

#if DATATOOLS_WITH_QT_GUI == 1
      if (_gui_) {
        // Launch the GUI editor:
        _do_variant_launch_gui_();
      }
#endif // DATATOOLS_WITH_QT_GUI == 1

      if (_tui_) {
        // Launch the TUI editor:
        _do_variant_launch_tui_();
      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    variant_repository & variant_service::grab_repository()
    {
      DT_THROW_IF(is_started(), std::logic_error, "Variant service is started and locked!");
      return _repository_;
    }

    const variant_repository & variant_service::get_repository() const
    {
      return _repository_;
    }

    bool variant_service::is_reporting() const
    {
      return ! _reporting_file_.empty();
    }

    void variant_service::start()
    {
      DT_LOG_TRACE_ENTERING(_logging_); //, "Instantation of the variant_service singleton.")
      if (is_started()) {
        return;
      }

      // Store the selected profile:
      _do_variant_store_profile_();

      // Export the application variant repository to the datatools' kernel
      // in order to make the variant service accessible from
      // each software component in the current application:
      _do_variant_system_export_();

      if (is_reporting()) {
        DT_LOG_TRACE(_logging_, "variant service is_reporting!!!");
        _do_variant_reporting_init_();
      }

      _started_ = true;
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_reporting_init_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      _reporting_.set_logging(_logging_);
      _reporting_.set_repository(_repository_);
      _repository_.set_reporting(_reporting_);
      if (datatools::logger::is_trace(_logging_)) {
        _reporting_.dump(std::cerr);
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_reporting_fini_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::ofstream reporting_out;
      std::string rep_out_path = _reporting_file_;
      datatools::fetch_path_with_env(rep_out_path);
      reporting_out.open(rep_out_path.c_str());
      DT_THROW_IF(!reporting_out,
                  std::runtime_error,
                  "Cannot open variant usage reporting file '" << rep_out_path << "'!");
      DT_LOG_NOTICE(_logging_, "Saving variant usage report in file '" << rep_out_path << "'...");
      _reporting_.print_report(reporting_out);
      _repository_.reset_reporting();
      _reporting_.reset_repository();
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::stop()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (!is_started()) {
        return;
      }
      // Clean the datatools' kernel from the application variant repository:
      _started_ = false;
      if (is_reporting()) {
        DT_LOG_TRACE(_logging_, "Variant service is reporting.");
        _do_variant_reporting_fini_();
      }
       _do_variant_system_discard_();
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    int32_t variant_service::get_max_rank() const
    {
      std::vector<std::string> ranked_keys;
      _repository_.build_ordered_registry_keys(ranked_keys);
      int32_t max_rank = -1;
      for (size_t ikey = 0; ikey < ranked_keys.size(); ikey++) {
        const std::string & key = ranked_keys[ikey];
        int rk = _repository_.get_rank(key);
        if (rk > max_rank) max_rank = rk;
      }
      return max_rank;
    }

    // int32_t variant_service::get_free_rank() const
    // {
    //   int free_rank = 0;
    //   // Not implemented
    //   return free_rank;
    // }

    int32_t variant_service::get_next_rank() const
    {
      return get_max_rank() + 1;
    }

    void variant_service::_do_variant_additional_registries_setup_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      bool was_locked = _repository_.is_locked();
      if (was_locked) {
        _repository_.unlock();
      }

      // Build additional registry records:
      /*
       *  registry rule : "name=path/to/registry/config/file"
       */
      int max_rank = get_max_rank();
      for (size_t i = 0; i < _registry_rules_.size(); i++) {
        const std::string & reg_rule = _registry_rules_[i];
        registry_record reg_rec;
        reg_rec.init_registry(max_rank, reg_rule);
        {
          // registry_record & new_reg_rec = _add_registry_record(reg_rec);
          /*
          for (size_t j = 0; j < _registry_deps_.size(); j++) {
            const std::string & reg_dep_rule = _registry_deps_[j];
            int ret = new_reg_rec.add_dependency(reg_dep_rule);
            if (ret == 0) {
              DT_LOG_TRACE(_logging_, "Add dependency to registry '" << new_reg_rec.get_name() << "'!");
            }
          }
          */
        }
      }

      // Traverse the dictionary of additional registry records and load the repository:
      for (registry_record_dict_type::const_iterator ireg = _registries_.begin();
           ireg != _registries_.end();
           ireg++) {
        const registry_record & reg_rec = ireg->second;
        _repository_.registration_embedded(reg_rec.get_config_file(),
                                           "",
                                           reg_rec.get_name(),
                                           reg_rec.get_display_name(),
                                           "",
                                           reg_rec.get_rank());
        // // Add registry dependency:
        // for (std::size_t idep = 0;
        //      idep < reg_rec.get_dependencies().size();
        //      idep++) {
        //   const std::string & reg_depend = reg_rec.get_dependencies()[idep];
        //   // 2016-11-02, FM : NOT SUPPORTED ANYMORE
        //   _repository_.add_registry_dependency(reg_rec.get_name(), reg_depend);
        // }
      }

      if (was_locked) {
        _repository_.lock();
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_config_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      datatools::kernel & krnl = datatools::kernel::instance();
      if (datatools::logger::is_trace(_logging_)) {
        krnl.set_logging(_logging_);
      }
      std::string config_filename = _repository_config_;
      datatools::fetch_path_with_env(config_filename);
      datatools::properties rep_config;
      datatools::read_config(config_filename, rep_config);
      DT_LOG_DEBUG(_logging_, "Initializing application variant repository from '" << config_filename << "'...");
      _repository_.set_logging_priority(_logging_);
      _repository_.initialize(rep_config);

      // Additional registries:
      if (_registry_rules_.size()) {
        _do_variant_additional_registries_setup_();
      }
      _repository_.update();
      DT_LOG_DEBUG(_logging_, "Application variant repository locked = " << _repository_.is_locked());
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_load_profile_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::string profile_filename = _input_profile_;
      boost::trim(profile_filename);
      if (profile_filename.empty()) return;
      datatools::fetch_path_with_env(profile_filename);
      DT_LOG_DEBUG(_logging_, "Loading profile for application variant repository from '" << profile_filename << "'...");
      std::ifstream rep_file;
      rep_file.open(profile_filename.c_str());
      if (!rep_file) {
        throw variant_exception("Cannot open input variant profile '"+_input_profile_+"'");
      }
      uint32_t rep_io_flags = 0;
      rep_io_flags |= datatools::configuration::ascii_io::IO_DEFAULT;
      if (_dont_ignore_unknown_at_load_) {
        rep_io_flags |= datatools::configuration::ascii_io::IO_DONT_IGNORE_UNKNOWN_REGISTRY;
      }
      if (datatools::logger::is_trace(_logging_)) {
        rep_io_flags |= datatools::configuration::ascii_io::IO_TRACE;
      }
      datatools::configuration::ascii_io rep_io(rep_io_flags);
      bool was_locked = _repository_.is_locked();
      if (was_locked) {
        _repository_.unlock();
      }
      int error = rep_io.load_repository(rep_file, _repository_);
      if (was_locked) {
        _repository_.lock();
      }
      if (error) {
        throw variant_exception("Loading of variant profile '"+_input_profile_+"' failed");
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_store_profile_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::string profile_filename = _output_profile_;
      boost::trim(profile_filename);
      if(profile_filename.empty()) return;
      datatools::fetch_path_with_env(profile_filename);
      DT_LOG_TRACE(_logging_, "Storing profile for application variant repository in '" << profile_filename << "'...");
      std::ofstream rep_file;
      rep_file.open(profile_filename.c_str());
      if (!rep_file) {
        throw variant_exception("Cannot open output variant profile '"+_output_profile_+"'");
      }
      uint32_t rep_io_flags = 0;
      rep_io_flags |= datatools::configuration::ascii_io::IO_DEFAULT;
      datatools::configuration::ascii_io rep_io(rep_io_flags);
      rep_io.store_repository(rep_file, _repository_);
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::apply_setting(const std::string & setting_)
    {
      DT_LOG_TRACE(_logging_, "Applying variant setting from rule '" << setting_ << "'...");
      datatools::configuration::variant_parameter_set_type vps;
      datatools::command::returned_info cri = vps.parse(setting_);
      if (cri.is_failure()) {
        throw variant_exception("Cannot parse variant parameter setting directive: '" + setting_ + "'");
      }
      bool was_locked = _repository_.is_locked();
      if (was_locked) {
        _repository_.unlock();
      }
      datatools::configuration::ui::variant_repository_cli vrepCli(_repository_);
      cri = vrepCli.cmd_set_parameter(vps.registry_key,
                                      vps.param_key,
                                      vps.param_value_str);
      if (was_locked) {
        _repository_.lock();
      }
      if (cri.is_failure()) {
        throw variant_exception("Cannot set variant parameter : " + cri.get_error_message());
      }
      return;
    }

    void variant_service::_do_variant_apply_settings_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::vector<std::string> settings = _settings_;
      // Re-order variant parameters:
      datatools::configuration::variant_parameter_set_comparator comp(_repository_);
      std::sort(settings.begin(), settings.end(), comp);
      // bool was_locked = _repository_.is_locked();
      // if (was_locked) {
      //   _repository_.unlock();
      // }
      // Apply variant parameters:
      for (size_t iset = 0; iset < settings.size(); iset++) {
        const std::string & variant_set = settings[iset];
        DT_LOG_TRACE(_logging_, "Applying variant setting from rule '" << variant_set << "'...");
        apply_setting(variant_set);
        // datatools::configuration::variant_parameter_set_type vps;
        // // Format is : "foo:param0/var0/key0=value"
        // vps.parse(variant_set);
        // datatools::command::returned_info cri =
        //   _repository_.cmd_set_parameter(vps.registry_key,
        //                                  vps.param_key,
        //                                  vps.param_value_str);
        // if (cri.is_failure()) {
        //   throw variant_exception("Cannot set variant parameter : " + cri.get_error_message());
        // }
      }
      // if (was_locked) {
      //   _repository_.lock();
      // }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_launch_tui_()
    {
      DT_LOG_TRACE_EXITING(_logging_);
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Variant TUI editor is not implemented yet!");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

#if DATATOOLS_WITH_QT_GUI == 1
    void variant_service::_do_variant_launch_gui_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      // Launch a Qt based dialog for the variant repository:
      const datatools::kernel & krnl = datatools::kernel::const_instance();
      /*datatools::qt::interface & iqt = */
      datatools::qt::interface::instance(krnl.get_argc(),
                                         krnl.get_argv(),
                                         "Bayeux Variant Service");
      datatools::configuration::ui::variant_repository_dialog vrep_dialog(_repository_);
      vrep_dialog.exec();
      // int ret = vrep_dialog.exec();
      // if (ret) {
      //   // throw variant_exception("Variant GUI editor failed");
      // }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }
#endif // DATATOOLS_WITH_QT_GUI == 1

    void variant_service::_do_variant_system_export_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      datatools::kernel & krnl = datatools::kernel::instance();
      if (krnl.is_initialized()) {
        DT_THROW_IF(krnl.has_variant_service(), std::logic_error,
                    "Kernel already has a variant service!");
        krnl.set_variant_service(*this);
      } else {
        throw variant_exception("Datatools' kernel system variant repository is not instantiated.");
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_service::_do_variant_system_discard_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      datatools::kernel & krnl = datatools::kernel::instance();
      if (krnl.is_initialized()) {
        if (krnl.has_variant_service() && this == &krnl.get_variant_service()) {
          krnl.reset_variant_service();
        }
      } else {
        // The datatools' kernel has shutdown. No need to unregister the application variant repository.
        DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is NOT instantiated.");
      }
      // Here there is potentially an issue with the order of termination for
      // the static system variant repository in the datatools' kernel.
      // So we first check if it is still present before to attempt to discard
      // user defined variant registries from it.
      // const datatools::kernel & krnl = datatools::kernel::const_instance();
      // if (krnl.is_initialized() && krnl.has_variant_repository()) {
      //   DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is instantiated.");
      //   _repository_.system_discard();
      // } else {
      //   // The datatools' kernel has shutdown... no need to unregister the application variant repository.
      //   DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is NOT instantiated.");
      // }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace configuration

} // namespace datatools
