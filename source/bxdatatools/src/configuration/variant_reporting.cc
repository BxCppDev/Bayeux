// datatools/configuration/variant_reporting.cc
// Ourselves
#include <datatools/configuration/variant_reporting.h>

// This project
#include <datatools/configuration/variant_registry.h>

namespace datatools {
  namespace configuration {
    void variant_reporting::_init_repository_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::vector<std::string> regkeys;
      _repository_->build_ordered_registry_keys(regkeys);

      for (const std::string& regname : regkeys) {
        if (!_repository_->is_active_registry(regname)) continue;
        const variant_registry & reg = _repository_->get_registry(regname);
        std::vector<std::string> setpars;
        reg.list_of_parameters(setpars, variant_registry::LIST_ACTIVE_ONLY);
        for (const std::string& parname : setpars) {
          std::string fullparpath = regname + ":" + parname;
          _parameter_stats_[fullparpath] = 0;
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

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

  } // namespace configuration
} // namespace datatools
