// mygsl::datapoint.h

#ifndef __mygsl__datapoint_h 
#define __mygsl__datapoint_h 1

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

#include <mygsl/ioutils.h>

using namespace std;

namespace mygsl {

  class datapoint
    {
    private:

      double __x, __y, __sigma;

    public:

      datapoint ();

      datapoint (double x_, 
		 double y_, 
		 double sigma_ = numeric_limits<double>::quiet_NaN ());

      const double & x () const;

      const double & y () const;

      const double & sigma () const;

      bool has_sigma () const;

      bool is_weighted () const;

      friend ostream & operator<< (ostream &, const datapoint &);

      friend istream & operator>> (istream &, datapoint &);

      static bool comp_by_x (const datapoint & p1_, 
			     const datapoint & p2_);

    };

}

#endif // __mygsl__datapoint_h

// end of mygsl::datapoint.h
