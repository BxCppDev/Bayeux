// linear_system_solver.h

#ifndef MYGSL_LINEAR_SYSTEM_SOLVER_H_
#define MYGSL_LINEAR_SYSTEM_SOLVER_H_ 1

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

#include <iostream>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <vector>

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

    bool is_initialized () const;

    size_t get_dimension () const;

    linear_system_solver (size_t dimension_ = 0);

    virtual ~linear_system_solver ();

    int solve (const std::vector<double> & a_,
	       const std::vector<double> & b_,
	       std::vector<double>       & x_);

  private:

    void _reset_ ();

    void _init_ (size_t dimension_);

  private:

    size_t              _dimension_;
    //bool                _use_gsl_vector_;
    std::vector<double> _va_;
    std::vector<double> _vb_;
    gsl_vector *        _vva_;
    gsl_vector *        _vvb_;
    gsl_matrix_view     _m_;
    gsl_vector_view     _b_;
    gsl_permutation *   _p_;
    gsl_vector *        _x_;

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

#endif // MYGSL_LINEAR_SYSTEM_SOLVER_H_

// end of linear_system_solver.h
