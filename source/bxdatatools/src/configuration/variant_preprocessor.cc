// datatools/configuration/variant_preprocessor.cc
// Ourselves
#include <datatools/configuration/variant_preprocessor.h>

// Third party
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

// This project
#include <datatools/ioutils.h>
#include <datatools/kernel.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_reporting.h>
#include <datatools/configuration/ui/variant_repository_cli.h>

namespace datatools {
  namespace configuration {

    variant_preprocessor::variant_preprocessor(unsigned int flags_)
    {
      if (flags_ & FLAG_TRACE) {
        set_logging(logger::PRIO_TRACE);
      }
      if (flags_ & FLAG_REMOVE_QUOTES) {
        set_remove_quotes(true);
      }

      if (datatools::kernel::is_instantiated()) {
        const datatools::kernel & dtkl = datatools::kernel::const_instance();
        if (dtkl.has_effective_variant_repository()) {
          set_repository(dtkl.get_effective_variant_repository());
        }
      }

      return;
    }

    variant_preprocessor::variant_preprocessor(const variant_repository& rep_, unsigned int flags_)
    {
      if (flags_ & FLAG_TRACE) {
        set_logging(logger::PRIO_TRACE);
      }
      if (flags_ & FLAG_REMOVE_QUOTES) {
        set_remove_quotes(true);
      }
      set_repository(rep_);
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
        // If variant repository is not activated:
        if (has_variant_default) {
          // We fallback to the default value if any. Syntax is:
          //   {variant-description}|true
          //   {variant-description}|false
          variant_active_ = variant_default;
          variant_reverse_ = false;
          return cri;
        } else {
          // It is an error: no variant repository nor fallback default value is available:
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_CONTEXT_INVALID,
                                    "Inactive variant repository while processing '" << variant_desc << "' !");
          return cri;
        }
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
      for (const std::string& arg : args_) {
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
