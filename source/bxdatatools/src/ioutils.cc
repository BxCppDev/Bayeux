// -*- mode: c++; -*- 
// ioutils.cc

#include <datatools/utils/ioutils.h>

using namespace std;

namespace datatools {

  namespace utils {

    ostream & debug (ostream & out_)
      {
	out_ << "DEBUG: ";
	return out_;
      }

    ostream & notice (ostream & out_)
      {
	out_ << "NOTICE: ";
	return out_;
      }

    ostream & warning (ostream & out_)
      {
	out_ << "WARNING: ";
	return out_;
      }

    ostream & error (ostream & out_)
      {
	out_ << "ERROR: ";
	return out_;
      }


  } // namespace utils

} // namespace datatools 

// end of ioutils.cc
