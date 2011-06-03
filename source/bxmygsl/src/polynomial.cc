// mygsl::polynomial.cc

#include <mygsl/polynomial.h>

#include <stdexcept>
#include <sstream>

namespace mygsl {

  size_t polynomial::get_ncoeffs () const
  {
    return __c.size ();
  }
  
  size_t polynomial::get_degree () const
  {
    return get_ncoeffs () - 1;
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
    size_t sz = __c.size ();
    const double * first_arg = 0;
    if (sz > 0)
      {
	first_arg = &(__c[0]);
      }
    return gsl_poly_eval (first_arg, sz, x_);
  }

  void polynomial::print (ostream & out_, int format_, bool eol_) const
  {
    string prod_sym = "×";
    string exp_sym  = "^";
	prod_sym = "*";
	exp_sym  = "**";
    if (format_ == 1)
      {
	prod_sym = "×";
	exp_sym  = "^";
      }
    bool first = true;
    for (int deg = 0; deg <= get_degree (); deg++)
      {
	if (__c[deg] == 0.0)
	  {
	    continue;
	  }
	else
	  {
	    if (! first)
	      {
		out_ << (__c[deg] > 0? " + ": " - ");
	      }
	    else
	      {
		out_ << (__c[deg] > 0? "": " - ");
		first = false;	    
	      }
	    out_.unsetf (ios::showpos);
	    out_ << abs(__c[deg]);
	    if (deg >= 1)
	      {
		out_ << ' ' << prod_sym << ' ' << 'X';
	      } 
	    if (deg >= 2)
	      {
		out_ << exp_sym << deg;
	      } 
	  }
      }
    if (eol_) out_ << endl;
  }

  bool polynomial::solve_linear (double p0_, 
				 double p1_, 
				 size_t & nsols_,
				 double & x0_)
  {
    if (p1_ == 0.0)
      {
	if (p0_ == 0.0)
	  {
	    nsols_ = -1;
	    return false;
	  }
	else
	  {
	    nsols_ = 0;
	    return false;
	  }
	x0_ = GSL_NAN;
      }
    x0_ = -p0_ / p1_;
    nsols_ = 1;
    return true;
  }

  bool polynomial::solve_quadratic (double p0_, 
				    double p1_, 
				    double p2_, 
				    size_t & nsols_,
				    double & x0_, double &  x1_)
  {
    if (p2_ == 0)
      {
	x1_ = GSL_NAN;
	return solve_linear (p0_, p1_, nsols_, x0_);	
      }
    nsols_ = (size_t) gsl_poly_solve_quadratic (p2_, p1_, p0_, &x0_, &x1_);
    if (nsols_ < 1)
      {
	x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
	x1_ = GSL_NAN;
      }
    return nsols_ > 0;
  } 

  bool polynomial::solve_cubic (double p0_, 
				double p1_, 
				double p2_, 
				size_t & nsols_,
				double & x0_, 
				double & x1_, 
				double & x2_)
  {
    nsols_ = (size_t) gsl_poly_solve_cubic (p2_, p1_, p0_, &x0_, &x1_, &x2_);
    if (nsols_ < 1)
      {
	x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
	x1_ = GSL_NAN;
      }
    if (nsols_ < 3)
      {
	x2_ = GSL_NAN;
      }
    return nsols_ > 0;
  }

  bool polynomial::solve_cubic (double p0_, 
				double p1_, 
				double p2_,
				double p3_,
				size_t & nsols_,
				double & x0_, 
				double & x1_, 
				double & x2_)
  {
    if (p3_ == 0)
      {
	x2_ = GSL_NAN;	
	return solve_quadratic (p0_, p1_, p2_, nsols_, x0_, x1_);
      }
    nsols_ = (size_t) solve_cubic (p2_ / p3_, p1_ / p3_, p0_ / p3_, nsols_, x0_, x1_, x2_);
    if (nsols_ < 1)
      {
	x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
	x1_ = GSL_NAN;
      }
    if (nsols_ < 3)
      {
	x2_ = GSL_NAN;
      }
    return nsols_ > 0;
  }

  /**************************************/

  void polynomial::solver::__init (size_t sz_)
  {
    __status = GSL_EFAILED;
    if (__ws != 0)
      {
	if (__sz >= sz_)
	  {
	    return;
	  }
	else
	  {
	    __reset ();
	  }
      }
    if (sz_ > 1)
      {
	__ws = gsl_poly_complex_workspace_alloc (sz_);
	__sz = sz_;
      }
  }

  void polynomial::solver::__reset ()
  {
    if (__ws != 0)
      {
	gsl_poly_complex_workspace_free (__ws);
	__ws = 0;
	__sz = 0;
      }
    __status = GSL_EFAILED;
  }

  polynomial::solver::solver (size_t sz_)
  {
    __status = GSL_EFAILED;
    __sz = 0;
    __ws = 0;
    __init (sz_);
  }

  polynomial::solver::~solver ()
  {
    __reset ();
  }

  bool polynomial::solver::solve (const polynomial & p_)
  {
    size_t deg = p_.get_degree ();
    __init (deg + 1);
    {
      throw runtime_error ("polynomial::solver::solve:  Not implemented yet!");
    }
    return __status == GSL_SUCCESS;
  }

}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::polynomial.cc
