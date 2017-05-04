// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>

// This project:
#include <mygsl/logistic_function.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(logistic_function,
                                             "mygsl::logistic_function")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(logistic_function)

  void logistic_function::_set_defaults()
  {
    datatools::invalidate(_steepness_);
    datatools::invalidate(_midpoint_);
    datatools::invalidate(_amplitude_);
    return;
  }

  logistic_function::logistic_function()
  {
    _set_defaults();
    return;
  }

  logistic_function::logistic_function(double steepness_,
                                       double midpoint_,
                                       double amplitude_)
  {
    _set_defaults();
    set_steepness(steepness_);
    set_midpoint(midpoint_);
    set_amplitude(amplitude_);
    return;
  }

  logistic_function::~logistic_function()
  {
    return;
  }

  void logistic_function::set_steepness(double steepness_)
  {
    DT_THROW_IF(datatools::is_valid(steepness_) && steepness_ <= 0.0, std::logic_error,
                "Invalid negative or null steepness!");
    _steepness_ = steepness_;
    return;
  }

  double logistic_function::get_steepness() const
  {
    return _steepness_;
  }

  void logistic_function::set_midpoint(double midpoint_)
  {
    _midpoint_ = midpoint_;
    return;
  }

  double logistic_function::get_midpoint() const
  {
    return _midpoint_;
  }

  void logistic_function::set_amplitude(double amplitude_)
  {
    DT_THROW_IF(datatools::is_valid(amplitude_) && amplitude_ < 0.0, std::logic_error,
                "Invalid negative amplitude!");
    _amplitude_ = amplitude_;
    return;
  }

  double logistic_function::get_amplitude() const
  {
    return _amplitude_;
  }

  bool logistic_function::is_initialized() const
  {
    return datatools::is_valid(_steepness_)
      && datatools::is_valid(_midpoint_)
      && datatools::is_valid(_amplitude_);
  }

  void logistic_function::initialize(const datatools::properties & config_,
                                     const unary_function_dict_type & functors_)
  {
    this->i_unary_function::_base_initialize(config_, functors_);

    // Parse configuration:
    if (!datatools::is_valid(_steepness_)) {
      if (config_.has_key("steepness")) {
        double s = config_.fetch_real("steepness");
        set_steepness(s);
      }
    }

    if (!datatools::is_valid(_midpoint_)) {
      if (config_.has_key("midpoint")) {
        double m = config_.fetch_real("midpoint");
        set_midpoint(m);
      }
    }

    if (!datatools::is_valid(_amplitude_)) {
      if (config_.has_key("amplitude")) {
        double a = config_.fetch_real("amplitude");
        set_amplitude(a);
      }
    }

    // Checks:
    DT_THROW_IF(!datatools::is_valid(_steepness_),
                std::logic_error,
                "Invalid steepness!");
    DT_THROW_IF(!datatools::is_valid(_midpoint_),
                std::logic_error,
                "Invalid midpoint!");
    DT_THROW_IF(!datatools::is_valid(_amplitude_),
                std::logic_error,
                "Invalid amplitude!");
    return;
  }

  void logistic_function::reset()
  {
    _set_defaults();
    this->i_unary_function::_base_reset();
    return;
  }

  double logistic_function::_eval(double x_) const
  {
    double res = _amplitude_/ (1.0 + std::exp(-_steepness_ * (x_ - _midpoint_)));
    return res;
  }

  void logistic_function::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Steepness : " << _steepness_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Midpoint : " << _midpoint_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Amplitude : " << _amplitude_ << std::endl;

    return;
  }

} // namespace mygsl
