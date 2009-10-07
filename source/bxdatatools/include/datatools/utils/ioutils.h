// ioutils.h

#ifndef __datatools__utils__ioutils_h
#define __datatools__utils__ioutils_h 1

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace datatools {
  
  namespace utils {

    ostream & debug (ostream &);
    
    ostream & notice (ostream &);

    ostream & warning (ostream &);

    ostream & error (ostream &);

  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__ioutils_h

// end of ioutils.h
