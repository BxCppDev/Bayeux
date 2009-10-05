// mygsl::von_neumann_method.cc

#include <mygsl/von_neumann_method.h>

namespace mygsl {

  bool von_neumann_method::g_debug = false;
  const double  von_neumann_method::AUTO_FMAX = 0.0;

  von_neumann_method::von_neumann_method (double xmin_, 
					  double xmax_, 
					  unary_eval & func_, 
					  double fmax_,
					  size_t nsamples_,
					  size_t max_counts_)
  {
    init (xmin_, xmax_, func_, fmax_, nsamples_, max_counts_);
  }

  void von_neumann_method::dump (std::ostream & out_) const
  {
    out_ << "von_neumann_method:" << std::endl;
    out_ << "|-- x(min)      : " << __xmin << std::endl;
    out_ << "|-- x(max)      : " << __xmax << std::endl;
    out_ << "|-- f(max)      : " << __fmax << std::endl;
    out_ << "|-- func        : " << std::hex << __func 
	 << std::dec << std::endl;
    out_ << "`-- counts(max) : " << __max_counts << std::endl;
  }

  void von_neumann_method::init (double xmin_, 
				 double xmax_, 
				 unary_eval & func_, 
				 double fmax_,
				 size_t nsamples_,
				 size_t max_counts_)
  {
    if (g_debug)
      {
	std::clog << "DEBUG: von_neumann_method::init: Entering..." 
		  << std::endl;
      }
    if (xmin_ > xmax_)
      {
	throw std::runtime_error ("von_neumann_method::init: invalid range!");
      }
    __xmin = xmin_;
    __xmax = xmax_;
    if (max_counts_ < 10)
      {
	throw std::runtime_error ("von_neumann_method::init: invalid maximum number of tries!");
      }
    __max_counts = max_counts_;
    __func = &func_;

    if (fmax_ > AUTO_FMAX)
      {
	__fmax = fmax_;
      }
    else 
      {
	if (g_debug)
	  {
	    std::clog << "DEBUG: von_neumann_method::init: searching for 'f(max)'..." 
		  << std::endl;
	  }
	double fmax = -1.0;
	double f1 = -1.0;
	double f2 = -1.0;
	double dfmax = -1.0;
	if (nsamples_ < 2)
	  {
	    throw std::runtime_error ("von_neumann_method::init: Invalid sampling!");
	  }
	double dx = (__xmax - __xmin) / nsamples_;
	for (double x = __xmin;
	     x <  (__xmax + 0.5 *dx);
	     x += dx)
	  {
	    double f = (*__func) (x);
	    if (f1 < 0.0)
	      {
		f1 = f;
	      }
	    else 
	      {
		f1 = f2;
	      }
	    f2 = f;
	    if ( abs (f) > fmax)
	      {
		fmax = abs (f);
		double df = (f2 - f1) / dx;
		if (abs (df) > dfmax) dfmax = abs (df);
	      }
	  }
	__fmax = fmax + dfmax * 2. * dx;
	if (g_debug)
	  {
	    std::clog << "DEBUG: von_neumann_method::init: found 'f(max)' = "
		      << __fmax << std::endl;
	  }
      }
    if (g_debug)
      {
	std::clog << "DEBUG: von_neumann_method::init: Exiting." 
		  << std::endl;
      }
  }

  double von_neumann_method::shoot (rng & ran_)
  {
    double res;
    size_t max_counts = __max_counts;
    size_t counts = 0;
    while (true)
      {
	double x = ran_.flat (__xmin, __xmax);
	double y = ran_.flat (0.0, __fmax);
	counts++;
	double f = (*__func) (x);
	if (y < f)
	  {
	    res = x;
	    break;
	  } 
	if ((max_counts > 0) && (counts > max_counts))
	  {
	    throw std::runtime_error ("von_neumann_method::shoot: maximum number of tries has been reached!");
	  }
      } 
    return res;
  }

}

// end of mygsl::von_neumann_method.cc

