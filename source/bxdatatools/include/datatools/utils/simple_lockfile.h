// -*- mode: c++; -*- 
/* simple_lockfile.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-02-25
 * Last modified: 2007-02-25
 *
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __datatools__utils__simple_lockfile_h
#define __datatools__utils__simple_lockfile_h 1

// standard includes:
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

/*
extern "C"
{
#include <lockfile.h>
#include <errno.h>
#include <string.h>
}

namespace datatools {

  namespace utils {
    
    class simple_lockfile
    {
      std::string __filename;
      int         __retry_count;
      int         __flags;
      bool        __created;

    public:
      bool verbose;

    public:
      simple_lockfile( const std::string & filename_ , 
		       int retry_count_ = 0 , 
		       bool pid_ = true , 
		       bool ppid_ = false );
      virtual ~simple_lockfile();
      void 
      create();

      void 
      remove();

      bool 
      check();

      void 
      touch();
    };
    
  } // end of namespace utils

} // end of namespace datatools
*/

#endif // __datatools__utils__simple_lockfile_h

// end of simple_lockfile.h
