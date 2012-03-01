// gauss.cc

#include <genbb_help/gauss.h>

// float gauss_wrap (f77_func_t func_, float * min_, float * max_, float * relerr_)
// {
//   float res = 0.0;
//   size_t ws_sz = 1000;
//   gsl_integration_workspace * ws
//     = gsl_integration_workspace_alloc (ws_sz);
//   double result, error;
//   gsl_function F;
//   F.function = &gauss_wrapper_func;
//   F.params = &func_;
//   double min = (double) *min_;
//   double max = (double) *max_;
//   double epsabs = 0.0;
//   double epsrel = (double) *relerr_;
//   gsl_integration_qags (&F, min, max, 
// 			epsabs, epsrel, 
// 			ws_sz, ws, 
// 			&result, &error);
//   res = (float) result;
//   gsl_integration_workspace_free (ws);
//   return res;
// }

// end of gauss.cc
