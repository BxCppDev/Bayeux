// \file mygsl/base_sampling.cc

// Ourselves:
#include <mygsl/base_sampling.h>

// Standard library:
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

namespace mygsl {

  base_sampling::base_sampling()
  {
    return;
  }

  base_sampling::~base_sampling()
  {
    return;
  }

  bool base_sampling::value_to_index(const double x_,  std::size_t & index_) const
  {
    sampling::index_classification_type dummy_ic = sampling::INDEX_UNDEFINED;
    return value_to_index(x_, index_, dummy_ic);
  }

  bool base_sampling::has_index(const double value_, const std::size_t index_) const
  {
    std::size_t index = sampling::INVALID_INDEX;
    sampling::index_classification_type ic = sampling::INDEX_UNDEFINED;
    if (value_to_index(value_, index, ic)) {
      return index == index_;
    }
    return false;
  }

  std::size_t base_sampling::get_first_index() const
  {
    return 0;
  }

  std::size_t base_sampling::get_last_index() const
  {
    return this->get_nsamples() - 1;
  }

  std::size_t base_sampling::get_first_step_index() const
  {
    return 0;
  }

  std::size_t base_sampling::get_last_step_index() const
  {
    return this->get_nsteps() - 1;
  }

  bool base_sampling::index_is_valid(const std::size_t index_) const
  {
    if (index_ >= get_nsamples()) return false;
    return true;
  }

  bool base_sampling::value_is_valid(const double value_) const
  {
    if (value_ < get_min()) return false;
    if (value_ > get_max()) return false;
    return true;
  }

  bool base_sampling::compute_step_bounds(const std::size_t step_index_,
                                          double & min_,
                                          double & max_) const
  {
    double min, max;
    datatools::invalidate(min);
    datatools::invalidate(max);
    if (step_index_ > get_last_step_index()) return false;
    if (step_index_ == get_first_step_index()) {
      min = get_min();
    } else {
      sampling::index_classification_type icmin = this->index_to_value(step_index_, min);
      if (icmin != sampling::INDEX_NORMAL) return false;
    }
    if (step_index_ == get_last_step_index()) {
      max = get_max();
    } else {
      sampling::index_classification_type icmax = this->index_to_value(step_index_ + 1, max);
      if (icmax != sampling::INDEX_NORMAL) return false;
    }
    min_ = min;
    max_ = max;
    return true;
  }

} // end of namespace mygsl
