// test_multifit_nlin.cxx


#include <iostream>
#include <stdexcept>
#include <vector>

#include <gsl/gsl_multifit_nlin.h>

using namespace std;

struct data
{
  size_t n;
  vector<double> t;
  vector<double> y;
  vector<double> sigma_y;
};

int f (const gsl_vector * x_,
       void * data_,
       gsl_vector * f_)
{
  const data & d = static_cast<const data & >(*data_);
  size_t n = d.n;
  const vector<double> & t = d.t;
  const vector<double> & y = d.y;
  const vector<double> & sigma_y = d.sigma_y;

  double A = gsl_vector_get (x_, 0);
  double lambda = gsl_vector_get (x_, 1);
  double b = gsl_vector_get (x_, 2);

  for (int i = 0; i < n; i++)
    {
      double ti = t[i];
      double yi = y[i];
      double sigma_i = sigma_y[i];
      double yi_model = A * exp (-lambda * ti) +  b;
      gsl_vector_set (f_, i, (yi_model - yi) / sigma_i);
    }
  return GSL_SUCCESS;
}

int df (const gsl_vector * x_,
	void * data_,
	gsl_matrix * J_)
{
  return GSL_SUCCESS;
}

int fdf (const gsl_vector * x_,
	 void * data_,
	 gsl_vector * f_,
	 gsl_matrix * J_)
{
  f (x_, data_, f_);
  df (x_, data_, f_, J_);
  return GSL_SUCCESS;
}



class multifit_nlin
{
    public:
      static size_t DEFAULT_MAX_ITER;
      static bool   DEFAULT_VERBOSE;
  const gsl_multifit_fdfsolver_type * __T;
  gsl_multifit_fdfsolver * __s;
  gsl_matrix * __covar;
  size_t __n;
  size_t __p;
public:
  multifit_nlin (size_t n_, size_t p_);
  virtual ~multifit_nlin ();
  void init (size_t n_, size_t p_);
  void reset ();
};

multifit_nlin::multifit_nlin (size_t n_, size_t p_)
{
  __T = 0;
  __s = 0;
  __covar = 0;
  init (n_, p_);
}

multifit_nlin::~multifit_nlin ()
{
  reset ();
}

void multifit_nlin::reset ()
{
  if (__s != 0)
    {
      gsl_multifit_fdfsolver_free (__s);
      __s = 0;      
    }
  if (__covar != 0)
    {
      gsl_matrix_free (__covar);
      __covar = 0;      
    }
}

void multifit_nlin::init (size_t n_, size_t p_)
{
  __n = n_;
  __p = p_;
  __covar = gsl_matrix_alloc (__p, __p);
  
}

int main (void)
{
  try 
    {
      const gsl_multifit_fdfsolver_type * T;
      gsl_multifit_fdfsolver * s;
      int status;
      unsigned int i, iter = 0;
      const size_t n = 40;
      const size_t p = 3;
      gsl_matrix * covar = gsl_matric_alloc (p, p);
    }
  catch (runtime_error & e)
    {
      cerr << "ERROR: " << e.what () << endl;
    }
}

// end of test_multifit_nlin.cxx
