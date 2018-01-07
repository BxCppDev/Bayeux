// datatools/ui/base_command.cc - A generic base command
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/ui/base_command.h>

// This project:
#include <datatools/ui/base_command_interface.h>
#include <datatools/detail/command_macros.h>
// #include <datatools/ui/traits.h>
#include <datatools/utils.h>

namespace datatools {

  namespace ui {

    // virtual
    bool base_command::is_name_valid(const std::string & candidate_name_) const
    {
      return ::datatools::name_validation(candidate_name_, ::datatools::NV_INSTANCE);
    }

    base_command::base_command()
    {
      _initialized_ = false;
      _parent_interface_ = nullptr;
      return;
    }

    base_command::base_command(const std::string & name_,
                               const std::string & description_,
                               const version_id & vid_)
    {
      _initialized_ = false;
      _parent_interface_ = nullptr;
      set_name(name_);
      set_terse_description(description_);
      if (vid_.is_valid()) {
        _set_version(vid_);
      }
      return;
    }

    base_command::~base_command()
    {
      return;
    }

    bool base_command::has_parent_interface() const
    {
      return _parent_interface_ != 0;
    }

    base_command_interface & base_command::grab_parent_interface()
    {
      DT_THROW_IF(!has_parent_interface(), std::logic_error,
                  "Command '" << get_name() << "' has no parent interface!");
      return *_parent_interface_;
    }

    const base_command_interface & base_command::get_parent_interface() const
    {
      DT_THROW_IF(!has_parent_interface(), std::logic_error,
                  "Command '" << get_name() << "' has no parent interface!");
      return *_parent_interface_;
    }

    void base_command::set_parent_interface(base_command_interface & pi_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Command '" << get_name() << "' is already initialized!");
      _parent_interface_ = &pi_;
      return;
    }

    bool base_command::has_version() const
    {
      return _version_ != boost::none;
    }

    void base_command::set_version(const datatools::version_id & version_)
    {
      _set_version(version_);
      return;
    }

    void base_command::_set_version(const datatools::version_id & version_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Command '" << get_name() << "' is initialized!");
      DT_THROW_IF(! version_.is_valid(), std::logic_error,
                  "Version ID '" << get_name() << "' is not valid!");
      _version_ = version_;
      return;
    }

    const datatools::version_id & base_command::get_version() const
    {
      DT_THROW_IF(!has_version(), std::logic_error,
                  "Command '" << get_name() << "' has no version information!");
      return _version_.get();
    }

    bool base_command::is_initialized() const
    {
      return _initialized_;
    }

    bool base_command::is_valid() const
    {
      return _is_valid();
    }

    bool base_command::is_disabled() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error,
                  "Command '" << get_name() << "' is not valid!");
      return this->_is_disabled();
    }

    // virtual
    bool base_command::_is_valid() const
    {
      return has_name() && is_initialized();
    }

    // virtual
    bool base_command::_is_disabled() const
    {
      return false;
    }

    void base_command::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void base_command::initialize(const properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Command '" << get_name() << "' is already initialized!");
      _init(config_);
      _initialized_ = true;
      return;
    }

    void base_command::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Command '" << get_name() << "' is not initialized!");
      _initialized_ = false;
      _fini();
      return;
    }

    bool base_command::use_generic_options() const
    {
      return !_no_generic_options_;
    }

    bool base_command::use_generic_help() const
    {
      return !_no_generic_help_;
    }

    bool base_command::has_generic_opts() const
    {
      return _generic_opts_.get() != nullptr && _generic_opts_->options().size();
    }

    const base_command::opts_type & base_command::get_generic_opts() const
    {
      DT_THROW_IF(!has_generic_opts(), std::logic_error, "No generic option is described!");
      return *_generic_opts_;
    }

    bool base_command::has_opts() const
    {
      return _opts_.get() != nullptr && _opts_->options().size();
    }

    const base_command::opts_type & base_command::get_opts() const
    {
      DT_THROW_IF(!has_opts(), std::logic_error, "No option is described!");
      return *_opts_;
    }

    bool base_command::has_args() const
    {
      return _args_.get() != nullptr;
    }

    bool base_command::has_short_ui_options() const
    {
      return !_no_short_ui_options_;
    }

    void base_command::print_version(std::ostream & out_,
                                     uint32_t /* flags_ */) const
    {
      DT_THROW_IF(is_disabled(), std::logic_error, "Command is disabled!");
      if (has_version()) {
        out_ << get_name() << " " << get_version() << std::endl;
      }
      return;
    }

    void base_command::print_usage(std::ostream & out_,
                                   uint32_t /* flags_ */) const
    {
      DT_THROW_IF(is_disabled(), std::logic_error, "Command is disabled!");
      out_ << "NAME : " << std::endl;
      out_ << "\t" << get_name();
      if (has_terse_description()) {
        out_ << " - " << get_terse_description();
      }
      out_ << std::endl;
      out_ << std::endl;
      out_ << "SYNOPSIS : " << std::endl
           << "\t" << get_name();
      if (has_generic_opts() || has_opts()) {
        out_ << " [OPTIONS]...";
        out_ << std::endl;
      }
      out_ << std::endl;

      if (has_generic_opts() || has_opts()) {
        out_ << "DESCRIPTION:" << std::endl;
        if (has_opts()) {
          out_ << get_opts() << std::endl;
        }
        if (has_generic_opts()) {
          out_ << get_generic_opts() << std::endl;
        }
      }

      return;
    }

    void base_command::_forbid_short_ui_options()
    {
      _no_short_ui_options_ = true;
      return;
    }

    base_command::opts_type & base_command::_grab_generic_opts()
    {
      if (_generic_opts_.get() == nullptr) {
        _alloc_generic_opts();
      }
      return *_generic_opts_.get();
    }

    void base_command::_alloc_generic_opts()
    {
      _generic_opts_.reset(new opts_type("Generic options"));
      return;
    }

    void base_command::_free_generic_opts()
    {
      if (_generic_opts_.get() != nullptr) _generic_opts_.reset();
      return;
    }

    base_command::opts_type & base_command::_grab_opts()
    {
      if (_opts_.get() == nullptr) {
        _alloc_opts("Arguments");
      }
      return *_opts_.get();
    }

    base_command::vmap_type & base_command::_grab_vmap()
    {
      if (_vmap_.get() == nullptr) {
        _alloc_vmap();
      }
      return *_vmap_.get();
    }

    base_command::args_type & base_command::_grab_args()
    {
      if (_args_.get() == nullptr) {
        _alloc_args();
      }
      return *_args_.get();
    }

    void base_command::_alloc_opts(const std::string & opts_title_)
    {
      _opts_.reset(new opts_type(opts_title_));
      return;
    }

    void base_command::_free_opts()
    {
      if (_opts_.get() != nullptr) _opts_.reset();
      return;
    }

    void base_command::_alloc_args()
    {
      _args_.reset(new args_type);
      return;
    }

    void base_command::_free_args()
    {
      if (_args_.get() != nullptr) _args_.reset();
      return;
    }

    void base_command::_alloc_vmap()
    {
      _vmap_.reset(new vmap_type);
      return;
    }

    void base_command::_free_vmap()
    {
      if (_vmap_.get() != nullptr) _vmap_.reset();
      return;
    }

    // virtual
    void base_command::_init(const properties & config_)
    {
      namespace po = boost::program_options;

      if (config_.has_key("no_generic_options")) {
        _no_generic_options_ = config_.fetch_boolean("no_generic_options");
      }

      if (config_.has_key("no_generic_help")) {
        _no_generic_help_ = config_.fetch_boolean("no_generic_help");
      }

      if (config_.has_key("no_short_ui_options")) {
        _no_short_ui_options_ = config_.fetch_boolean("no_short_ui_options");
      }

      if (use_generic_options()) {

        // Options description:
        po::options_description_easy_init easy_init
          = _grab_generic_opts().add_options();

        if (use_generic_help()) {
          easy_init("help",
                    "Produce help message");
        }

        if (has_version()) {
            easy_init("version",
                      "Print version");
        }

        easy_init("logging",
                  po::value<std::string>()
                  ->value_name("level"),
                  "Set the logging priority\n"
                  "Allowed values are:\n"
                  "  - \"trace\"       : \tHeavy development messages\n"
                  "  - \"debug\"       : \tDebug messages\n"
                  "  - \"information\" : \tInformation messages\n"
                  "  - \"notice\"      : \tNotice messages\n"
                  "  - \"warning\"     : \tWarning messages\n"
                  "  - \"error\"       : \tError messages\n"
                  "  - \"critical\"    : \tCritical error messages\n"
                  "  - \"fatal\"       : \tFatal error messages"
                  );

        // end of options description
      }

      return;
    }

    // virtual
    void base_command::_fini()
    {
      _free_vmap();
      _free_args();
      _free_opts();
      _free_generic_opts();
      if (_version_) _version_.reset();
      return;
    }

    // virtual
    void base_command::_base_run(datatools::command::returned_info & cri_,
                                 uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);

      if (use_generic_options()) {

        if (use_generic_help()) {
          if (_grab_vmap().count("help")) {
            print_usage(std::cout);
            cri_.set_error_code(datatools::command::CEC_STOP);
            return;
          }
        }

        if (has_version()) {
          if (_grab_vmap().count("version")) {

            if (has_version()) {
              print_version(std::cout);
              cri_.set_error_code(datatools::command::CEC_STOP);
            } else {
              DT_COMMAND_RETURNED_ERROR(cri_,
                                        datatools::command::CEC_FAILURE,
                                        "Command '" << get_name() << "' has no available version information!");
            }
            return;
          }
        }

        // Fetch the verbosity level:
        if (_grab_vmap().count("logging")) {
          const std::string & logging_label = _grab_vmap()["logging"].as<std::string>();
          datatools::logger::priority logging = datatools::logger::get_priority(logging_label);
          if (logging == datatools::logger::PRIO_UNDEFINED) {
            DT_COMMAND_RETURNED_ERROR(cri_,
                                      datatools::command::CEC_FAILURE,
                                      "Invalid logging priority label '" << logging_label << "' !");
            return;
          }
          set_logging_priority(logging);
        }
      }

      return;
    }

    void base_command::call(const std::vector<std::string> & argv_,
                            datatools::command::returned_info & cri_,
                            uint32_t flags_)
    {
      this->operator()(argv_, cri_, flags_);
      return;
    }

    int base_command::operator()(const std::vector<std::string> & argv_,
                                 datatools::command::returned_info & cri_,
                                 uint32_t flags_)
    {
      DT_THROW_IF(is_disabled(), std::logic_error, "Command '" << get_name() << "' is disabled!");

      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      for (auto arg : argv_) {
        DT_LOG_TRACE(get_logging_priority(), "  With arg = '" << arg << "'");
      }
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);

      // Allocate variables map:
      _alloc_vmap();

      try {
        // Parse command line:
        namespace po = boost::program_options;
        // Build options:
        po::options_description cl_options;
        if (has_generic_opts()) {
          cl_options.add(*_generic_opts_.get());
        }
        if (has_opts()) {
          cl_options.add(*_opts_.get());
        }
        // Build parser from options and positional args :
        po::command_line_parser cl_parser(argv_);
        cl_parser.options(cl_options);
        if (_args_.get() != nullptr) {
          cl_parser.positional(*_args_.get());
        }
        uint32_t cl_style = po::command_line_style::unix_style;
        if (!has_short_ui_options()) {
          cl_style ^= po::command_line_style::allow_short;
        }
        cl_parser.style(cl_style);
        // cl_parser.style(po::command_line_style::unix_style ^ po::command_line_style::allow_short);
        // cl_parser.allow_unregistered();
        po::parsed_options parsed = cl_parser.run();
        // std::vector<std::string> unrecog = po::collect_unrecognized(parsed.options,
        //                                                          po::exclude_positional);
        // And store options in the variables map:
        po::store(parsed, *_vmap_.get());
        po::notify(*_vmap_.get());
      } catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_COMMAND_INVALID_SYNTAX,
                                  get_name() + ": " + error.what());
      } catch (...) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  get_name() + ": " + "Unexpected error!");
      }

      if (cri_.is_success()) {
        try {
          _base_run(cri_, flags_);
          if (cri_.is_stop()) {
            cri_.set_success();
          } else if (cri_.is_success()) {
            // Run command:
            _run(cri_, flags_);
          }
          set_logging_priority(datatools::logger::PRIO_FATAL);
        } catch (std::exception & error) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_COMMAND_INVALID_SYNTAX,
                                    get_name() + ": " + error.what());
        } catch (...) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_FAILURE,
                                    get_name() + ": " + "Unexpected error!");
        }
      }

      // Free variables map:
      _free_vmap();
      return cri_.get_error_code();
    }

    datatools::command::returned_info
    base_command::operator()(const std::vector<std::string> & argv_,
                             uint32_t flags_)
    {
      datatools::command::returned_info cri;
      this->operator()(argv_, cri, flags_);
      return cri;
    }

    void base_command::tree_dump(std::ostream & out_,
                                 const std::string & title_,
                                 const std::string & indent_,
                                 bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      if (has_version()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Version     : '" << _version_.get() << "'" << std::endl;
      }

      if (has_parent_interface()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Parent interface  : '" << _parent_interface_->get_name() << "'" << std::endl;
      }

      if (has_generic_opts()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Generic options   : [" << get_generic_opts().options().size() << "]" << std::endl;
      }

      if (has_opts()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Options   : [" << get_opts().options().size() << "]" << std::endl;

        out_ << indent_ << i_tree_dumpable::tag
             << "Positional options  : " << ( has_args() ? "yes" : "no" )  << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << is_initialized() << std::endl;

      return;
    }

  } // namespace ui

} // namespace datatools
