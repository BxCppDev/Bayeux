// -*- mode:c++; -*- 
// mygsl::datapoint.h

#ifndef __mygsl__datapoint_h 
#define __mygsl__datapoint_h 1

#include <iostream>
#include <limits>

#include <mygsl/ioutils.h>

//using namespace   std;

namespace mygsl {

  class datapoint
  {

  public:

    datapoint ();

    datapoint (double x_, 
               double y_, 
               double sigma_ = std::numeric_limits<double>::quiet_NaN ());

    const double & x () const;

    const double & y () const;

    const double & sigma () const;

    bool has_sigma () const;

    bool is_weighted () const;

    friend std::ostream & operator<< (std::ostream &, const datapoint &);

    friend std::istream & operator>> (std::istream &, datapoint &);

    static bool comp_by_x (const datapoint & p1_, 
                           const datapoint & p2_);

  private:

    double _x_;
    double _y_;
    double _sigma_;

  };

}

#endif // __mygsl__datapoint_h

// end of mygsl::datapoint.h
