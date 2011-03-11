// -*- mode: c++; -*-
// utils.h

#ifndef __datatools__utils__utils_h
#define __datatools__utils__utils_h 1

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <list>
#include <limits>

namespace datatools {

  namespace utils {

    // Invalidate a double float (using NaN):
    void invalidate (double & x_);

    // Check if a double float is valid (not a NaN):
    bool is_valid (double x_);

    // Set a double to +infinity:
    void plus_infinity (double & x_);

    // Set a double to -infinity:
    void minus_infinity (double & x_);

    // Set a double to +infinity:
    void infinity (double & x_);

    /** Extract the expanded path computed from the 'text_' string.
     * Internally uses the 'getenv' function.
     */
    bool fetch_path_with_env (std::string & text_);

    /* This method is not very elegant. I use
     * brute force, asking a shell to interpret the string via a 'system' call
     * and using a temporary file to save the result. Urrkkk!
     * That enables the expansion of environment variables embeded
     * in the 'path_str_' string:
     *
     * Example: '$HOME/foo.dat' is expanded to '/home/<login>/foo.dat'
     *
     */
    std::string expand_path (const std::string & path_str_);

    /** The function splits the string 'text_' using separators given
     *  within a string 'separators_' and provides the list of tokens
     *  'words_'.
     *
     * The Boost library provides some powerful alternative through
     * the tokenizer class stuff.
     *
     */
    void split_string (const std::string & text_ ,
		       const std::string & separators_ ,
		       std::list<std::string> & words_);

  } // namespace utils

} // namespace datatools

#endif // __datatools__utils__utils_h

// end of utils.h
