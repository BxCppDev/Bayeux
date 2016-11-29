/// \file mygsl/compare_value.h

#ifndef MYGSL_COMPARE_VALUE_H
#define MYGSL_COMPARE_VALUE_H

namespace mygsl {

  enum compare_score_type {
    COMPARE_LESS    = -1,
    COMPARE_EQUAL   =  0,
    COMPARE_GREATER =  1,
    COMPARE_UNORDERED =  2
  };

  compare_score_type compare_relative(double x_, double y_, double epsilon_);

  compare_score_type compare_absolute(double x_, double y_, double epsilon_);

  bool approximately_equal(double x_, double y_, double epsilon_);

  bool essentially_equal(double x_, double b, double epsilon_);

  bool definitely_greater_than(double x_, double y_, double epsilon_);

  bool definitely_less_than(double x_, double b, double epsilon_);

  bool nearly_equal(double x_, double y_, double epsilon_);

} // namespace mygsl

#endif // MYGSL_COMPARE_VALUE_H
