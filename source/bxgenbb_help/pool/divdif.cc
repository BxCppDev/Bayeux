// divdif.cc

#include <genbb_help/divdif.h>

float divdif_wrap (float * f_, float * a_, int32_t * n_, 
		   float * x_, int32_t * degree_)
{
  const size_t MAXSZ = 10000;
  double x[MAXSZ], y[MAXSZ];
  double x_value = (double) *x_;
  size_t npoints = *n_;
  for (int i = 0; i < npoints; i++)
    {
      x[i] = (double) a_[i];
      y[i] = (double) f_[i];
    }
  const gsl_interp_type * git = 0;
  //git = gsl_interp_linear;
  //git = gsl_interp_polynomial;
  git = gsl_interp_cspline;
  gsl_spline * spline = gsl_spline_alloc (git, npoints);
  size_t min_npoints  = gsl_spline_min_size (spline);
  std::string name = gsl_spline_name (spline);
  if (npoints < min_npoints) 
    {
      std::ostringstream message;
      message << "genbb_help::divdif_wrap: "
	      << "Not enough data points for the '"
	      << name << "' interpolator !";
      if (spline != 0) gsl_spline_free (spline);
      spline = 0;
      throw std::logic_error (message.str ());     
    }
  gsl_interp_accel * acc = gsl_interp_accel_alloc ();
  gsl_spline_init (spline, x, y, npoints);
  double extrapolated_f = gsl_spline_eval (spline, x_value, acc);
  float res = (float) extrapolated_f;
  gsl_interp_accel_free (acc);
  gsl_spline_free (spline);
  return res;
}

// end of divdif.cc
