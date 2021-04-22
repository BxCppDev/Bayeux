// numeric.cc

// Ourselves
#include <datatools/numeric.h>

// Standard Library
#include <cmath>
#include <limits>

namespace datatools {
  /* Single precision utility functions */

  void invalidate(float& x) {
    x = std::numeric_limits<float>::quiet_NaN();
  }

  float invalid_real_single()
  {
    return std::numeric_limits<float>::quiet_NaN();
  }

  bool is_valid(float x) {
    return x == x;
  }

  bool is_infinity(float x) {
    return std::isinf(x);
  }

  bool is_plus_infinity(float x)
  {
    return is_infinity(x) && x > 0.0F;
  }

  bool is_minus_infinity(float x)
  {
    return is_infinity(x) && x < 0.0F;
  }

  void plus_infinity(float& x) {
    x = std::numeric_limits<float>::infinity();
  }

  void minus_infinity(float& x) {
    x = -std::numeric_limits<float>::infinity();
  }

  void infinity(float& x) {
    plus_infinity(x);
  }

  /* Double precision utility functions */

  void invalidate(double& x) {
    x = std::numeric_limits<double>::quiet_NaN();
  }

  double invalid_real()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }

  double invalid_real_double()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }

  bool is_valid(double x) {
    return x == x;
  }

  bool is_infinity(double x) {
    return std::isinf(x);
  }

  bool is_plus_infinity(double x)
  {
    return is_infinity(x) && x > 0.0;
  }

  bool is_minus_infinity(double x)
  {
    return is_infinity(x) && x < 0.0;
  }

  bool is_normal(double x) {
    return ! is_infinity(x) && is_valid(x);
  }

  void plus_infinity(double& x) {
    x = std::numeric_limits<double>::infinity();
  }

  void minus_infinity(double& x) {
    x = -std::numeric_limits<double>::infinity();
  }

  void infinity(double& x) {
    plus_infinity(x);
  }

  compare_result_type compare_real(double x1_, double x2_,
                                   double abs_eps_, double rel_eps_)
  {
    if (std::isunordered(x1_,x2_)) {
      // At least one id a NaN: cannot compare NaNs:
      return COMPARE_UNDEFINED;
    }

    if (is_infinity(x1_) && is_infinity(x2_)) {
      // Both numbers are infinite:
      if (is_minus_infinity(x1_) && is_minus_infinity(x2_)) {
        return COMPARE_UNDEFINED;
      }
      if (is_plus_infinity(x1_) && is_plus_infinity(x2_)) {
        return COMPARE_UNDEFINED;
      }
      if (is_minus_infinity(x1_) && is_plus_infinity(x2_)) {
        return COMPARE_LESS;
      }

      if (is_plus_infinity(x1_) && is_minus_infinity(x2_)) {
        return COMPARE_GREATER;
      }
    }

    // Only one number is infinite:
    if (is_minus_infinity(x1_)) {
      return COMPARE_LESS;
    }
    if (is_plus_infinity(x1_)) {
      return COMPARE_GREATER;
    }
    if (is_minus_infinity(x2_)) {
      return COMPARE_GREATER;
    }
    if (is_plus_infinity(x2_)) {
      return COMPARE_LESS;
    }

    // Both numbers are finite:

    // First we try to compare numbers using an absolute tolerance, if given
    double abseps = abs_eps_;
    if (is_valid(abseps) && abseps > 0.0) {
      // We compare them using an absolute tolerance:
      if (x1_ < (x2_ - abseps)) {
        return COMPARE_LESS;
      } else if (x1_ > (x2_ + abseps)) {
        return COMPARE_GREATER;
      } else {
        return COMPARE_EQUAL;
      }
    }

    // If no absolute tolerance is provided, try to use comparison
    // with respect to a relative error:
    double releps = rel_eps_;
    if (!is_valid(abseps) || abseps < 0.0) {
      releps = std::numeric_limits<double>::epsilon();
    }
    double diff = std::abs(x1_ - x2_);
    double ax1 = std::fabs(x1_);
    double ax2 = std::fabs(x2_);
    double largest = (ax1 > ax2) ? ax1 : ax2;
    if (diff <= largest * releps) {
      return COMPARE_EQUAL;
    } else if (x1_ < x2_) {
      return COMPARE_LESS;
    } else {
      return COMPARE_GREATER;
    }

    return COMPARE_UNDEFINED;
  }

} // namespace datatools

