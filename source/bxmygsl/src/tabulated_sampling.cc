// \file mygsl/tabulated_sampling.cc

// Ourselves:
#include <mygsl/tabulated_sampling.h>

// Standard library:
#include <bitset>

// Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <mygsl/compare_value.h>

namespace mygsl {

  tabulated_sampling::tabulated_sampling()
    : base_sampling()
    , _initialized_(false)
    , _guard_(datatools::invalid_real())
    , _min_step_(datatools::invalid_real())
    , _max_step_(datatools::invalid_real())
  {
    return;
  }

  tabulated_sampling::~tabulated_sampling()
  {
    if (is_initialized()) this->tabulated_sampling::reset();
    return;
  }

  double tabulated_sampling::get_min() const
  {
    DT_THROW_IF(!_table_.size(), std::logic_error, "Empty table!");
    return _table_.front();
  }

  double tabulated_sampling::get_max() const
  {
    DT_THROW_IF(!_table_.size(), std::logic_error, "Empty table!");
    return _table_.back();
  }

  std::size_t tabulated_sampling::get_nsteps() const
  {
    return _table_.size() - 1;
  }

  std::size_t tabulated_sampling::get_nsamples() const
  {
    return _table_.size();
  }

  double tabulated_sampling::get_min_step() const
  {
    return _min_step_;
  }

  double tabulated_sampling::get_max_step() const
  {
    return _max_step_;
  }

  double tabulated_sampling::get_guard() const
  {
    return _guard_;
  }

  bool tabulated_sampling::is_initialized() const
  {
    return _initialized_;
  }

  void tabulated_sampling::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is already initialized!");

    std::vector<double> ordered_values;
    double guard = datatools::invalid_real();
    const datatools::units::unit_dimension * udim = nullptr;
    bool without_unit = false;

    const datatools::units::registry & ureg
      = datatools::units::registry::const_system_registry();

    if (config_.has_key("unit_dimension")) {
      const std::string & udim_label = config_.fetch_string("unit_dimension");
      DT_THROW_IF(!ureg.has_dimension(udim_label), std::logic_error,
                  "Unsupported unit dimension '" << udim_label << "'!");
      udim = &ureg.get_dimension(udim_label);
    }

    // Fetch values:
    if (config_.has_key("values")) {
      config_.fetch("values", ordered_values);
      if (config_.has_explicit_unit("values")) {
        DT_THROW_IF(without_unit, std::logic_error, "No unit is expected!");
        const std::string & unit_symbol = config_.get_unit_symbol("values");
        const datatools::units::unit_dimension * udim_values
          = &ureg.get_dimension(ureg.get_unit(unit_symbol).get_dimension_label());
        if (udim == nullptr) {
          udim = udim_values;
        } else {
          DT_THROW_IF(udim != udim_values,
                      std::logic_error,
                      "Uncompatible unit dimension '" << udim_values->get_label()
                      << "' (expected '" <<  udim->get_label()<< "')!");
        }
      } else {
        without_unit = true;
      }
    }

    // Fetch guard:
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

    if (ordered_values.size()) {
      make_ordered(ordered_values, guard);
    }

    if (!_initialized_) _initialized_ = true;
    return;
  }

  void tabulated_sampling::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    _initialized_ = false;
    _reset_();
    return;
  }

  void tabulated_sampling::_check_guard_()
  {
    DT_THROW_IF(_guard_ > 0.25 * get_min_step(), std::logic_error,
                "Absolute guard distance is too large [" << _guard_ << " > min(step)/4 == " <<  0.25 * get_min_step() << "]!");
    return;
  }

  void tabulated_sampling::_init_()
  {
    DT_THROW_IF(_table_.size() < sampling::MIN_NUMBER_OF_SAMPLING_VALUES,
                std::logic_error,
                "Table has not enough sampling values [" << _table_.size()
                << " < " << sampling::MIN_NUMBER_OF_SAMPLING_VALUES << "]!");
    if (!datatools::is_valid(_guard_)) {
      _set_guard_(get_min_step() * std::numeric_limits<double>::epsilon());
    }
    _check_guard_();
    _initialized_ = true;
    return;
  }

  void tabulated_sampling::_reset_()
  {
    _initialized_ = false;
    datatools::invalidate(_min_step_);
    datatools::invalidate(_max_step_);
    datatools::invalidate(_guard_);
    _table_.clear();
    return;
  }

  void tabulated_sampling::lock()
  {
    if (is_initialized()) return;
    _init_();
    return;
  }

  void tabulated_sampling::_set_guard_(double guard_)
  {
    if (guard_ <= 0.0) {
      datatools::invalidate(_guard_);
    } else {
      if (datatools::is_valid(_min_step_)) {
        DT_THROW_IF(guard_ >= 0.25 * get_min_step(), std::domain_error,
                    "Absolute guard distance is larger than [min step/4=" << 0.25 * get_min_step() << "]!");
      }
      _guard_ = guard_;
    }
    return;
  }

  bool tabulated_sampling::value_to_index(const double x_,
                                          std::size_t & index_,
                                          sampling::index_classification_type & classification_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    classification_ = sampling::INDEX_UNDEFINED;
    index_ = sampling::INVALID_INDEX;
    if (!datatools::is_valid(x_)) {
      return false;
    }

    {
      compare_score_type cs = compare_absolute(x_, get_min(), 0.0);
      if (cs == COMPARE_LESS) {
        classification_ = sampling::INDEX_UNDERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    {
      compare_score_type cs = compare_absolute(x_, get_max(), 0.0);
      if (cs == COMPARE_GREATER) {
        classification_ = sampling::INDEX_OVERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    classification_ = sampling::INDEX_NORMAL;
    std::size_t index = 0;
    for (; index < get_nsteps(); index++) {
      compare_score_type cs = compare_absolute(x_, _table_[index+1], 0.0);
      if (cs == COMPARE_LESS) {
        break;
      }
    }
    index_ = index;
    return true;
  }

  sampling::index_classification_type
  tabulated_sampling::index_to_value(const std::size_t index_,
                                     double & value_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    sampling::index_classification_type ic = sampling::INDEX_UNDEFINED;
    datatools::invalidate(value_);
    if (index_ != sampling::INVALID_INDEX) {
      if (index_ > get_nsteps()) {
        ic = sampling::INDEX_OVERFLOW;
        datatools::invalidate(value_);
      } else {
        ic = sampling::INDEX_NORMAL;
        value_ = _table_[index_];
      }
    }
    return ic;
  }

  void tabulated_sampling::set_guard(const double guard_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is initialized and locked!");
    _set_guard_(guard_);
    return;
  }

  void tabulated_sampling::append(const double value_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Locked sampling table!");
    DT_THROW_IF(!datatools::is_valid(value_), std::logic_error, "Invalid/NaN value!");
    DT_THROW_IF(std::isinf(value_), std::logic_error, "Invalid infinite value!");
    DT_THROW_IF(_table_.size() > 0 && value_ <= _table_.back(), std::domain_error,
                "Appended value [" << value_ << "] is smaller than the last sampling value [" << _table_.back() << "] !");
    if (_table_.size() >= sampling::MIN_NUMBER_OF_SAMPLING_VALUES) {
      if (value_ - _table_.back() < _min_step_) {
        _min_step_ = value_ - _table_.back();
      }
      if (value_ - _table_.back() > _max_step_) {
        _max_step_ = value_ - _table_.back();
      }
    }
    _table_.push_back(value_);
    return;
  }

  void tabulated_sampling::make_ordered(const std::vector<double> & values_, const double guard_)
  {
    if (is_initialized()) {
      reset();
    }
    DT_THROW_IF(values_.size() < 2, std::logic_error, "Array of values is empty!");
    for (const auto val : values_) {
      append(val);
    }
    _set_guard_(guard_);
    _init_();
    if (!is_initialized()) {
      _initialized_ = true;
    }
    return;
  }

} // end of namespace mygsl
