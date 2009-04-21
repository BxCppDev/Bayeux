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

namespace datatools {
  
  namespace utils {
    
    bool fetch_path_with_env (std::string & text_);
    
    /* This method is not very elegant. I use 
     * brute force, asking a shell to interpret the string via a 'system' call
     * and using a temporary file to save the result. Urrkkk!
     * That enables the expansion of environment variables embeded
     * in the 'path_str_' string:
     *
     * Example: '$HOME/foo.dat' is expanded to '/home/mauger/foo.dat'
     *
     */
    std::string expand_path (const std::string & path_str_);
    
    void split_string (const std::string & text_ ,
		       const std::string & separators_ ,
		       std::list<std::string> & words_);
    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__utils_h

// end of utils.h
