// -*- mode:c++; -*- 
// mygsl::datapoint.cc

#include <mygsl/datapoint.h>

#include <string>
#include <sstream>
#include <stdexcept>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

namespace mygsl {
  
  using namespace std;
  
  datapoint::datapoint ()
  {
    _x_ = 0.0;
    _y_ = 0.0;
    _sigma_ = numeric_limits<double>::quiet_NaN ();
    return;
  }

  datapoint::datapoint (double x_, 
                        double y_, 
                        double sigma_)
  {
    _x_ = x_;
    _y_ = y_;
    _sigma_ = sigma_;
    return;
  }
    
  const double & datapoint::x () const
  {
    return _x_;
  }
    
  const double & datapoint::y () const
  {
    return _y_;
  }
    
  bool datapoint::has_sigma () const
  {
    return ! isnan (_sigma_);
  }

  bool datapoint::is_weighted () const
  {
    return has_sigma ();
  }

  const double & datapoint::sigma () const
  {
    return _sigma_;
  }

  ostream & operator<< (ostream & out_, const datapoint & p_)
  {
    out_ << odouble (p_._x_ ) << ' ' 
         << odouble (p_._y_ ) << ' ' 
         << odouble (p_._sigma_); 
    return out_; 
  }

  istream & operator>> (istream & in_ , datapoint & p_)
  {
    in_ >> idouble (p_._x_ ) >> ws 
        >> idouble (p_._y_ ) >> ws 
        >> idouble (p_._sigma_); 
    return in_;
  }

  bool datapoint::comp_by_x (const datapoint & p1_, 
                             const datapoint & p2_)

  { 
    return p1_._x_ < p2_._x_;
  }

}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::datapoint.cc
