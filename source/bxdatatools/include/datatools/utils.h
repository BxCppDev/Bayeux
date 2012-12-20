// -*- mode: c++; -*-
// utils.h

#ifndef DATATOOLS_UTILS_H_
#define DATATOOLS_UTILS_H_

// Standard Library
#include <cstdlib>
#include <list>
#include <stdexcept>
#include <string>

// Third Party
// - A

// This Project

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

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the 'getenv' function.
 */
bool fetch_path_with_env(std::string& word);

/** Extract the expanded path computed from the 'word' string.
 * Internally uses the system's shell.
 *
 * This method is not very elegant. I use
 * brute force, asking a shell to interpret the string via a 'system' call
 * and using a temporary file to save the result. Urrkkk!
 * That enables the expansion of environment variables embeded
 * in the 'path_str' string:
 *
 * Example: '$HOME/foo.dat' is expanded to '/home/&gt;login&lt;/foo.dat'
 *
 */
std::string expand_path(const std::string& path_str);

/** The function splits the string 'a_word' using separators given
 *  within a string 'a_separators' and provides the list of tokens
 *  'a_words'.
 *
 * The Boost library provides some powerful alternative through
 * the tokenizer class.
 *
 */
void split_string(const std::string& word, const std::string& separators,
                  std::list<std::string>& words);


template<typename Integral>
void set_bit(Integral& number, int pos) {
  if (pos < 0 || pos >= sizeof(Integral) * 8) {
    throw std::range_error("datatools::set_bit: Invalid bit position !");
  }
  number |= 1 << pos;
}


template<typename Integral>
void unset_bit(Integral& number, int pos) {
  if (pos < 0 || pos >= sizeof(Integral) * 8) {
    throw std::range_error("datatools::unset_bit: Invalid bit position !");
  }
  number &= ~(1 << pos);
}


template<typename Integral>
void toggle_bit(Integral& number, int pos) {
  if (pos < 0 || pos >= sizeof(Integral) * 8) {
    throw std::range_error("datatools::toggle_bit: Invalid bit position !");
  }
  number ^= 1 << pos;
}


template<typename Integral>
bool check_bit(Integral& number, int pos) {
  if (pos < 0 || pos >= sizeof(Integral) * 8) {
    throw std::range_error("datatools::toggle_bit: Invalid bit position !");
  }
  if (number & (1 << pos)) return false;
  return true;
}

} // namespace datatools

#endif // DATATOOLS_UTILS_H_

