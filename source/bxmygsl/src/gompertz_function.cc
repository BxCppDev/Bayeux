// Standard library:
#include <limits>
#include <cmath>

// This project:
#include <mygsl/gompertz_function.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(gompertz_function,
                                             "mygsl::gompertz_function")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(gompertz_function)

  void gompertz_function::_set_defaults()
  {
    datatools::invalidate(_asymptote_);
    datatools::invalidate(_displacement_);
    datatools::invalidate(_growth_rate_);
    return;
  }

  gompertz_function::gompertz_function()
  {
    _set_defaults();
    return;
  }

  gompertz_function::gompertz_function(double asymptote_,
                                       double displacement_,
                                       double growth_rate_)
  {
    _set_defaults();
    set_asymptote(asymptote_);
    set_displacement(displacement_);
    set_growth_rate(growth_rate_);
    return;
  }

  gompertz_function::~gompertz_function()
  {
    return;
  }

  void gompertz_function::set_asymptote(double asymptote_)
  {
    DT_THROW_IF(datatools::is_valid(asymptote_) && asymptote_ < 0.0,
                std::logic_error,
                "Invalid asymptote!");
    _asymptote_ = asymptote_;
    return;
  }

  double gompertz_function::get_asymptote() const
  {
    return _asymptote_;
  }

  void gompertz_function::set_displacement(double displacement_)
  {
    DT_THROW_IF(datatools::is_valid(displacement_) && displacement_ < 0.0,
                std::logic_error,
                "Invalid displacement!");
    _displacement_ = displacement_;
    return;
  }

  double gompertz_function::get_displacement() const
  {
    return _displacement_;
  }

  void gompertz_function::set_growth_rate(double growth_rate_)
  {
    DT_THROW_IF(datatools::is_valid(growth_rate_) && growth_rate_ < 0.0,
                std::logic_error,
                "Invalid growth rate!");
    _growth_rate_ = growth_rate_;
    return;
  }

  double gompertz_function::get_growth_rate() const
  {
    return _growth_rate_;
  }

  double gompertz_function::_eval(double x_) const
  {
    double res = _asymptote_ * std::exp( - _displacement_ * std::exp( - _growth_rate_ * x_));
    return res;
  }

  bool gompertz_function::is_initialized() const
  {
    return datatools::is_valid(_asymptote_)
      && datatools::is_valid(_displacement_)
      && datatools::is_valid(_growth_rate_);
  }

  void gompertz_function::initialize(const datatools::properties & config_,
                                     unary_function_dict_type & functors_)
  {
    this->i_unary_function::_base_initialize(config_, functors_);

    // Parse configuration:
    if (!datatools::is_valid(_asymptote_)) {
      if (config_.has_key("asymptote")) {
        double asymptote = config_.fetch_real("asymptote");
        set_asymptote(asymptote);
      }
    }

    if (!datatools::is_valid(_displacement_)) {
      if (config_.has_key("displacement")) {
        double displacement = config_.fetch_real("displacement");
        set_displacement(displacement);
      }
    }

    if (!datatools::is_valid(_growth_rate_)) {
      if (config_.has_key("growth_rate")) {
        double growth_rate = config_.fetch_real("growth_rate");
        set_growth_rate(growth_rate);
      }
    }

    // Checks:
    DT_THROW_IF(!datatools::is_valid(_asymptote_),
                std::logic_error,
                "Invalid asymptote!");
    DT_THROW_IF(!datatools::is_valid(_displacement_),
                std::logic_error,
                "Invalid displacement!");
    DT_THROW_IF(!datatools::is_valid(_growth_rate_),
                std::logic_error,
                "Invalid growth rate!");
    return;
  }

  void gompertz_function::reset()
  {
    _set_defaults();
    this->i_unary_function::_base_reset();
    return;
  }

  void gompertz_function::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Asymptote : " << _asymptote_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Displacement : " << _displacement_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Growth rate : " << _growth_rate_ << std::endl;

    return;
  }

} // namespace mygsl
