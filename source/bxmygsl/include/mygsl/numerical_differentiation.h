// -*- mode: c++; -*- 
/* mygsl::numerical_differentiation.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2009-10-02
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Interface for GSL numerical differentiation
 *
 * History: 
 * 
 */

#ifndef __mygsl__numerical_differentiation_h
#define __mygsl__numerical_differentiation_h 1

#include <mygsl/unary_eval.h>
#include <mygsl/interval.h>

using namespace std;

namespace mygsl {

  double derivative_central (const unary_eval & eval_, double x_, double h_, double & abserr_ );

  double derivative_central (const unary_eval & eval_, double x_, double h_);

  double derivative_forward (const unary_eval & eval_, double x_, double h_, double & abserr_ );

  double derivative_forward (const unary_eval & eval_, double x_, double h_);

  double derivative_backward (const unary_eval & eval_, double x_, double h_, double & abserr_ );

  double derivative_backward (const unary_eval & eval_, double x_, double h_);

  double derivative (const unary_eval & eval_, double x_, const interval & domain_, double & abserr_);

  double derivative (const unary_eval & eval_, double x_, const interval & domain_);

  /*****************************************************
   *                                                   *
   *   Derivative formula for regular sampled function *
   *                                                   *
   *****************************************************/

  // T.J. Chung:

  // central:

  // first order derivative:
  double derivative_central_regular_sampled_2 (double f_1_, 
					       double f1_, 
					       double h_);

  double derivative_central_regular_sampled_4 (double f_2_, 
					       double f_1_, 
					       double f1_, 
					       double f2_, 
					       double h_);
  
  // second order derivative:
  double second_derivative_central_regular_sampled_2 (double f_1_, 
						      double f0_, 
						      double f1_, 
						      double h_);


  double second_derivative_central_regular_sampled_4 (double f_2_, 
						      double f_1_, 
						      double f0_, 
						      double f1_, 
						      double f2_, 
						      double h_);


  // forward:

  double derivative_forward_regular_sampled_1 (double f0_, 
					       double f1_, 
					       double h_);

  double derivative_forward_regular_sampled_2 (double f0_, 
					       double f1_, 
					       double f2_, 
					       double h_);

  double derivative_forward_regular_sampled_3 (double f0_, 
					       double f1_, 
					       double f2_, 
					       double f3_, 
					       double h_);

  double derivative_forward_regular_sampled_4 (double f0_, 
					       double f1_, 
					       double f2_,  
					       double f3_,  
					       double f4_, 
					       double h_); 

  double second_derivative_forward_regular_sampled_1 (double f0_, 
						      double f1_, 
						      double f2_, 
						      double h_);

  double second_derivative_forward_regular_sampled_2 (double f0_, 
						      double f1_, 
						      double f2_, 
						      double f3_, 
						      double h_);

  // backward:

  double derivative_backward_regular_sampled_1 (double f_1_, 
						double f0_, 
						double h_);


  double derivative_backward_regular_sampled_2 (double f_2_, 
					        double f_1_, 
						double f0_, 
						double h_);
  
  double derivative_backward_regular_sampled_3 (double f_3_, 
						double f_2_, 
						double f_1_, 
						double f0_, 
						double h_);
  
  double derivative_backward_regular_sampled_4 (double f_4_,
						double f_3_,  
						double f_2_, 
						double f_1_, 
						double f0_, 
						double h_);

  double second_derivative_backward_regular_sampled_1 (double f_2_, 
						       double f_1_, 
						       double f0_, 
						       double h_);

  double second_derivative_backward_regular_sampled_2 (double f_3_, 
						       double f_2_, 
						       double f_1_, 
						       double f_0_, 
						       double h_);



} // end of namespace mygsl

#endif // __mygsl__numerical_differentiation_h

// end of mygsl::numerical_differentiation.h
