// mygsl::min_max.cc

#include <mygsl/min_max.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

namespace mygsl {

  using namespace std;

  double min_max::get_min () const
  {
    return _min_;
  }

  double min_max::get_max () const
  {
    return _max_;
  }

  bool min_max::is_valid () const
  {
    return _min_ <= _max_;
  }

  double min_max::get_median () const
  {
    return 0.5 * (_min_ + _max_);
  }

  min_max::min_max ()
  {
    reset ();
    return;
  }

  void min_max::reset ()
  {
    _min_ = 0.0;
    _max_ = -1.0;
    return;
  }

  void min_max::add (double value_)
  {
    if (_min_ > _max_) {
      _min_ = value_;
      _max_ = value_;
    } else {
      if (value_ < _min_) {
        _min_ = value_;
      }
      if (value_ > _max_) {
        _max_ = value_;
      }
    }
    return;
  }

}

// end of mygsl::min_max.cc

