// \file mygsl/datapoint.cc

// Ourselves:
#include <mygsl/datapoint.h>

// Standard library:
#include <string>
#include <sstream>
#include <stdexcept>

// Third party:
// - GSL:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

namespace mygsl {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(datapoint,"mygsl::datapoint")

  datapoint::datapoint ()
  {
    _x_ = 0.0;
    _y_ = 0.0;
    _sigma_y_ = numeric_limits<double>::quiet_NaN ();
    return;
  }

  datapoint::~datapoint ()
  {
    return;
  }

  datapoint::datapoint (double x_,
                        double y_,
                        double sigma_y_)
  {
    _x_ = x_;
    _y_ = y_;
    _sigma_y_ = sigma_y_;
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

  bool datapoint::has_sigma_y () const
  {
    return ! isnan (_sigma_y_);
  }

  bool datapoint::is_weighted () const
  {
    return has_sigma_y ();
  }

  const double & datapoint::sigma_y () const
  {
    return _sigma_y_;
  }

  ostream & operator<< (ostream & out_, const datapoint & p_)
  {
    out_ << odouble (p_._x_ ) << ' '
         << odouble (p_._y_ ) << ' '
         << odouble (p_._sigma_y_);
    return out_;
  }

  istream & operator>> (istream & in_ , datapoint & p_)
  {
    in_ >> idouble (p_._x_ ) >> ws
        >> idouble (p_._y_ ) >> ws
        >> idouble (p_._sigma_y_);
    return in_;
  }

  bool datapoint::comp_by_x (const datapoint & p1_,
                             const datapoint & p2_)
  {
    return p1_._x_ < p2_._x_;
  }

} // namespace mygsl

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl/datapoint.cc
