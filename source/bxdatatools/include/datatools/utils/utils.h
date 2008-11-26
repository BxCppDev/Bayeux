// utils.h

#ifndef __datatools__utils__utils_h
#define __datatools__utils__utils_h 1

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <list>

namespace datatools {

  namespace utils {

    bool 
      fetch_path_with_env(std::string & text_);
    
    void 
      split_string(const std::string & text_ ,
		   const std::string & separators_ ,
		   std::list<std::string> & words_);

  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__utils_h

// end of utils.h
