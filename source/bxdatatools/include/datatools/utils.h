// -*- mode: c++; -*-
/// \file datatools/utils.h

#ifndef DATATOOLS_UTILS_H
#define DATATOOLS_UTILS_H

// Standard Library:
#include <cstdlib>
#include <list>
#include <string>

// This Project:
#include "datatools/exception.h"

namespace datatools {

/// \brief Return codes
enum return_code_type {
  SUCCESS = EXIT_SUCCESS,
  ERROR   = EXIT_FAILURE,
  FAILURE = EXIT_FAILURE
};

/// \brief Comparison results for ordered objects
enum compare_result_type {
  COMPARE_UNDEFINED = -2, //!< Comparison cannot be performed
  COMPARE_LESS      = -1, //!< Less than
  COMPARE_EQUAL     =  0, //!< Equal to
  COMPARE_GREATER   =  1  //!< Greater than
};

/* Single precision utility functions */

/// Invalidate a float (using NaN):
void invalidate(float& value);

/// Check if a float is valid (not a NaN):
bool is_valid(float value);

/// Check if a float is infinity (+/-inf):
bool is_infinity(float value);

/// Check if a float is plus infinity (+inf):
bool is_plus_infinity(float value);

/// Check if a float is minus infinity (-inf):
bool is_minus_infinity(float value);

/// Set a float to +infinity:
void plus_infinity(float& value);

/// Set a float to -infinity:
void minus_infinity(float& value);

/// Set a float to +infinity:
void infinity(float& value);

/* Double precision utility functions */

/// Invalidate a double (using NaN):
void invalidate(double& value);

/// Check if a double is valid (not a NaN):
bool is_valid(double value);

/// Check if a double is infinity (+/-inf):
bool is_infinity(double value);

/// Check if a double is plus infinity (+inf):
bool is_plus_infinity(double value);

/// Check if a double is minus infinity (-inf):
bool is_minus_infinity(double value);

/// Check if a double is normal (not a NaN and not inf):
bool is_normal(double value);

/// Set a double to +infinity:
void plus_infinity(double& value);

/// Set a double to -infinity:
void minus_infinity(double& value);

/// Set a double to +infinity:
void infinity(double& value);

/// Compare two real numbers given explicit absolute and/or relative tolerances
compare_result_type compare_real(double x1_, double x2_, double abs_eps_ = 0.0, double rel_eps_ = 0.0);

/* String utility functions */

/// Check if a string is quoted
bool is_quoted(const std::string & text_, char q_ = '"');

/// Add quotes to a string
void add_quotes(const std::string & from_, std::string & to_, char q_ = '"');

/// Add quotes to a string
void add_quotes(std::string & text_, char q_ = '"');

/// Remove quotes from a string
void remove_quotes(const std::string & from_, std::string & to_, char q_ = '"');

/// Remove quotes from a string
void remove_quotes(std::string & text_, char q_ = '"');

/* Functions to expand string as path */

/** Extract the keys used by library information registration mechanism
 */
bool resolve_library_info_path_keys(const std::string & library_topic_,
                                    std::string & install_path_key_,
                                    std::string & environ_path_key_);

/** Fetch the expanded path computed from the 'word' string.
 */
std::string fetch_path(const std::string& path_str);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'getenv' function and the 'datatools::library_info'
 * register.
 */
bool fetch_path_with_env(std::string& word);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'datatools::library_info'
 * register.
 */
bool fetch_path_without_env(std::string& word);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'getenv' function and the datatools::library_info
 * register.
 */
bool fetch_path_with_env_p(std::string& word,
                           const std::string & parent_path_);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'getenv' function and the datatools::library_info
 * register.
 */
bool fetch_path_with_env_g(std::string& word);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'getenv' function and the datatools::library_info
 * register.
 */
bool fetch_path_with_env_pg(std::string& word,
                            const std::string & parent_path_);

/** Check if datatools has its global path activated */
bool has_global_path();

/** Reset the datatools' global path */
void reset_global_path();

/** Set/activate the datatools' global path */
void set_global_path(const std::string & gpath_);

/** Get the datatools' global path */
const std::string & get_global_path();


//! Extract the expanded path computed from the 'word' string.
//! Internally uses the system's shell.
//!
//! This method is not very elegant. It uses
//! brute force, asking a shell to interpret the string via a 'system' call
//! and using a temporary file to save the result. Urrkkk!
//! That enables the expansion of environment variables embeded
//! in the 'path_str' string:
//!
//! Example: '$HOME/foo.dat' is expanded to '/home/&gt;login&lt;/foo.dat'
//! @deprecated Use the fetch_path function.
std::string expand_path(const std::string& path_str);

/** The function splits the string 'a_word' using separators given
 *  within a string 'a_separators' and provides the list of tokens
 *  'a_words'.
 *
 * The Boost library provides some powerful alternative through
 * the tokenizer class and split string algorithms.
 *
 */
void split_string(const std::string& word, const std::string& separators,
                  std::list<std::string>& words);

/* Bit manipulation utility template functions */

template<typename Integral>
void set_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= (int) sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number |= 1 << pos;
}


template<typename Integral>
void unset_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= (int) sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number &= ~(1 << pos);
}


template<typename Integral>
void toggle_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= (int) sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number ^= 1 << pos;
}


template<typename Integral>
bool check_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= (int) sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");

  if (number & (1 << pos)) return false;
  return true;
}

} // namespace datatools

#endif // DATATOOLS_UTILS_H
