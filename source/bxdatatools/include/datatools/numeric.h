/// \file datatools/numeric.h
/// \brief Functions for numeric types

#ifndef DATATOOLS_NUMERIC_H
#define DATATOOLS_NUMERIC_H

#include <datatools/return_types.h>

namespace datatools {

  /* Single precision utility functions */

  /// Invalidate a float (set at NaN):
  /// @arg value the real number to be invalidated
  void invalidate(float& value_);

  /// Return invalid single precision float (set at NaN):
  float invalid_real_single();

  /// Check if a float is valid (not a NaN):
  /// @arg value the real number to be checked
  bool is_valid(float value_);

  /// Check if a float is infinity (+/-inf):
  /// @arg value the real number to be checked
  bool is_infinity(float value_);

  /// Check if a float is plus infinity (+inf):
  /// @arg value the real number to be checked
  bool is_plus_infinity(float value_);

  /// Check if a float is minus infinity (-inf):
  /// @arg value the real number to be checked
  bool is_minus_infinity(float value_);

  /// Set a float to +infinity:
  /// @arg value the real number to be set
  void plus_infinity(float& value_);

  /// Set a float to -infinity:
  /// @arg value the real number to be set
  void minus_infinity(float& value_);

  /// Set a float to +infinity:
  /// @arg value the real number to be set
  void infinity(float& value_);

  /* Double precision utility functions */

  /// Invalidate a double (set at NaN):
  /// @arg value the real number to be invalidated
  void invalidate(double& value_);

  /// Return invalid double precision real (set at NaN):
  double invalid_real_double();

  /// Return invalid double precision real (set at NaN):
  double invalid_real();

  /// Check if a double is valid (not a NaN):
  /// @arg value the real number to be checked
  bool is_valid(double value_);

  /// Check if a double is infinity (+/-inf):
  /// @arg value the real number to be checked
  bool is_infinity(double value_);

  /// Check if a double is plus infinity (+inf):
  /// @arg value the real number to be checked
  bool is_plus_infinity(double value_);

  /// Check if a double is minus infinity (-inf):
  /// @arg value the real number to be checked
  bool is_minus_infinity(double value_);

  /// Check if a double is normal (not a NaN and not inf):
  /// @arg value the real number to be checked
  bool is_normal(double value_);

  /// Set a double to +infinity:
  /// @arg value the real number to be set
  void plus_infinity(double& value_);

  /// Set a double to -infinity:
  /// @arg value the real number to be set
  void minus_infinity(double& value_);

  /// Set a double to +infinity:
  /// @arg value the real number to be set
  void infinity(double& value_);

  /// Compare two real numbers given explicit absolute and/or relative tolerances
  /// @arg x1_ the first value to be compared
  /// @arg x2_ the second value to be compared
  /// @arg abs_eps_ the absolute precision for the comparison
  /// @arg rel_eps_ the relative precision for the comparison
  /// @return the comparison result code
  compare_result_type compare_real(double x1_, double x2_,
                                   double abs_eps_ = 0.0, double rel_eps_ = 0.0);

} // namespace datatools

#endif // DATATOOLS_NUMERIC_H

