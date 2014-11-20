// min_max.cc

// Ourselves:
#include <mygsl/min_max.h>

// Standard library:
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

namespace mygsl {

  double min_max::get_min() const
  {
    return _min_;
  }

  double min_max::get_max() const
  {
    return _max_;
  }

  bool min_max::is_valid() const
  {
    return _min_ <= _max_;
  }

  double min_max::get_median() const
  {
    return 0.5 * (_min_ + _max_);
  }

  int min_max::get_min_index() const
  {
    return _min_index_;
  }

  int min_max::get_max_index() const
  {
    return _max_index_;
  }

  int min_max::get_counter() const
  {
    return _counter_;
  }

  min_max::min_max()
  {
    reset();
    return;
  }

  void min_max::reset()
  {
    _counter_   =  0;
    _min_       =  0.0;
    _min_index_ = -1;
    _max_       = -1.0;
    _max_index_ = -1;
    return;
  }

  void min_max::add(double value_)
  {
    if (_min_ > _max_) {
      _min_ = value_;
      _max_ = value_;
      _min_index_ = _counter_;
      _max_index_ = _counter_;
    } else {
      if (value_ < _min_) {
        _min_ = value_;
        _min_index_ = _counter_;
      }
      if (value_ > _max_) {
        _max_ = value_;
        _max_index_ = _counter_;
      }
    }
    _counter_++;
    return;
  }


  void min_max::tree_dump(std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {
    if (! title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Counter   : " << _counter_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Min       : " << _min_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Min index : " << _min_index_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Max       : " << _max_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Max index : " << _max_index_ << std::endl;
    return;
  }


} // end of namespace mygsl
