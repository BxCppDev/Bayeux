// mygsl/compare_value.cc

// Ourselves:
#include <mygsl/compare_value.h>

// Standard library:
#include <cmath>
#include <limits>
#include <algorithm>

// GSL:
#include <gsl/gsl_math.h>

namespace mygsl {

  compare_score_type compare_relative(double x_, double y_, double epsilon_)
  {
    if (std::isnan(x_)) return COMPARE_UNORDERED;
    if (std::isnan(y_)) return COMPARE_UNORDERED;
    double eps = epsilon_;
    if (std::isnan(eps) || eps <= 0.0) {
      eps = std::numeric_limits<double>::epsilon();
    }
    int result = gsl_fcmp(x_, y_, eps);
    if (result == -1) return COMPARE_LESS;
    if (result == 1) return COMPARE_GREATER;
    return COMPARE_EQUAL;
  }

  compare_score_type compare_absolute(double x_, double y_, double epsilon_)
  {
    if (std::isnan(x_)) return COMPARE_UNORDERED;
    if (std::isnan(y_)) return COMPARE_UNORDERED;
    double eps = epsilon_;
    if (std::isnan(eps) || eps <= 0.0) {
      eps = std::numeric_limits<double>::epsilon();
    }
    if (x_ < y_ - eps) return COMPARE_LESS;
    if (x_ > y_ + eps) return COMPARE_GREATER;
    return COMPARE_EQUAL;
  }

  bool approximately_equal(double x_, double y_, double epsilon_)
  {
    return std::abs(x_ - y_) <= ( (std::abs(x_) < std::abs(y_) ? std::abs(y_) : std::abs(x_)) * epsilon_);
  }

  bool essentially_equal(double x_, double y_, double epsilon_)
  {
    return std::abs(x_ - y_) <= ( (std::abs(x_) > std::abs(y_) ? std::abs(y_) : std::abs(x_)) * epsilon_);
  }

  bool definitely_greater_than(double x_, double y_, double epsilon_)
  {
    return (x_ - y_) > ( (std::abs(x_) < std::abs(y_) ? std::abs(y_) : std::abs(x_)) * epsilon_);
  }

  bool definitely_less_than(double x_, double y_, double epsilon_)
  {
    return (y_ - x_) > ( (std::abs(x_) < std::abs(y_) ? std::abs(y_) : std::abs(x_)) * epsilon_);
  }

  // See: http://stackoverflow.com/questions/5064377/c-comparing-two-floating-point-values
  bool nearly_equal(double x_, double y_, double epsilon_)
  {
    double eps = epsilon_;
    if (std::isnan(eps) || eps <= 0.0) {
      eps = std::numeric_limits<double>::epsilon();
    }
    if (x_ == 0 && y_ == 0) return true;
    return std::abs(x_ - y_) < eps * std::pow (2.0, static_cast<int> (std::log2(std::max(std::abs(x_), std::abs(y_)))));
  }

}
