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

  bool base_sampling::value_is_valid(const double value_) const
  {
    if (value_ < get_min()) return false;
    if (value_ > get_max()) return false;
    return true;
  }

} // end of namespace mygsl
