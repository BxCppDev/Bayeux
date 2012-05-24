// -*- mode: c++; -*-
// utils.h

#ifndef __datatools__utils__utils_h
#define __datatools__utils__utils_h 1

#include <cstdlib>
#include <string>
#include <list>
#include <stdexcept>

namespace datatools {

  namespace utils {

    enum return_code_type
      {
        SUCCESS = EXIT_SUCCESS,
        ERROR   = EXIT_FAILURE,
        FAILURE   = EXIT_FAILURE
      };

    // Invalidate a float (using NaN):
    void invalidate (float & a_value);

    // Check if a float is valid (not a NaN):
    bool is_valid (float a_value);

    // Set a float to +infinity:
    void plus_infinity (float & a_value);

    // Set a float to -infinity:
    void minus_infinity (float & a_value);

    // Set a float to +infinity:
    void infinity (float & a_value);

    // Invalidate a double (using NaN):
    void invalidate (double & a_value);

    // Check if a double is valid (not a NaN):
    bool is_valid (double a_value);

    // Set a double to +infinity:
    void plus_infinity (double & a_value);

    // Set a double to -infinity:
    void minus_infinity (double & a_value);

    // Set a double to +infinity:
    void infinity (double & a_value);

    /** Extract the expanded path computed from the 'a_word' string.
     * Internally uses the 'getenv' function.
     */
    bool fetch_path_with_env (std::string & a_word);

    /** Extract the expanded path computed from the 'a_word' string.
     * Internally uses the system's shell.
     *
     * This method is not very elegant. I use
     * brute force, asking a shell to interpret the string via a 'system' call
     * and using a temporary file to save the result. Urrkkk!
     * That enables the expansion of environment variables embeded
     * in the 'a_path_str' string:
     *
     * Example: '$HOME/foo.dat' is expanded to '/home/<login>/foo.dat'
     *
     */
    std::string expand_path (const std::string & a_path_str);

    /** The function splits the string 'a_word' using separators given
     *  within a string 'a_separators' and provides the list of tokens
     *  'a_words'.
     *
     * The Boost library provides some powerful alternative through
     * the tokenizer class.
     *
     */
    void split_string (const std::string & a_word ,
                       const std::string & a_separators ,
                       std::list<std::string> & a_words);

    template<typename Integral>
    void set_bit (Integral & number_, int pos_)
    {
      if (pos_ < 0 || pos_ >= sizeof (Integral) * 8)
        {
          throw std::range_error ("datatools::utils::set_bit: Invalid bit position !");
        }
      number_ |= 1 << pos_;
      return;
    }

    template<typename Integral>
    void unset_bit (Integral & number_, int pos_)
    {
      if (pos_ < 0 || pos_ >= sizeof (Integral) * 8)
        {
          throw std::range_error ("datatools::utils::unset_bit: Invalid bit position !");
        }
      number_ &= ~(1 << pos_);
      return;
    }

    template<typename Integral>
    void toggle_bit (Integral & number_, int pos_)
    {
      if (pos_ < 0 || pos_ >= sizeof (Integral) * 8)
        {
          throw std::range_error ("datatools::utils::toggle_bit: Invalid bit position !");
        }
      number_ ^= 1 << pos_;
      return;
    }

    template<typename Integral>
    bool check_bit (Integral & number_, int pos_)
    {
     if (pos_ < 0 || pos_ >= sizeof (Integral) * 8)
        {
          throw std::range_error ("datatools::utils::toggle_bit: Invalid bit position !");
        }
      if (number_ & (1 << pos_)) return false;
      return true;
    }

  } // namespace utils

} // namespace datatools

#endif // __datatools__utils__utils_h

// end of utils.h
