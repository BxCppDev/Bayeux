// \file mygsl/linear_sampling.cc

// Ourselves:
#include <mygsl/linear_sampling.h>

// Standard library:
#include <bitset>

// Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <mygsl/compare_value.h>

namespace mygsl {

  linear_sampling::linear_sampling()
    : base_sampling()
    , _min_(datatools::invalid_real())
    , _max_(datatools::invalid_real())
    , _nsteps_(0)
    , _step_(datatools::invalid_real())
    , _guard_(datatools::invalid_real())
  {
    return;
  }

  linear_sampling::~linear_sampling()
  {
    if (is_initialized()) this->linear_sampling::reset();
    return;
  }

  double linear_sampling::get_min() const
  {
    return _min_;
  }

  double linear_sampling::get_max() const
  {
    return _max_;
  }

  std::size_t linear_sampling::get_nsteps() const
  {
    return _nsteps_;
  }

  std::size_t linear_sampling::get_nsamples() const
  {
    return _nsteps_ + 1;
  }

  double linear_sampling::get_step() const
  {
    return _step_;
  }

  double linear_sampling::get_guard() const
  {
    return _guard_;
  }

  bool linear_sampling::is_initialized() const
  {
    return _nsteps_ >= sampling::MIN_NUMBER_OF_SAMPLING_VALUES;
  }

  void linear_sampling::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is already initialized!");

    double min = datatools::invalid_real();
    double max = datatools::invalid_real();
    std::size_t nsteps = 0;
    double step = datatools::invalid_real();
    double guard = datatools::invalid_real();
    const datatools::units::unit_dimension * udim = nullptr;
    bool without_unit = false;
    std::bitset<4> init_flags;

    const datatools::units::registry & ureg
      = datatools::units::registry::const_system_registry();

    if (config_.has_key("unit_dimension")) {
      const std::string & udim_label = config_.fetch_string("unit_dimension");
      DT_THROW_IF(!ureg.has_dimension(udim_label), std::logic_error,
                  "Unsupported unit dimension '" << udim_label << "'!");
      udim = &ureg.get_dimension(udim_label);
    }

    // Fetch min:
    if (config_.has_key("min")) {
      min = config_.fetch_real("min");
      if (config_.has_explicit_unit("min")) {
        DT_THROW_IF(without_unit, std::logic_error, "No unit is expected!");
        const std::string & unit_symbol = config_.get_unit_symbol("min");
        const datatools::units::unit_dimension * udim_min
          = &ureg.get_dimension(ureg.get_unit(unit_symbol).get_dimension_label());
        if (udim == nullptr) {
          udim = udim_min;
        } else {
          DT_THROW_IF(udim != udim_min,
                      std::logic_error,
                      "Uncompatible unit dimension '" << udim_min->get_label()
                      << "' (expected '" <<  udim->get_label()<< "')!");
        }
      } else {
        without_unit = true;
      }
    }
    if (datatools::is_valid(min)) init_flags.set(0);

    // Fetch max:
    if (config_.has_key("max")) {
      max = config_.fetch_real("max");
      if (config_.has_explicit_unit("max")) {
        DT_THROW_IF(without_unit, std::logic_error, "No unit is expected!");
        const std::string & unit_symbol = config_.get_unit_symbol("max");
        const datatools::units::unit_dimension * udim_max
          = &ureg.get_dimension(ureg.get_unit(unit_symbol).get_dimension_label());
        if (udim == nullptr) {
          udim = udim_max;
        } else {
          DT_THROW_IF(udim != udim_max,
                      std::logic_error,
                      "Uncompatible unit dimension '" << udim_max->get_label()
                      << "' (expected '" <<  udim->get_label()<< "')!");
        }
      } else {
        without_unit = true;
      }
    }
    if (datatools::is_valid(max)) init_flags.set(1);

    // Fetch nsteps:
    if (config_.has_key("nsteps")) {
      nsteps = config_.fetch_strict_positive_integer("nsteps");
    }
    if (nsteps > 0) init_flags.set(2);

    // Fetch step:
    if (config_.has_key("step")) {
      step = config_.fetch_real("step");
      if (config_.has_explicit_unit("step")) {
        DT_THROW_IF(without_unit, std::logic_error, "No unit is expected!");
        const std::string & unit_symbol = config_.get_unit_symbol("step");
        const datatools::units::unit_dimension * udim_step
          = &ureg.get_dimension(ureg.get_unit(unit_symbol).get_dimension_label());
        if (udim == nullptr) {
          udim = udim_step;
        } else {
          DT_THROW_IF(udim != udim_step,
                      std::logic_error,
                      "Uncompatible unit dimension '" << udim_step->get_label()
                      << "' (expected '" <<  udim->get_label()<< "')!");
        }
      } else {
        without_unit = true;
      }
    }
    if (datatools::is_valid(step)) init_flags.set(3);

    if (config_.has_key("guard")) {
      guard = config_.fetch_real("guard");
      if (config_.has_explicit_unit("guard")) {
        const std::string & unit_symbol = config_.get_unit_symbol("guard");
        const datatools::units::unit_dimension * udim_guard
          = &ureg.get_dimension(ureg.get_unit(unit_symbol).get_dimension_label());
        if (udim == nullptr) {
          udim = udim_guard;
        } else {
          DT_THROW_IF(udim != udim_guard,
                      std::logic_error,
                      "Uncompatible unit dimension '" << udim_guard->get_label()
                      << "' (expected '" <<  udim->get_label()<< "')!");
        }
      }
      guard = config_.fetch_real("guard");
    }

    static const std::bitset<4> init_range("0111");
    static const std::bitset<4> init_from_min("1101");
    static const std::bitset<4> init_to_max("1110");

    if (init_flags == init_range) {
      make_range(min, max, nsteps, guard);
    } else if (init_flags == init_from_min) {
      make_from_min(min, nsteps, step, guard);
    } else if (init_flags == init_to_max) {
      make_to_max(max, nsteps, step, guard);
    }

    return;
  }

  void linear_sampling::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    _reset_();
    return;
  }

  void linear_sampling::_init_()
  {
    DT_THROW_IF(_nsteps_ < sampling::MIN_NUMBER_OF_SAMPLING_VALUES, std::logic_error, "Invalid initialization!");
    if (!datatools::is_valid(_guard_)) {
      _guard_ = _step_ * std::numeric_limits<double>::epsilon();
    }
    return;
  }

  void linear_sampling::_reset_()
  {
    datatools::invalidate(_min_);
    datatools::invalidate(_max_);
    datatools::invalidate(_step_);
    _nsteps_ = 0;
    datatools::invalidate(_guard_);
    return;
  }

  void linear_sampling::_set_guard_(double guard_)
  {
    if (guard_ <= 0.0) {
      datatools::invalidate(_guard_);
    } else {
      DT_THROW_IF(guard_ >= 0.25 * _step_, std::domain_error,
                  "Absolute guard distance is too large [" << _guard_ << " > step/4 ==" << 0.25 * _step_ << "]!");
      _guard_ = guard_;
    }
    return;
  }

  bool linear_sampling::value_to_index(const double x_,
                                       std::size_t & index_,
                                       sampling::index_classification_type & classification_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    if (!datatools::is_valid(x_)) {
      classification_ = sampling::INDEX_UNDEFINED;
      index_ = sampling::INVALID_INDEX;
      return false;
    }

    {
      compare_score_type cs = compare_absolute(x_, _min_, 0.0);
      if (cs == COMPARE_LESS) {
        classification_ = sampling::INDEX_UNDERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    {
      compare_score_type cs = compare_absolute(x_, _max_, 0.0);
      if (cs == COMPARE_GREATER) {
        classification_ = sampling::INDEX_OVERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    classification_ = sampling::INDEX_NORMAL;
    index_ = (x_ - _min_) / _step_;
    return true;
  }

  sampling::index_classification_type
  linear_sampling::index_to_value(const std::size_t index_, double & value_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    sampling::index_classification_type ic = sampling::INDEX_UNDEFINED;
    datatools::invalidate(value_);
    if (index_ != sampling::INVALID_INDEX) {
      if (index_ > _nsteps_) {
        ic = sampling::INDEX_OVERFLOW;
        datatools::invalidate(value_);
      } else {
        ic = sampling::INDEX_NORMAL;
        value_ = _min_ + _step_ * index_;
      }
    }
    return ic;
  }

  void linear_sampling::make_range(const double min_,
                                   const double max_,
                                   const std::size_t nsteps_,
                                   const double guard_)
  {
    if (is_initialized()) reset();
    DT_THROW_IF(!datatools::is_valid(min_) ||
                !std::isfinite(min_),
                std::logic_error,
                "Invalid min sampling value!");
    DT_THROW_IF(!datatools::is_valid(max_) ||
                !std::isfinite(max_),
                std::logic_error,
                "Invalid max sampling value!");
    DT_THROW_IF(min_ >= max_,
                std::domain_error,
                "Invalid max sampling value!");
    _min_ = min_;
    _max_ = max_;
    _nsteps_ = nsteps_;
    _step_ = (_max_ - _min_) / _nsteps_;
    _set_guard_(guard_);
    _init_();
    return;
  }

  void linear_sampling::make_from_min(const double min_,
                                      const std::size_t nsteps_,
                                      const double step_,
                                      const double guard_)
  {
    if (is_initialized()) reset();
    DT_THROW_IF(!datatools::is_valid(min_) ||
                !std::isfinite(min_),
                std::logic_error,
                "Invalid min sampling value!");
    DT_THROW_IF(!datatools::is_valid(step_) ||
                !std::isfinite(step_)||
                step_ <= 0.0,
                std::logic_error,
                "Invalid sampling step value!");
    DT_THROW_IF(nsteps_ < 1,
                std::logic_error,
                "Invalid number of steps!");
    _nsteps_ = nsteps_;
    _step_ = step_;
    _min_ = min_;
    _max_ = min_ + _nsteps_ * step_;
    _set_guard_(guard_);
    _init_();
    return;
  }

  void linear_sampling::make_to_max(const double max_,
                                    const std::size_t nsteps_,
                                    const double step_,
                                    const double guard_)
  {
    if (is_initialized()) reset();
    DT_THROW_IF(!datatools::is_valid(max_) ||
                !std::isfinite(max_),
                std::logic_error,
                "Invalid max sampling value!");
    DT_THROW_IF(!datatools::is_valid(step_) ||
                !std::isfinite(step_)||
                step_ <= 0.0,
                std::logic_error,
                "Invalid sampling step value!");
    DT_THROW_IF(nsteps_ < 1,
                std::logic_error,
                "Invalid number of steps!");
    _nsteps_ = nsteps_;
    _step_ = step_;
    _min_ = max_ - _nsteps_ * step_;
    _max_ = max_;
    _set_guard_(guard_);
    _init_();
    return;
  }

}
