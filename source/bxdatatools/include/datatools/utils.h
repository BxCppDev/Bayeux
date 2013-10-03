// -*- mode: c++; -*-
// utils.h

#ifndef DATATOOLS_UTILS_H_
#define DATATOOLS_UTILS_H_

// Standard Library
#include <cstdlib>
#include <list>
#include <string>

// Third Party
// - A

// This Project
#include "datatools/exception.h"

namespace datatools {

enum return_code_type {
  SUCCESS = EXIT_SUCCESS,
  ERROR   = EXIT_FAILURE,
  FAILURE = EXIT_FAILURE
};

// Invalidate a float (using NaN):
void invalidate(float& value);

// Check if a float is valid (not a NaN):
bool is_valid(float value);

// Set a float to +infinity:
void plus_infinity(float& value);

// Set a float to -infinity:
void minus_infinity(float& value);

// Set a float to +infinity:
void infinity(float& value);

// Invalidate a double (using NaN):
void invalidate(double& value);

// Check if a double is valid (not a NaN):
bool is_valid(double value);

// Check if a double is infinity (+/-inf):
bool is_infinity(double value);

// Check if a double is normal (not a NaN and not inf):
bool is_normal(double value);

// Set a double to +infinity:
void plus_infinity(double& value);

// Set a double to -infinity:
void minus_infinity(double& value);

// Set a double to +infinity:
void infinity(double& value);

/* Functions to expand string as path */

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
 * the tokenizer class and split string algorithm.
 *
 */
void split_string(const std::string& word, const std::string& separators,
                  std::list<std::string>& words);


template<typename Integral>
void set_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number |= 1 << pos;
}


template<typename Integral>
void unset_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number &= ~(1 << pos);
}


template<typename Integral>
void toggle_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");
  number ^= 1 << pos;
}


template<typename Integral>
bool check_bit(Integral& number, int pos) {
  DT_THROW_IF(pos < 0 || pos >= sizeof(Integral) * 8,
              std::range_error,
              "Invalid bit position [pos = " << pos << "]");

  if (number & (1 << pos)) return false;
  return true;
}

} // namespace datatools

#endif // DATATOOLS_UTILS_H_

