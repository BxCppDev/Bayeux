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

#include <functional>
#include <limits>

#include <gsl/gsl_deriv.h>

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


} // end of namespace mygsl

#endif // __mygsl__numerical_differentiation_h

// end of mygsl::numerical_differentiation.h
