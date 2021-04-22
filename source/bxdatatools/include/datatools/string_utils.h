/// \file datatools/string_utils.h
/// \brief utilities for manipulating and validating strings

#ifndef DATATOOLS_STRING_UTILS_H
#define DATATOOLS_STRING_UTILS_H

// Standard Library
#include <cstdint>
#include <list>
#include <string>

// This project
#include <datatools/bit_mask.h>

namespace datatools {
  /* String constants */

  const std::string & empty_string();

  const std::string & none_label();

  const std::string & yes_label();

  const std::string & no_label();

  /* String utility functions */

  enum name_validation_flags {
    NV_NO_COLON      = bit_mask::bit00, ///< Forbid the 'colon' character in names
    NV_NO_DOT        = bit_mask::bit01, ///< Forbid the 'dot' character in names
    NV_NO_HYPHEN     = bit_mask::bit02, ///< Forbid the 'hyphen' character in names
    NV_NO_UNDERSCORE = bit_mask::bit03, ///< Forbid the 'underscore' character in names
    NV_LEADING_DIGIT = bit_mask::bit04, ///< Allow leading digit
    NV_DEFAULT       = NV_NO_HYPHEN ,   ///< Default validation rule
    NV_INSTANCE      = NV_NO_HYPHEN | NV_NO_COLON, ///< Default validation rule for object instance
    NV_MODEL         = NV_NO_HYPHEN,   ///< Default validation rule for object model
    NV_NO_LOWER_CASE = bit_mask::bit05, ///< Forbid lower case characters
    NV_NO_UPPER_CASE = bit_mask::bit06  ///< Forbid upper case characters
  };

  /// Check if a name (object identifier) is valid using simple criteria
  bool name_validation(const std::string & name_, uint32_t flags_ = NV_DEFAULT);

  /// Check if a string is quoted
  /// @arg text_ the string to check
  /// @arg q_ the quoting character (may be double quote or single quote)
  /// @return true if the string is quoted with a pair of quoting characters
  bool is_quoted(const std::string & text_, char q_ = '"');

  /// Add quotes to a string
  void add_quotes(const std::string & from_, std::string & to_, char q_ = '"');

  /// Add quotes to a string
  void add_quotes(std::string & text_, char q_ = '"');

  /// Remove quotes from a string
  void remove_quotes(const std::string & from_, std::string & to_, char q_ = '"');

  /// Remove quotes from a string
  void remove_quotes(std::string & text_, char q_ = '"');

  /// Remove all quotes from a string
  void remove_all_quotes(std::string & text_);

} // namespace datatools

#endif // DATATOOLS_STRING_UTILS_H

