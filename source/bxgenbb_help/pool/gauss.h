// -*- mode: c++; -*-
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <gsl/gsl_integration.h>

#ifndef __genbb_help__gauss_h
#define __genbb_help__gauss_h 1

extern "C"
{

  // typedef float (*f77_func_t) (float *);

  // double gauss_wrapper_func (double x_, void * params)
  // {
  //   double res;
  //   float x = (float) x_;
  //   f77_func_t f = (f77_func_t) params;
  //   float y = f (&x);
  //   res = (double) y;
  //   return res;
  // }

  // float gauss_wrap (f77_func_t func_, float * min_, float * max_, float * relerr_);

  /* typedef void (*f77_dgmlk_subroutine_t) (int32_t * m_,  */
  /* 					  double * uk_, */
  /* 					  double * fk_, */
  /* 					  double * x_); */

  
  /* double dgmlk1_wrap (f77_dgmlk_subroutine_t fsubk_,  */
  /* 		      double * ak_, double * bk_,  */
  /* 		      int32_t * nik_, int32_t * ngk_,  */
  /* 		      double * x_); */

}

#endif // __genbb_help__gauss_h 1

// end of gauss.h
