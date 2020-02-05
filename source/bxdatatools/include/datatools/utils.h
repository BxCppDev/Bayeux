/// \file datatools/utils.h

#ifndef DATATOOLS_UTILS_H
#define DATATOOLS_UTILS_H

// Standard Library:
#include <cstdlib>
#include <list>
#include <string>

// Third party:
// - Boost:
// #include <boost/property_tree/ptree.hpp>

// This Project:
#include <datatools/exception.h>
#include <datatools/bit_mask.h>

namespace datatools {

  /// \brief Return codes
  enum return_code_type {
    SUCCESS = EXIT_SUCCESS, //!< Returned code in case of success
    ERROR   = EXIT_FAILURE, //!< Returned code in case of failure
    FAILURE = EXIT_FAILURE  //!< Returned code in case of failure
  };

  /// \brief Comparison result code for comparable objects
  enum compare_result_type {
    COMPARE_UNDEFINED = -2, //!< Comparison cannot be performed
    COMPARE_LESS      = -1, //!< Less than
    COMPARE_EQUAL     =  0, //!< Equal to
    COMPARE_GREATER   =  1  //!< Greater than
  };

  /* String constants */

  const std::string & empty_string();

  const std::string & none_label();

  const std::string & yes_label();

  const std::string & no_label();
  
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

  /* Functions to expand string as path */

  /// Extract the keys used by library information registration mechanism
  bool resolve_library_info_path_keys(const std::string & library_topic_,
                                      std::string & install_path_key_,
                                      std::string & environ_path_key_);

  /// Fetch the expanded path computed from a string.
  /// @arg path_str the string representing the path to be processed
  /// @return the processed string with resolved environment variable and registered library path
  std::string fetch_path(const std::string & path_str_);

  //! Extract the expanded path computed from the 'word' string.
  bool fetch_path(std::string & word_, std::string & errmsg_);

  //! Extract the expanded path computed from the 'word' string.
  //! Internally uses the getenv function and the datatools::library_info
  //! register.
  /// @arg word the string path to be processed
  /// @return true if the processing was successful (successfully resolved environ variables and
  ///         registered library path)
  ///
  /// Supported formats:
  ///
  /// * Plain path:
  ///
  ///  "/tmp/foo/bar/baz.txt"
  ///  "./foo/bar/baz.txt"
  ///
  /// * Path with special tokens and/or environment variables::
  ///
  ///  "~/foo/bar/baz.txt"
  ///  "~foo/bar/baz.txt"
  ///  "${HOME}/foo/bar/baz.txt"
  ///  "${ENV1}/foo/bar/baz.txt"
  ///  "${ENV1}/foo/${ENV2}/baz.txt"
  ///
  /// * Path from registered/mount point:
  ///
  ///  "@foo:bar/baz.txt"
  ///  "@foo.resources:config/simulation/1.0/manager.conf"
  ///  "@foo.data:dataset/march2017/run_1.data"
  ///
  /// * URN resolved as path:
  ///
  ///   - plain URN:
  ///
  ///     "urn:snemo:analysis:setup:1.0"
  ///
  ///   - URN with explicit category:
  ///
  ///     "urn:snemo:analysis:setup:1.0@configuration"
  ///     "urn:snemo:analysis:logo:small@image"
  ///     "urn:snemo:analysis:run:1234@data"
  ///
  bool fetch_path_with_env(std::string & word_);

  //! Extract the expanded path computed from the 'word' string.
  bool fetch_path_with_env(std::string & word_, std::string & errmsg_);

  /** Extract the expanded path computed from a string.
   * Internally uses the datatools::library_info
   * register.
   * @arg word the string path to be processed
   * @return true if the processing was successful (successfully resolved
   *         registered library path)
   */
  bool fetch_path_without_env(std::string & word_);

  /** Extract the expanded path computed from a string.
   *  Internally uses the getenv function and the datatools::library_info
   *  register.
   */
  bool fetch_path_with_env_p(std::string & word_,
                             const std::string & parent_path_);

  /** Extract the expanded path computed from a string.
   * Internally uses the getenv function and the datatools::library_info
   * register.
   */
  bool fetch_path_with_env_g(std::string & word_);

  /** Extract the expanded path computed from a string.
   * Internally uses the getenv function and the datatools::library_info
   * register.
   */
  bool fetch_path_with_env_pg(std::string & word_,
                              const std::string & parent_path_);

  /** Check if datatools has its global path activated */
  bool has_global_path();

  /** Reset the datatools global path */
  void reset_global_path();

  /** Set/activate the datatools global path */
  void set_global_path(const std::string & gpath_);

  /** Get the datatools global path */
  const std::string & get_global_path();

  //! Extract the expanded path computed from a string.
  //! Internally uses the system's shell.
  //!
  //! This method is not very elegant. It uses
  //! brute force, asking a shell to interpret the string via a 'system' call
  //! and using a temporary file to save the result. Urrkkk!
  //! That enables the expansion of environment variables embeded
  //! in the 'path_str' string:
  //!
  //! Example: '$HOME/foo.dat' is expanded to '/home/&gt;login&lt;/foo.dat'
  //! @deprecated Use the fetch_path or fetch_path_with_env functions.
  std::string expand_path(const std::string & path_str_);

  /** The function splits a string using separators
   *   and provides a list of tokens.
   *
   * The Boost library provides some powerful alternative through
   * the tokenizer class and split string algorithms.
   * @deprecated Use the Boost string split function or tokenizer class
   */
  void split_string(const std::string & word_,
                    const std::string & separators_,
                    std::list<std::string> & words_);

  /* Bit manipulation utility template functions */

  /// Set the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void set_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ |= 1 << pos_;
    return;
  }

  /// Unset the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void unset_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ &= ~(1 << pos_);
    return;
  }


  /// Toggle the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void toggle_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ ^= 1 << pos_;
    return;
  }


  /// Check the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  bool check_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");

    if (number_ & (1 << pos_)) return false;
    return true;
  }

} // namespace datatools

#endif // DATATOOLS_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
