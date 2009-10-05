// mygsl::datapoint.cc

#include <mygsl/datapoint.h>

namespace mygsl {
  
  datapoint::datapoint ()
  {
    __x = 0.0;
    __y = 0.0;
    __sigma = numeric_limits<double>::quiet_NaN ();
  }

  datapoint::datapoint (double x_, 
			double y_, 
			double sigma_)
  {
    __x = x_;
    __y = y_;
    __sigma = sigma_;
  }
    
  const double & datapoint::x () const
  {
    return __x;
  }
    
  const double & datapoint::y () const
  {
    return __y;
  }
    
  bool datapoint::has_sigma () const
  {
    return ! isnan (__sigma);
  }

  bool datapoint::is_weighted () const
  {
    return has_sigma ();
  }

  const double & datapoint::sigma () const
  {
    return __sigma;
  }

  ostream & operator<< (ostream & out_, const datapoint & p_)
  {
    out_ << odouble (p_.__x ) << ' ' 
	 << odouble (p_.__y ) << ' ' 
	 << odouble (p_.__sigma); 
    return out_;
  }

  istream & operator>> (istream & in_ , datapoint & p_)
  {
    in_ >> idouble (p_.__x ) >> ws 
	>> idouble (p_.__y ) >> ws 
	>> idouble (p_.__sigma); 
    return in_;
  }

  bool datapoint::comp_by_x (const datapoint & p1_, 
			     const datapoint & p2_)

  { 
    return p1_.__x < p2_.__x;
  }

}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::datapoint.cc
