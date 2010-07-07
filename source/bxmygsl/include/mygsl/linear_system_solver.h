// gsl::linear_system_solver.h

#ifndef __gsl__linear_system_solver_h
#define __gsl__linear_system_solver_h 1

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

#include <iostream>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <vector>
#include <cmath>
#include <stdexcept>

/*
 * Solve:  A.X = B
 *
 *     ( a00 a10 a20 )
 * A = ( a01 a11 a21 )
 *     ( a02 a12 a22 )
 *
 *     ( b0 )
 * B = ( b1 )
 *     ( b2 )
 *
 *     ( x0 )
 * X = ( x1 )
 *     ( x2 )
 *
 */

namespace mygsl {

  class linear_system_solver 
  {
  public:
    static bool g_debug;

  private:

    size_t              __dimension;
    bool __use_gsl_vector;
    std::vector<double> __va;
    std::vector<double> __vb;
    gsl_vector *        __vva;
    gsl_vector *        __vvb;
    gsl_matrix_view     __m;
    gsl_vector_view     __b;
    gsl_permutation *   __p;
    gsl_vector *        __x;

  public:

    bool is_initialized () const;

    size_t get_dimension () const;
    
    linear_system_solver (size_t dimension_ = 0);

    virtual ~linear_system_solver ();

  private:

    void __reset ();

    void __init (size_t dimension_);

  public:

    int solve (const std::vector<double> & a_, 
	       const std::vector<double> & b_,
	       std::vector<double>       & x_);
    
  };

  /* Solve:
   *
   *  a1 x + b1 y = c1
   *  a2 x + b2 y = c2 
   *
   */
  bool linear_system_2x2_solve (double a1_, double b1_, double c1_,
				double a2_, double b2_, double c2_,
				double & x_, double & y_);
  
} // end of namespace mygsl

#endif // __gsl__linear_system_solver_h

// end of gsl::linear_system_solver.h
