// mygsl::linear_system_solver.cc

#include <mygsl/linear_system_solver.h>

#include <cmath>
#include <stdexcept>

namespace mygsl {

  using namespace std;

  bool linear_system_solver::g_debug = false;

  linear_system_solver::linear_system_solver (size_t dimension_)
  {
    __dimension = 0;
    __p = 0;
    __x = 0; 
    __vva = 0;
    __vvb = 0;
    if (dimension_ > 0) __init (dimension_);
  }

  linear_system_solver::~linear_system_solver ()
  {
    __reset ();
  }

  void linear_system_solver::__reset ()
  {
    if (g_debug) 
      {
	std::clog << "DEVEL: linear_system_solver::__reset: entering..." 
		  << endl;
      }
    if (__x) gsl_vector_free (__x);
    if (__p) gsl_permutation_free (__p);

    gsl_vector_free (__vva);
    gsl_vector_free (__vvb);
    __vva = 0;
    __vvb = 0;
    //__va.clear ();
    //__vb.clear ();
    __dimension = 0;
  }

  void linear_system_solver::__init (size_t dimension_)
  {
    if (g_debug) 
      {
	std::clog << "DEVEL: linear_system_solver::__init: entering..." 
		  << endl;
      }
    if (dimension_ < 1)
      {
	throw runtime_error ("linear_system_solver::__init: Invalid dimension");
	return;
      }
    __dimension = dimension_;
    //__va.assign (__dimension * __dimension, 0.0);
    //__vb.assign (__dimension, 0.0);
    __vva = gsl_vector_calloc (__dimension * __dimension);
    __vvb = gsl_vector_calloc (__dimension);    
    __m = gsl_matrix_view_array (__vva->data, __dimension, __dimension);
    __b = gsl_vector_view_array (__vvb->data, __dimension);
    __p = gsl_permutation_alloc (__dimension);
    __x = gsl_vector_alloc (__dimension);
  }

  bool linear_system_solver::is_initialized () const
  {
    return (__dimension > 0);
  }

  size_t linear_system_solver::get_dimension () const
  {
    return __dimension;
  }

  int linear_system_solver::solve (const vector<double> & a_, 
				   const vector<double> & b_,
				   vector<double> & x_)
  {
    bool debug = false;
    // checks:
    size_t dim = b_.size ();
    if (a_.size () != (dim * dim))
      {
	std::cerr << "ERROR: linear_system_solver::solve: Invalid system dimensions!" << endl;
	return EXIT_FAILURE;
      }
    if (dim != __vvb->size)
      {
	std::clog << "WARNING: linear_system_solver::solve: Redimensioning system!" << endl;
	__reset ();
	__init (dim);
      }
    // input values:
    for (int i = 0; i < a_.size (); i++)
      {
	gsl_vector_set (__vva, (size_t) i, a_[i]);
      }
    for (int i = 0; i < b_.size (); i++)
      {
	gsl_vector_set (__vvb, (size_t) i, b_[i]);
      }
    // computing:
    int s;
    if (gsl_linalg_LU_decomp (&__m.matrix, __p, &s))
      {
	std::cerr << "ERROR: linear_system_solver::solve: Cannot do LU decomposition!" << endl;
	return EXIT_FAILURE;
      }
    if (gsl_linalg_LU_solve (&__m.matrix, __p, &__b.vector, __x))
      {
	std::cerr << "ERROR: linear_system_solver::solve: Cannot do LU solving!" << endl;
	return EXIT_FAILURE;
      }
    if (debug)
      {
	clog << "DEBUG: linear_system_solver::solve: x = (";
	for (int i = 0; i < __dimension; i++)
	  {
	    clog << ' ' << gsl_vector_get (__x, i);
	  }
	clog << ')' << endl;
      }
    // output values:
    x_.reserve (__dimension);
    double check = 0.0;
    for (int i = 0; i < __dimension; i++)
      {
	x_[i] = gsl_vector_get (__x, i);
	check += x_[i];
      }
    if (! isnormal (check))
      {
	return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }

  /* Solve:
   *
   *  a1 x + b1 y = c1
   *  a2 x + b2 y = c2 
   *
   */
  bool linear_system_2x2_solve (double a1_, double b1_, double c1_,
				double a2_, double b2_, double c2_,
				double & x_, double & y_)
  {
    double det = b1_ * a2_ - a1_ * b2_;
    if (abs (det) < 1.e-15) 
      {
	return false;
      }
    x_ = (b1_ * c2_ - c1_ * b2_) / det;
    y_ = (c1_ * a2_ - c2_ * a1_) / det;
    return true;
  }

}

// end of mygsl::linear_system_solver.cc
