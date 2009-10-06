// mygsl::polynomial.cc

#include <mygsl/polynomial.h>

namespace mygsl {

  size_t polynomial::get_degree () const
  {
    return __c.size () - 1;
  }
  
  double polynomial::get_coeff (size_t i_) const
  {
    return __c.at (i_);
  }
  
  void polynomial::set_coeff (size_t i_, double c_)
  {
    if (i_ > get_degree ())
      {
	size_t sz = __c.size ();
	__c.resize (i_ + 1);
	for (int i = sz; i < __c.size (); i++)
	  {
	    __c[i] = 0.0;
	  }
      }
    __c.at (i_) = c_;
  }
  
  polynomial::polynomial (size_t degree_)
  {
    __c.assign (degree_ + 1, 0.0);
    __c [degree_] = 1.0;
  }
  
  polynomial::polynomial (double c0_)
  {
    __c.assign (1, c0_);
  }
  
  polynomial::polynomial (double c0_, double c1_)
  {
    __c.assign (2, 0.0);
    __c [0] = c0_;
    __c [1] = c1_;
  }
  
  polynomial::polynomial (double c0_, double c1_, double c2_)
  {
    __c.assign (3, 0.0);
    __c [0] = c0_;
    __c [1] = c1_;
    __c [2] = c2_;
  }
  
  polynomial::polynomial (const vector<double> & c_)
  {
    __c = c_;
  }
  
  polynomial::polynomial (const polynomial & p_)
  {
    __c = p_.__c;
  }
  
  polynomial::~polynomial ()
  {
    __c.clear ();
  }
  
  double polynomial::eval (double x_) const
  {
    return gsl_poly_eval (__c.data (), __c.size (),x_);
  }

  void polynomial::print (ostream & out_) const
  {
    bool first = true;
    for (int deg = 0; deg <= get_degree (); deg++)
      {
	if (__c[deg] == 0.0)
	  {
	    continue;
	  }
	if (! first) out_.setf (ios::showpos);
	if (__c[deg] != 0.0)
	  {
	    first = false;
	  }
	out_ << __c[deg];
	out_.unsetf (ios::showpos);
	if (deg >= 1)
	  {
	    out_ << "*x";
	  } 
	if (deg >= 2)
	  {
	    out_ << "**" << deg;
	  } 
      }
  }

}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::polynomial.cc
