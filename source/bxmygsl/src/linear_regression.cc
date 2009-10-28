// mygsl::linear_regression.cc

#include <mygsl/linear_regression.h>

namespace mygsl {

  linear_regression::fit_data::fit_data ()
  {
    reset ();
  }

  void linear_regression::fit_data::reset ()
  {
    status = GSL_CONTINUE;
    weighted = false;
    with_constant = false;
    n = 0;
    sumsq = -1.0;
    chisq = -1.0;
  }

  bool linear_regression::fit_data::is_valid () const
  {
    return status == GSL_SUCCESS;
  }

  bool linear_regression::fit_data::is_weighted () const
  {
    return weighted;
  }

  bool linear_regression::fit_data::has_constant () const
  {
    return with_constant;
  }

  double linear_regression::fit_data::get_constant () const
  {
    return c0;
  }

  double linear_regression::fit_data::get_slope () const
  {
    return c1;
  }
	
  size_t linear_regression::fit_data::get_ndof () const
  {
    return n - 2;
  }

  double linear_regression::fit_data::get_chisq () const
  {
    if (! is_weighted ()) 
      {
	throw runtime_error ("linear_regression::fit_data::get_sumsq: Not computed! Use 'get_sumsq'!");
      }
    return chisq;
  }

  double linear_regression::fit_data::get_sumsq () const
  {
    if (is_weighted ()) 
      {
	throw runtime_error ("linear_regression::fit_data::get_sumsq: Not computed! Use 'get_chisq'!");
      }
    return sumsq;
  }

  double linear_regression::fit_data::get_slope_err () const
  {
    return sqrt (cov11);
  }

  double linear_regression::fit_data::get_constant_err () const
  {
    return sqrt (cov00);
  }

  /************************/

  linear_regression::linear_regression ()
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
  }

  linear_regression::linear_regression (const vector<datapoint> & p_)
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
    init (p_);
  }

  linear_regression::linear_regression (size_t npoints_, 
					const double * x_, 
					const double * y_, 
					const double * w_)
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
    init (npoints_, x_, y_, w_);
  }

  linear_regression::linear_regression (const vector<double> & x_, 
					const vector<double> & y_, 
					const vector<double> & w_)
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
    init (x_, y_, w_);
  }

  linear_regression::linear_regression (const vector<double> & x_, 
					const vector<double> & y_)
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
    init (x_, y_);
  }

  linear_regression::~linear_regression ()
  {
    reset ();
  }

  bool linear_regression::is_initialized () const
  {
    return __initialized;
  }
  
  bool linear_regression::can_weighted () const
  {
    return __can_weighted;
  }

  void linear_regression::init (const vector<datapoint> & p_)
  {
    if (p_.size () < 3)
      {
	ostringstream message;
	message << "linear_regression::init: Not enough datapoints!";
	throw runtime_error (message.str ());
      }
    size_t n = p_.size ();
    __can_weighted = true;    
    __delete = true;
    __x = new double [n];
    __y = new double [n];
    __w = new double [n];
    __can_weighted = true;
    int count = 0;
    for (int i = 0; i < n; i++)
      {
	__x[i] = p_[i].x ();
	__y[i] = p_[i].y ();
	if (! p_[i].is_weighted ())
	  {
	    clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
	    __can_weighted = false;
	  }
	__w[i] = p_[i].sigma ();
      }
    __fit_data.n = n;
    __initialized = true;
  }


  void linear_regression::init (size_t npoints_, 
				const double * x_, 
				const double * y_, 
				const double * w_)
  {
    if (npoints_ < 3)
      {
	ostringstream message;
	message << "linear_regression::init: Not enough datapoints!";
	throw runtime_error (message.str ());
      }
    size_t n = npoints_;
    __can_weighted = true;    
    __delete = false;
    __x = const_cast<double *> (x_);
    __y = const_cast<double *> (y_);
    __w = 0;
    __can_weighted = false;
    int count = 0;
    if (w_ != 0)
      {
	__w = const_cast<double *> (w_);
	__can_weighted = true;
	for (int i = 0; i < n; i++)
	  {
	    if (isnan (__w[i]))
	      {
		clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
		__can_weighted = false;
	      }
	  }
      }
    __fit_data.n = n;
    __initialized = true;
  }

  void linear_regression::init (const vector<double> & x_, 
				const vector<double> & y_)
  {
    vector<double> w_no;
    init (x_, y_, w_no);
  }

  void linear_regression::init (const vector<double> & x_, 
				const vector<double> & y_, 
				const vector<double> & w_)
  {
    if (x_.size () < 3)
      {
	ostringstream message;
	message << "linear_regression::init: Not enough datapoints!";
	throw runtime_error (message.str ());
      }
    if (x_.size () != y_.size ())
      {
	ostringstream message;
	message << "linear_regression::init: Data vectors' dimensions do not match!";
	throw runtime_error (message.str ());
      }
    if (w_.size ())
      {
	if (y_.size () != w_.size ())
	  {
	    ostringstream message;
	    message << "linear_regression::init: Error vector's dimension and data vactor do not match!";
	    throw runtime_error (message.str ());
	  }
	__can_weighted = true;    
      }
    else
      {
	__can_weighted = false;    
      }
    size_t n = x_.size ();

    // 2009-10-28 FM: std::vector::data() method is not available:
    /*
    __delete = false;
    __x = const_cast<double *> (x_.data ());
    __y = const_cast<double *> (y_.data ());
    */
    __delete = true;
    __x = new double [n];
    __y = new double [n];
    for (int i = 0; i < n; i++)
      {
	__x[i] = x_[i];
	__y[i] = y_[i];
      }

    __w = 0;
    int count = 0;
    if (w_.size () != 0)
      {
	// 2009-10-28 FM: missing std::vector::data() method:
	//__w = const_cast<double *>(w_.data ());
	__w = new double [n];
	__can_weighted = true;
	for (int i = 0; i < n; i++)
	  {
	    __w[i] = w_[i];
	    if (isnan (__w[i]))
	      {
		clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
		__can_weighted = false;
	      }
	  }
      }
    __fit_data.n = n;
    __initialized = true;
  }

  void linear_regression::reset ()
  {
    __initialized = false;
    __can_weighted = false;
    __fit_data.reset ();
    if (__delete)
      {
	if (__x != 0) 
	  {
	    delete [] __x;
	  }
	if (__y != 0) 
	  {
	    delete [] __y;
	    __y = 0;
	  }
	if (__w != 0) 
	  {
	    delete [] __w;
	  }
      }
    __delete = false;
    __x = 0;
    __y = 0;
    __w = 0;
  }

  const linear_regression::fit_data & linear_regression::get_fit_data () const
  {
    return __fit_data;
  }

  bool linear_regression::fit_linear ()
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::fit_linear: Not initialized!";
	throw runtime_error (message.str ());
      }

    int gsl_err =  gsl_fit_linear (__x, 1,
				   __y, 1,
				   __fit_data.n,
				   &__fit_data.c0,
				   &__fit_data.c1,
				   &__fit_data.cov00,
				   &__fit_data.cov01,
				   &__fit_data.cov11,
				   &__fit_data.sumsq);
    __fit_data.status = gsl_err;
    __fit_data.weighted = false;
    __fit_data.with_constant = true;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_weighted_linear ()
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::fit_linear: Not initialized!";
	throw runtime_error (message.str ());
      }
    if (! __can_weighted)
      {
	ostringstream message;
	message << "linear_regression::fit_linear: Cannot perform weighted fit!";
	throw runtime_error (message.str ());
      }
    int gsl_err =  gsl_fit_wlinear (__x, 1,
				    __w, 1,
				    __y, 1,
				    __fit_data.n,
				    &__fit_data.c0,
				    &__fit_data.c1,
				    &__fit_data.cov00,
				    &__fit_data.cov01,
				    &__fit_data.cov11,
				    &__fit_data.chisq);
    __fit_data.status = gsl_err;
    __fit_data.weighted = true;
    __fit_data.with_constant = true;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_linear_no_constant ()
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::fit_linear_no_constant: Not initialized!";
	throw runtime_error (message.str ());
      }

    int gsl_err =  gsl_fit_mul (__x, 1,
				__y, 1,
				__fit_data.n,
				&__fit_data.c1,
				&__fit_data.cov11,
				&__fit_data.sumsq);
    __fit_data.status = gsl_err;
    __fit_data.weighted = false;
    __fit_data.with_constant = false;
    __fit_data.c0 = 0.0;
    __fit_data.cov00 = 0.0;
    __fit_data.cov01 = 0.0;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_weighted_linear_no_constant ()
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::fit_weighted_linear_no_constant: Not initialized!";
	throw runtime_error (message.str ());
      }
    if (! __can_weighted)
      {
	ostringstream message;
	message << "linear_regression::fit_weighted_linear_no_constant: Cannot perform weighted fit!";
	throw runtime_error (message.str ());
      }
    int gsl_err =  gsl_fit_wmul (__x, 1,
				 __w, 1,
				 __y, 1,
				 __fit_data.n,
				 &__fit_data.c1,
				 &__fit_data.cov11,
				 &__fit_data.chisq);
    __fit_data.status = gsl_err;
    __fit_data.weighted = true;
    __fit_data.with_constant = false;
    __fit_data.c0 = 0.0;
    __fit_data.cov00 = 0.0;
    __fit_data.cov01 = 0.0;
    return gsl_err == GSL_SUCCESS;
  }
   
  void linear_regression::eval_err (double x_, 
				    double & y_, 
				    double & y_err_) const
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::eval: Not initialized!";
	throw runtime_error (message.str ());
      }
    int gsl_err;
    if (__fit_data.has_constant ())
      {
	gsl_err = gsl_fit_linear_est (x_,
				      __fit_data.c0, 
				      __fit_data.c1, 
				      __fit_data.cov00, 
				      __fit_data.cov01, 
				      __fit_data.cov11, 
				      &y_,
				      &y_err_);
      }
    else
      {
	gsl_err = gsl_fit_mul_est (x_,
				   __fit_data.c1, 
				   __fit_data.cov11, 
				   &y_,
				   &y_err_);
      }
    if (gsl_err != GSL_SUCCESS)
      {
	ostringstream message;
	message << "linear_regression::eval_err: Cannot evaluate value!";
	throw runtime_error (message.str ());
      }
  }

  double linear_regression::eval (double x_) const
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "linear_regression::eval: Not initialized!";
	throw runtime_error (message.str ());
      }
    
    double y, y_err;
    eval_err (x_, y, y_err);
    return y;
  }
  
}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::linear_regression.cc
