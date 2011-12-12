// -*- mode: c++; -*- 
/* simple_lockfile.cc
 */

#include <datatools/utils/simple_lockfile.h>

/*

namespace datatools {

  namespace utils {

    simple_lockfile::simple_lockfile(const std::string & filename_ , 
				     int retry_count_ , 
				     bool pid_ , 
				     bool ppid_ )
    {
      verbose       = false;
      __filename    = filename_;
      __retry_count = retry_count_;
      __flags       = 0;
      __created     = false;
      if (pid_) __flags  |= L_PID;
      if (ppid_) __flags |= L_PPID;
    }

    simple_lockfile::~simple_lockfile()
    {
      if (__created && this->check()) remove();
    }

    void 
    simple_lockfile::create() 
    {
      int lc_ret = ::lockfile_create(__filename.c_str(), 
				     __retry_count, 
				     __flags);
    
      if (lc_ret != L_SUCCESS) 
	{
	  std::string msg;
	  switch (lc_ret)
	    {
	    case L_NAMELEN: msg = "Recipient name too long"; break;
	    case L_TMPLOCK: msg = "Error creating tmp lockfile"; break;
	    case L_TMPWRITE: msg = "Can't write pid int tmp lockfile"; break;
	    case L_MAXTRYS: msg = "Failed after max. number of attempts"; break;
	    case L_ERROR: ;
	    default:
	      std::ostringstream message2;
	      const size_t MYBUFSZ = 1024;
	      char buf[MYBUFSZ];
	      buf[0] = 0;
	      int myerrno = errno;
	      strerror_r(myerrno, buf, MYBUFSZ);
	      message2 << "Unknown error;errno message is '" 
		       << buf << "'  (errno=" << myerrno << ")";
	      msg = message2.str();
	      break;
	    }
	  std::ostringstream message;
	  message << "simple_lockfile::create: Cannot create lockfile '" 
		  <<  __filename << "' with error code '" 
		  << lc_ret << "': '" << msg << "'";
	  throw std::runtime_error(message.str());
	}
      else 
	{
	  __created = true;
	  if (verbose) std::clog << "simple_lockfile::create: Created lockfile '" <<  __filename << "'" << std::endl;
	} 
    }

    bool 
    simple_lockfile::check()
    {
      return (::lockfile_check(__filename.c_str(), __flags ) == 0);
    }

    void 
    simple_lockfile::remove()
    {
      if (__created && ::lockfile_remove( __filename.c_str() ) != 0) 
	{
	  std::ostringstream message;
	  const size_t MYBUFSZ=1024;
	  char buf[MYBUFSZ];
	  buf[0] = 0;
	  int myerrno = errno;
	  strerror_r(myerrno, buf, MYBUFSZ);
	  message << "simple_lockfile::remove: Cannot remove lockfile '" 
		  <<  __filename << "': errno message is '" 
		  << buf << "'  (errno=" << myerrno << ")";
	  throw std::runtime_error(message.str());
	}
      else 
	{
	  __created = false;
	  if (verbose) std::clog << "simple_lockfile::remove: Removed lockfile '" <<  __filename << "'" << std::endl;
	}
    }

    void 
    simple_lockfile::touch()
    {
      if (! __created) return;
      if (::lockfile_touch(__filename.c_str() ) != 0) 
	{
	  std::ostringstream message;
	  const size_t MYBUFSZ=1024;
	  char buf[MYBUFSZ];
	  buf[0] = 0;
	  int myerrno = errno;
	  strerror_r(myerrno, buf, MYBUFSZ);
	  message << "simple_lockfile::touch: Cannot touch lockfile '" 
		  <<  __filename << "': errno message is '" 
		  << buf << "'  (errno=" << myerrno << ")";
	  if (verbose) std::clog << "simple_lockfile::touch: Touch lockfile '" 
				 <<  __filename << "' failed with error message: " 
				 << message.str() << std::endl;      
	  throw std::runtime_error(message.str());
	}
      else 
	{
	  if (verbose) std::clog << "simple_lockfile::touch: Touched lockfile '" <<  __filename << "'" << std::endl;
	}
    }
    
  } // end of namespace utils

} // end of namespace datatools

*/

// end of simple_lockfile.cc
